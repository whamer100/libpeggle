#ifndef LIBPEGGLE_H
#define LIBPEGGLE_H

#include <cstdint>
#include <filesystem>
#include <map>

namespace Peggle {
    // forward declarations for types
    struct PakRecord;

    enum class FileState : bool {
        OK = true,
        // both of these return false, but their implications are the same.
        // just doing this for clarity in code
        DoesNotExist = false,
        InvalidOperation = false,
    };

    struct FileRef {
        const FileState State;
        const void* Data;
        const uint32_t Size;
    };

    class Pak {
    public:
        // open pak file or folder
        explicit Pak(const std::filesystem::path& path);
        // save pak to file
        void Save(const std::filesystem::path& path) const;
        // save pak to folder
        void Export(const std::filesystem::path& path) const;

        [[nodiscard]]
        // get file reference (immutable)
        FileRef GetFile(const std::string& Path) const;
        [[nodiscard]]
        // check if file exists
        bool HasFile(const std::string& Path) const;
        // replace file data
        FileState UpdateFile(const std::string& Path, const void* Data, uint32_t Size);
        // replace file data (update modified timestamp)
        FileState UpdateFile(const std::string& Path, const void* Data, uint32_t Size, std::chrono::file_clock::time_point Timestamp);
        // insert new file at path (modified timestamp automatically set to the timestamp of invocation)
        FileState AddFile(const std::string& Path, const void* Data, uint32_t Size);
        // insert new file at path (set modified timestamp)
        FileState AddFile(const std::string& Path, const void* Data, uint32_t Size, std::chrono::file_clock::time_point Timestamp);
        // remove file at path
        FileState RemoveFile(const std::string& Path);

        [[nodiscard]]
        bool IsPak() const;
        void SetXor(uint8_t Xor);
        const std::vector<std::string>& GetFileList();
        ~Pak();
    private:
        // TODO: dont expose this with public api somehow
        bool Valid;
        uint32_t Version;
        uint8_t Xor;
        void LoadPak(const std::filesystem::path& path);
        void LoadFolder(const std::filesystem::path& path);
        FILE* fp;

        std::vector<std::string> FileList;
        void UpdateFileList();
        std::map<std::string, PakRecord> FileTable;

        // std::vector<PakRecord> PakCollection;
        // std::vector<PakEntry> PakEntries;
        // std::vector<std::string> FileList;
    };

    enum log_mode_e {
        LogDefault,  // Enable logging, Default verbosity
        LogVerbose,  // Enable logging, Increased verbosity
        LogDisable   // Disable logging entirely
    };

    void change_logging(log_mode_e log_mode);

}

#endif //LIBPEGGLE_H
