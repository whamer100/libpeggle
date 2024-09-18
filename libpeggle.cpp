#include "libpeggle.h"

#define WIN32_LEAN_AND_MEAN
#include <fstream>
#include <ranges>
#include <Windows.h>

#include "binstream.h"
#include "iohelper.h"
#include "logma.h"

#include "macros.h"

// PakInterface
constexpr uint32_t PAK_MAGIC = 0xBAC04AC0;
constexpr uint8_t FILEFLAGS_END = 0x80;

// Microsoft stuff
constexpr uint64_t EPOCH_AS_FILETIME = 116444736000000000;

using namespace iohelper;

namespace Peggle {

#pragma region libpeggle_Generic

    void change_logging(const log_mode_e log_mode) {
        switch (log_mode) {
            case LogDefault: {
                log_disableCompletely(false);
                log_setVerbosity(false);
                break;
            }
            case LogVerbose:{
                log_disableCompletely(false);
                log_setVerbosity(true);
                break;
            }
            case LogDisable:{
                log_disableCompletely(true);
                log_setVerbosity(false);
                break;
            }
        }
    }

    std::chrono::file_clock::time_point FileTimeToTimePoint(const uint64_t ft) {
        const std::chrono::file_clock::duration d{ft};
        return std::chrono::file_clock::time_point(d);
    }

    struct PopcapString {
        uint8_t Length;
        char* Data;

        [[nodiscard]]
        std::string toString() const {
            return std::string{Data};
        }
        PopcapString() = default;
        explicit PopcapString(const uint8_t size) {
            Length = size;
            Data = (char*)calloc(1, size + 1);
        }
    };

    struct PakRecord {
        PopcapString FileName{};
        std::chrono::file_clock::time_point FileTime;
        uint32_t StartPos{};
        uint32_t Size{};
        char* Data{};
        std::string toString() {
            const auto system_time_point = std::chrono::clock_cast<std::chrono::system_clock>(FileTime);
            auto local_time = std::chrono::zoned_time{std::chrono::current_zone(), system_time_point};

            return std::format("PakRecord {{ {}, {}, {}, {} }}",
                FileName.Data, local_time, StartPos, Size
                );
        }
    };

#pragma endregion

#pragma region libpeggle_Pak

    Pak::Pak(const std::filesystem::path &path) {
        fp = nullptr;
        Valid = false;
        Xor = 0x00;
        Version = 0;
        if (!exists(path)) {
            Valid = false;
            return;
        }
        if (is_directory(path))
            LoadFolder(path);
        else
            LoadPak(path);
    }

    FileRef Pak::GetFile(const std::string& Path) const {
        if (!HasFile(Path))
            return {
                FileState::DoesNotExist,
                nullptr,
                0
            };
        const auto& rec = FileTable.at(Path);
        return {
            FileState::OK,
            rec.Data,
            rec.Size
        };
    }

    bool Pak::HasFile(const std::string& Path) const {
        return FileTable.contains(Path);
    }

    FileState Pak::UpdateFile(const std::string& Path, const void* Data, const uint32_t Size) {
        return UpdateFile(Path, Data, Size, std::chrono::file_clock::now());
    }

    FileState Pak::UpdateFile(const std::string& Path, const void* Data, const uint32_t Size, const std::chrono::file_clock::time_point Timestamp) {
        if (!HasFile(Path)) return FileState::InvalidOperation;
        RemoveFile(Path);
        AddFile(Path, Data, Size, Timestamp);
        return FileState::OK;
    }

    FileState Pak::AddFile(const std::string& Path, const void* Data, const uint32_t Size) {
        return AddFile(Path, Data, Size, std::chrono::file_clock::now());
    }

    FileState Pak::AddFile(const std::string& Path, const void* Data, const uint32_t Size, const std::chrono::file_clock::time_point Timestamp) {
        if (HasFile(Path)) return FileState::InvalidOperation;
        if (Path.size() > UINT8_MAX) {
            log_fatal("File \"%s\" has too large of a file name! (%d > 255)\n",
                Path.c_str(), Path.size());
            return FileState::InvalidOperation;
        }
        const auto pstr_len = static_cast<uint8_t>(Path.size());
        const auto pstr = PopcapString(pstr_len);
        memcpy(pstr.Data, Path.c_str(), pstr_len);
        const auto rec = PakRecord {
            pstr,
            Timestamp,
            0,  // not being read from pak data, we can ignore this
            Size,
            (char*)malloc(Size)
        };
        memcpy(rec.Data, Data, rec.Size);
        FileTable[Path] = rec;
        return FileState::OK;
    }

    FileState Pak::RemoveFile(const std::string& Path) {
        if (!HasFile(Path)) return FileState::InvalidOperation;
        FileTable.erase(Path);
        UpdateFileList();
        return FileState::OK;
    }

