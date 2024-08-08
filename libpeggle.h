#ifndef LIBPEGGLE_H
#define LIBPEGGLE_H

#include <cstdint>
#include <filesystem>
#include <map>

namespace Peggle {
    // forward declarations for types
    struct PakRecord;
    struct Token;
    enum class TokenType {
        Unset,  // this type should never happen
        String,
        Integer,
        Decimal
    };

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

/// Pak ///

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
        // TODO: dont expose this with public api somehow (pimpl?)
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

/// Config ///

    namespace ConfigTypes {
        enum class ConfigType {
            Trophy,
            Stage,
            Character
        };

        /// generic ///

        struct Simple {
            std::string Key;
            std::vector<Token*> Values;
        };

        /// stages.cfg ///

        struct Level {
            std::string Id;
            std::string Name;
        };
        struct Dialog {
            int Index;
            std::string Text;
            std::string Title;
        };
        struct StageDialog {
            int Index;
            std::string Text;
        };
        struct Credit {
            int Int1;  // credits index?
            std::string Text;
            int Int2;  // size? font?
        };

        struct Stage {
            std::vector<Level> Levels;
            std::vector<Dialog> Dialog;
            std::vector<StageDialog> StageDialog;
            std::vector<Credit> Credits;
        };

        struct StageCfg {
            bool Valid = false;
            std::vector<Stage> Stages;
            std::vector<int> ExcludeRandStages;
            std::vector<std::string> IncludeRandLevels;
            std::vector<std::string> Tips;
        };

        /// trophy.cfg ///

        struct Trophy {
            std::string Name;
            int Id;
            std::vector<Simple> Etc;
        };

        struct Page {
            std::string Name;
            std::string Desc;
            std::string SmallDesc;
            std::vector<Trophy> Trophies;
        };

        struct TrophyCfg {
            bool Valid = false;
            std::vector<Page> Pages;
        };

        /// characters.cfg ///

        struct Character {
            std::string Name;
            std::string Powerup;
            std::string Desc;
            std::vector<std::string> Tips;
            std::vector<Simple> Etc;
        };

        struct CharacterCfg {
            bool Valid = false;
            std::vector<Character> Characters;
        };

        /// LevelEditor ///  TODO: not planned

        // struct Choice {
        //     std::string Key;
        //     std::vector<Token*> Values;
        // };
        // struct Script {
        //     std::vector<CfgElement> Entries;
        // };

        struct CfgObject {
            std::vector<Simple> Elements;
            static std::string toString();
        };
    }

    class Config {
    public:
        static ConfigTypes::StageCfg LoadStageConfig(const std::string& cfg_string);
        static ConfigTypes::StageCfg LoadStageConfig(const std::filesystem::path& path);
        static ConfigTypes::StageCfg LoadStageConfig(const Pak& pak, const std::filesystem::path& path);

        static ConfigTypes::TrophyCfg LoadTrophyConfig(const std::string& cfg_string);
        static ConfigTypes::TrophyCfg LoadTrophyConfig(const std::filesystem::path& path);
        static ConfigTypes::TrophyCfg LoadTrophyConfig(const Pak& pak, const std::filesystem::path& path);

        static ConfigTypes::CharacterCfg LoadCharacterConfig(const std::string& cfg_string);
        static ConfigTypes::CharacterCfg LoadCharacterConfig(const std::filesystem::path& path);
        static ConfigTypes::CharacterCfg LoadCharacterConfig(const Pak& pak, const std::filesystem::path& path);

        static std::string BuildConfig(const ConfigTypes::StageCfg& cfg);
        static std::string BuildConfig(const ConfigTypes::TrophyCfg& cfg);
        static std::string BuildConfig(const ConfigTypes::CharacterCfg& cfg);

        static void SaveConfig(const ConfigTypes::StageCfg& cfg, Pak& pak, const std::filesystem::path& path);
        static void SaveConfig(const ConfigTypes::TrophyCfg& cfg, Pak& pak, const std::filesystem::path& path);
        static void SaveConfig(const ConfigTypes::CharacterCfg& cfg, Pak& pak, const std::filesystem::path& path);

        static TokenType GetTokenType(const Token& token);
        static TokenType GetTokenType(const Token* token);

        static const std::string* GetTokenStringRef(const Token& token);
        static const std::string* GetTokenStringRef(const Token* token);
        static std::string GetTokenString(const Token& token);
        static std::string GetTokenString(const Token* token);
        static int32_t GetTokenInteger(const Token& token);
        static int32_t GetTokenInteger(const Token* token);
        static float_t GetTokenDecimal(const Token& token);
        static float_t GetTokenDecimal(const Token* token);

        static void UpdateToken(Token& token, const std::string& data);
        static void UpdateToken(Token& token, int32_t data);
        static void UpdateToken(Token& token, float_t data);

    private:
        static std::vector<Token*> Tokenize(const std::string& text);
    };

/// Logging ///

    enum log_mode_e {
        LogDefault,  // Enable logging, Default verbosity
        LogVerbose,  // Enable logging, Increased verbosity
        LogDisable   // Disable logging entirely
    };

    void change_logging(log_mode_e log_mode);

}

#endif //LIBPEGGLE_H
