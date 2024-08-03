#ifndef LIBPEGGLE_H
#define LIBPEGGLE_H

#include <cstdint>
#include <filesystem>

namespace Peggle {
    // forward declarations for types
    struct PakRecord;
    struct PakEntry;

    class Pak {
    public:
        // open pak file or folder
        explicit Pak(const std::filesystem::path& path);
        // save pak to file
        void Save(const std::filesystem::path& path) const;
        // save pak to folder
        void Export(const std::filesystem::path& path) const;
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
        std::vector<PakRecord> PakCollection;
        std::vector<PakEntry> PakEntries;
        std::vector<std::string> FileList;
    };

    enum log_mode_e {
        LogDefault,  // Enable logging, Default verbosity
        LogVerbose,  // Enable logging, Increased verbosity
        LogDisable   // Disable logging entirely
    };

    void change_logging(log_mode_e log_mode);

}

#endif //LIBPEGGLE_H