    void Pak::LoadPak(const std::filesystem::path &path) {
        const auto* fpath = path.c_str();
        char fpath_str[512] = {0};
        size_t NumOfCharConverted;
        wcstombs_s(&NumOfCharConverted, fpath_str, fpath, 512);

        if (const auto err = _wfopen_s(&fp, fpath, L"rb")) {
            char buf[128] = {0};
            strerror_s(buf, sizeof buf, err);
            fwprintf_s(stderr, L"cannot open file '%s': %hs\n",
                      fpath, buf);
        }

        log_debug("Loading file \"%s\".\n", fpath_str);

        const uint32_t magic = read_uint32le(fp);
        if (magic == PAK_MAGIC) {
            Xor = 0x00;
            Valid = true;
        }
        if ((magic ^ 0xF7F7F7F7) == PAK_MAGIC) {
            Xor = 0xF7;
            Valid = true;
        }
        set_xor(Xor);

        if (Valid)
            log_debug("PopCap Pak file format found.\n");

        Version = read_uint32le(fp);
        log_debug("Pak version: %d %s\n", Version, Version > 0 ? "(Unexpected version! Errors may occur.)" : "");
        log_debug("Parsing file table...\n");

        size_t pos = 0;
        forever {
            const uint8_t flags = read_uint8(fp);
            if (flags & FILEFLAGS_END)
                break;

            const uint8_t flen = read_uint8(fp);
            auto pstr = PopcapString(flen);
            read_bytes(fp, pstr.Data, flen);
            xor_bytes(pstr.Data, flen);

            const auto src_size = read_uint32le(fp);
            const auto file_time = read_uint64le(fp);

            auto rec = PakRecord {
                pstr,
                FileTimeToTimePoint(file_time),
                pos,
                src_size,
                nullptr
            };
            FileTable[pstr.toString()] = rec;

            pos += src_size;
        }
        const auto header_size = ftell(fp);

        log_debug("Parsed %d file(s).\n", FileTable.size());
        log_debug("Reading files...\n");

        for (auto &rec: FileTable | std::views::values) {
            auto* buf = static_cast<char*>(malloc(rec.Size));
            fseek(fp, rec.StartPos + header_size, 0);
            read_bytes(fp, buf, rec.Size);
            xor_bytes(buf, rec.Size);
            rec.Data = buf;
        }
        log_debug("Done reading file data.\n");
    }

    void Pak::LoadFolder(const std::filesystem::path &path) {
        log_debug("Loading folder \"%s\".\n", path.generic_string().c_str());
        for (const auto& dir : std::filesystem::recursive_directory_iterator(path)) {
            if (!dir.is_regular_file())
                continue;  // we dont actually care about directories, just the relative path that includes them
            const auto& file = dir.path();
            const auto relative = std::filesystem::relative(file, path);
            const auto relative_str = relative.generic_string();
            std::string rel_str(relative_str.length(), 0);
            std::ranges::replace_copy(relative_str, rel_str.begin(), '/', '\\');
            const auto pstr_len = relative.generic_string().length();
            // if (pstr_len > 0) {
            if (pstr_len > UINT8_MAX) {
                log_fatal("File \"%s\" has too large of a file name! (%d > 255)\n",
                    relative.generic_string().c_str(), pstr_len);
                log_info("Skipping file...\n");
                continue;
            }
            const auto pstr = PopcapString(static_cast<uint8_t>(pstr_len));
            memcpy(pstr.Data, rel_str.c_str(), pstr_len);
            const std::chrono::file_clock::time_point modified_time = std::filesystem::last_write_time(file);
            const auto file_size = std::filesystem::file_size(file);
            if (file_size > UINT32_MAX) {
                log_fatal("File \"%s\" has too large of a file size! (%d > %d)\n",
                    relative.generic_string().c_str(), pstr_len, UINT32_MAX); // i was too lazy to type out the size lol
                log_info("Skipping file...\n");
                continue;
            }

            auto* file_data = static_cast<char*>(malloc(file_size));
            std::ifstream fs(file, std::ifstream::in | std::ifstream::binary);
            fs.read(file_data, static_cast<std::streamsize>(file_size));
            fs.close();

            const auto rec = PakRecord {
                pstr,
                modified_time,
                0,
                static_cast<uint32_t>(file_size),
                file_data
            };
            FileTable[pstr.toString()] = rec;
        }
        log_debug("Done reading %d file(s).\n", FileTable.size());
    }

    bool Pak::IsPak() const {
        return Valid;
    }

    void Pak::SetXor(const uint8_t Xor) {
        this->Xor = Xor;
    }

    void Pak::UpdateFileList() {
        FileList.clear();
        for (auto &file: FileTable | std::views::keys) {
            FileList.push_back(file);
        }
    }

    const std::vector<std::string>& Pak::GetFileList() {
        UpdateFileList();
        return FileList;
    }

    void Pak::Save(const std::filesystem::path &path) const {
        binstream bs;

        bs << PAK_MAGIC;
        bs << Version;

        // write file table
        for (auto &rec: FileTable | std::views::values) {
            const auto& file_name = rec.FileName;
            bs.write<uint8_t>(0x00);  // entry flags
            bs.write<uint8_t>(file_name.Length);
            bs.write(file_name.Data, file_name.Length);
            bs.write<uint32_t>(rec.Size);
            bs.write<uint64_t>(rec.FileTime.time_since_epoch().count());
        }
        bs.write<uint8_t>(FILEFLAGS_END);

        // write file block
        for (auto &rec: FileTable | std::views::values) {
            bs.write(rec.Data, rec.Size);
        }

        auto transform_xor = Xor;
        bs.transform([&transform_xor](const uint8_t& it) {
            return it ^ transform_xor;
        });

        // log_info("%d\n", bs.size());

        std::ofstream out_fs(path, std::ofstream::out | std::ofstream::binary);
        out_fs.write(reinterpret_cast<const char*>(bs.buffer()), bs.size());
        out_fs.close();
    }

    void Pak::Export(const std::filesystem::path& path) const {
        for (auto &[file_name, rec]: FileTable) {
            const auto out_path = path / file_name;
            const auto out_path_base = out_path.parent_path();
            std::filesystem::create_directories(out_path_base);

            std::ofstream out_fs(out_path, std::ofstream::out | std::ofstream::binary);
            out_fs.write(rec.Data, rec.Size);
            out_fs.close();

            std::filesystem::last_write_time(out_path, rec.FileTime);
        }
    }

    Pak::~Pak() {
        SAFE_FCLOSE(fp);
    }

#pragma endregion

#pragma region libpeggle_Level

#pragma endregion

}
