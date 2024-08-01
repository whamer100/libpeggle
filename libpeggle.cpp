#include "libpeggle.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

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

        std::string toString() const {
            return std::string{Data};
        }
    };

    struct PakRecord {
        PopcapString FileName{};
        std::chrono::file_clock::time_point FileTime;
        uint32_t StartPos{};
        uint32_t Size{};
        std::string toString() {
            auto system_time_point = std::chrono::clock_cast<std::chrono::system_clock>(FileTime);
            auto local_time = std::chrono::zoned_time{std::chrono::current_zone(), system_time_point};

            return std::format("PakRecord {{ {}, {}, {}, {} }}",
                FileName.Data, local_time, StartPos, Size
                );
        }
    };

    struct PakEntry {
        PopcapString FileName{};
        std::chrono::file_clock::time_point FileTime;
        size_t Size{};
        char* Data{};
    };

    Pak::Pak(const std::filesystem::path &path) {
        fp = nullptr;
        Valid = false;
        Xor = 0x00;
        if (!exists(path)) {
            Valid = false;
            return;
        }
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
        if (magic ^ 0xF7F7F7F7 == PAK_MAGIC) {
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
            auto pstr = PopcapString{flen};
            pstr.Data = static_cast<char*>(calloc(1, flen + 1));
            read_bytes(fp, pstr.Data, flen);
            xor_bytes(pstr.Data, flen);

            const auto src_size = read_uint32le(fp);
            const auto file_time = read_uint64le(fp);

            auto rec = PakRecord {
                pstr,
                FileTimeToTimePoint(file_time),
                pos,
                src_size
            };
            PakCollection.emplace_back(rec);
            FileList.emplace_back(rec.FileName.toString());
            // std::printf("%s\n", rec.toString().c_str());

            pos += src_size;
        }
        // and now we must correct the file starts, because just like me, PopCap is lazy.
        const auto offset = ftell(fp);
        for (auto& rec: PakCollection) {
            rec.StartPos += offset;
        }
        log_debug("Parsed %d file(s).\n", PakCollection.size());
        log_debug("Reading files...\n");

        for (auto& rec: PakCollection) {
            auto entry = PakEntry {
                rec.FileName,
                rec.FileTime,
                rec.Size,
                nullptr
            };
            auto* buf = static_cast<char*>(malloc(entry.Size));
            fseek(fp, rec.StartPos, 0);
            read_bytes(fp, buf, rec.Size);
            xor_bytes(buf, rec.Size);
            entry.Data = buf;
            PakEntries.emplace_back(entry);
        }
        log_debug("Done reading file data.\n");
    }

    bool Pak::IsPak() const {
        return Valid;
    }

    uint32_t Pak::GetVersion() const {
        return Version;
    }

    // TODO: write pak to folder
    void Pak::Save(const std::filesystem::path &path) {

    }

    Pak::~Pak() {
        SAFE_FCLOSE(fp);
    }


}
