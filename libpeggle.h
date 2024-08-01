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
        explicit Pak(const std::filesystem::path& path);
        void Save(const std::filesystem::path& path);
        bool IsPak() const;
        uint32_t GetVersion() const;
        ~Pak();
    private:
        // TODO: dont expose this with public api somehow
        bool Valid;
        uint32_t Version;
        uint8_t Xor;
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
