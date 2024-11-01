#ifndef LIBPEGGLE_H
#define LIBPEGGLE_H

#include <cstdint>
#include <filesystem>
#include <map>
#include <variant>

#include "logma.h"

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
        // strings_are_strings means to treat string tokens like "this" instead of raw text
        static std::string JoinTokens(const std::vector<Token*>& tokens, const std::string& delimiter, bool strings_are_strings = false);
    };

/// Level ///

    // region is here for your convenience. there are a LOT of structs here, and odds are you wont probably need these anyways
#pragma region libpeggle_Level

    namespace LevelTypes {
        enum LevelEntryTypes : int32_t {
            Unknown = 0,
            Rod = 2,
            Polygon = 3,
            Circle = 5,
            Brick = 6,
            Teleport = 8,
            Emitter = 9
        };

        struct GenericDataFlags {
            union {
                struct {
                    bool isRolly               : 1;  // 0 -> float
                    bool isBouncy              : 1;  // 1 -> float
                    bool hasPegInfo            : 1;  // 2 -> struct : PegInfo
                    bool hasMovementInfo       : 1;  // 3 -> struct : MovementInfo
                    bool unk0                  : 1;  // 4      -> int32
                    bool hasCollision          : 1;  // 5
                    bool isVisible             : 1;  // 6
                    bool canMove               : 1;  // 7
                    bool hasSolidColor         : 1;  // 8 -> int32 : ColorARGB
                    bool hasOutlineColor       : 1;  // 9 -> int32 : ColorARGB
                    bool hasImage              : 1;  // 10 -> string
                    bool hasImageDX            : 1;  // 11 -> float
                    bool hasImageDY            : 1;  // 12 -> float
                    bool hasRotation           : 1;  // 13 -> float
                    bool hasBackground         : 1;  // 14
                    bool hasBaseObject         : 1;  // 15
                    bool unk1                  : 1;  // 16     -> int32
                    bool hasID                 : 1;  // 17 -> string
                    bool unk2                  : 1;  // 18     -> int32
                    bool hasSound              : 1;  // 19 -> byte
                    bool hasBallStopReset      : 1;  // 20
                    bool hasLogic              : 1;  // 21 -> string
                    bool hasForeground         : 1;  // 22
                    bool hasMaxBounceVelocity  : 1;  // 23 -> float
                    bool hasDrawSort           : 1;  // 24
                    bool hasForeground2        : 1;  // 25
                    bool hasSubID              : 1;  // 26 -> int32
                    bool hasFlipperFlags       : 1;  // 27 -> byte
                    bool hasDrawFloat          : 1;  // 28
                    bool unk3                  : 1;  // 29     -> unknown
                    // >= version 0x50
                    bool hasShadow             : 1;  // 30
                };
                uint32_t asInt;
            };
        };

        struct MovementInfoFlags {
            union {
                struct {
                    bool hasOffset            : 1;  // 0 -> int16
                    bool hasRadius1           : 1;  // 1 -> int16
                    bool hasStartPhase        : 1;  // 2 -> float
                    bool hasMovementRotation  : 1;  // 3 -> float
                    bool hasRadius2           : 1;  // 4 -> int16
                    bool hasPause1            : 1;  // 5 -> int16
                    bool hasPause2            : 1;  // 6 -> int16
                    bool hasPhase1            : 1;  // 7 -> byte
                    bool hasPhase2            : 1;  // 8 -> byte
                    bool hasPostDelayPhase    : 1;  // 9 -> float
                    bool hasMaxAngle          : 1;  // 10 -> float
                    bool hasUnknown8          : 1;  // 11 -> float
                    bool hasSubMovement       : 1;  // 12 -> float, float, MovementLink
                    bool hasObject            : 1;  // 13 -> float, float
                    bool hasRotation          : 1;  // 14 -> float
                };
                uint16_t asShort;
            };
        };

        struct EmitterFlags {
            union {
                struct {
                    bool hasUnk0 : 1;                 // 0
                    bool hasUnk1 : 1;                 // 1
                    bool hasTransparancy : 1;         // 2
                    bool hasUnk2 : 1;                 // 3
                    bool hasRandomStartPosition : 1;  // 4
                    bool hasPosition : 1;             // 5
                    bool hasChangeUnknown : 1;        // 6
                    bool hasChangeScale : 1;          // 7
                    bool hasChangeColor : 1;          // 8
                    bool hasChangeOpacity : 1;        // 9
                    bool hasChangeVelocity : 1;       // 10
                    bool hasChangeDirection : 1;      // 11
                    bool hasChangeRotation : 1;       // 12
                    bool hasUnk5 : 1;                 // 13
                    bool hasUnk6 : 1;                 // 14
                };
                uint16_t asShort;
            };
        };

        // generic 8 bit field structure
        struct Bits8 {
            union {
                struct {
                    bool v0 : 1;
                    bool v1 : 1;
                    bool v2 : 1;
                    bool v3 : 1;
                    bool v4 : 1;
                    bool v5 : 1;
                    bool v6 : 1;
                    bool v7 : 1;
                };
                uint8_t asByte;
            };
        };
        // generic 16 bit field structure
        struct Bits16 {
            union {
                struct {
                    bool v0  : 1;
                    bool v1  : 1;
                    bool v2  : 1;
                    bool v3  : 1;
                    bool v4  : 1;
                    bool v5  : 1;
                    bool v6  : 1;
                    bool v7  : 1;
                    bool v8  : 1;
                    bool v9  : 1;
                    bool v10 : 1;
                    bool v11 : 1;
                    bool v12 : 1;
                    bool v13 : 1;
                    bool v14 : 1;
                    bool v15 : 1;
                };
                uint16_t asShort;
            };
        };
        // generic 32 bit field structure
        struct Bits32 {
            union {
                struct {
                    bool v0  : 1;
                    bool v1  : 1;
                    bool v2  : 1;
                    bool v3  : 1;
                    bool v4  : 1;
                    bool v5  : 1;
                    bool v6  : 1;
                    bool v7  : 1;
                    bool v8  : 1;
                    bool v9  : 1;
                    bool v10 : 1;
                    bool v11 : 1;
                    bool v12 : 1;
                    bool v13 : 1;
                    bool v14 : 1;
                    bool v15 : 1;
                    bool v16 : 1;
                    bool v17 : 1;
                    bool v18 : 1;
                    bool v19 : 1;
                    bool v20 : 1;
                    bool v21 : 1;
                    bool v22 : 1;
                    bool v23 : 1;
                    bool v24 : 1;
                    bool v25 : 1;
                    bool v26 : 1;
                    bool v27 : 1;
                    bool v28 : 1;
                    bool v29 : 1;
                    bool v30 : 1;
                    bool v31 : 1;
                };
                uint32_t asInt;
            };
        };

        struct ColorARGB {
            union {
                uint8_t A;
                uint8_t R;
                uint8_t G;
                uint8_t B;
            };
            uint32_t asInt;
        };

        struct PegInfo {
            uint8_t mType{};
            Bits8 mFlags{};
            int32_t mUnk0{};
            int32_t mUnk1{};
            uint8_t mUnk2{};
            uint8_t mUnk3{};
            bool mVariable = false;
            bool mCrumble = false;
        };

        struct Point {
            float x;
            float y;
        };

        struct MovementLink;

        struct MovementInfo {
            int32_t mInternalLinkID = 0;  // int32
            int8_t mMovementShape = 0;  // int8;  // signed
            int32_t mType = 0;  // abs(mMovementShape)
            bool mReverse = false;
            Point mAnchorPoint = {0, 0};
            int16_t mTimePeriod = 0;  // int16

            MovementInfoFlags mFlags {};
            int16_t mOffset = 0;  // 0;  // int16
            int16_t mRadius1 = 0;  // 1;  // int16
            float mStartPhase = 0.;  // 2
            float mMoveRotation = 0.;  // 3
            int16_t mRadius2 = 0;  // 4;  // int16
            int16_t mPause1 = 0;  // 5;  // int16
            int16_t mPause2 = 0;  // 6;  // int16
            uint8_t mPhase1 = 0;  // 7;  // byte
            uint8_t mPhase2 = 0;  // 8;  // byte
            float mPostDelayPhase = 0.;  // 9
            float mMaxAngle = 0.;  // 10
            float mUnknown8 = 0.;  // 11
            float mRotation = 0.;  // 14
            float mSubMovementOffsetX = 0.;  // 12
            float mSubMovementOffsetY = 0.;
            MovementLink* mSubMovementLink = {};
            float mObjectX = 0.;  // 13
            float mObjectY = 0.;
        };

        struct MovementLink {
            int32_t InternalLinkId = 0;
            MovementInfo InternalMovement;
        };

        struct GenericData {
            float mRolly = 0.;
            float mBouncy = 0.;
            PegInfo mPegInfo {};
            MovementLink mMovementLink {};
            int32_t mUnk0 = 0;  // figure out what this is
            ColorARGB mSolidColor {};
            ColorARGB mOutlineColor {};
            std::string mImage;
            float mImageDX = 0.;
            float mImageDY = 0.;
            float mRotation = 0.;
            int32_t mUnk1 = 0;  // figure out what this is
            std::string mID;
            int32_t mUnk2 = 0;  // figure out what this is
            uint8_t mSound = 0;
            std::string mLogic;
            float mMaxBounceVelocity = 0.;
            float mSubID = 0;
            // int32_t mUnk3 = 0  // figure out what this is
            uint8_t mFlipperFlags = 0;
        };

        struct VariableFloat {
            bool mIsVariable = false;
            float mStaticVariable = 0;
            std::string mVariableValue;
        };

        struct Element;
        struct RodEntry {
            Bits8 mFlags{};
            Point mPointA {0, 0};
            Point mPointB {0, 0};
            float mE = 0.;
            float mF = 0.;
        };
        struct PolygonEntry {
            Bits8 mFlagsA{};
            Bits8 mFlagsB{};  // ver >= 0x23
            float mRotation = 0.;
            float mUnk1 = 0.;
            float mScale = 0.;
            uint8_t mNormalDir = 0;
            Point mPos {0, 0};
            std::vector<Point> mPoints {};
            uint8_t mUnk2 = 0;
            int32_t mGrowType = 0;
        };
        struct CircleEntry {
            Bits8 mFlagsA{};
            Bits8 mFlagsB{};  // ver >= 0x23
            Point mPos {0, 0};
            float mRadius = 0.;
        };
        struct BrickEntry {
            Bits8 mFlagsA{};
            Bits8 mFlagsB{};  // ver >= 0x23
            Bits16 mFlagsC{};
            float mUnk1 = 0.;
            float mUnk2 = 0.;
            float mUnk3 = 0.;
            uint8_t mUnk4 = 0;
            Point mPos {0, 0};
            uint8_t mUnk5 = 0;
            int32_t mUnk6 = 0;
            int16_t mUnk7 = 0;
            float mUnk8 = 0.;
            float mUnk9 = 0.;
            uint8_t mType = 0;
            bool mCurved = true;
            uint32_t mCurvedPoints = 0;
            float mLeftAngle = 0.;
            float mRightAngle = 0.;
            float mUnk10 = 0.;
            float mSectorAngle = 0.;
            float mWidth = 0.;
            float mLength = 0.;
            float mAngle = 0.;
            bool mTextureFlip = false;
            // union {  // im not entire sure what this is, but it appears to be some kind of short array(?)
            //     uint8_t asArray[4] = {};
            //     uint32_t asInt;
            // } mUnk12;
            uint32_t mUnk12 = 0;
        };
        struct TeleportEntry {
            Bits8 mFlags{};
            int32_t mWidth = 0;
            int32_t mHeight = 0;
            int32_t mUnk0 = 0;
            int32_t mUnk1 = 0;
            int32_t mUnk2 = 0;
            Element* mEntry{};
            Point mPos{0, 0};
            float mUnk3 = 0.;
            float mUnk4 = 0.;
        };
        struct EmitterEntry {
            int32_t mMainVar = 0;
            EmitterFlags mFlags{};

            std::string mImage;
            int32_t mWidth = 0;
            int32_t mHeight = 0;

            int32_t mMainVar0 = 0;
            float mMainVar1 = 0.;
            std::string mMainVar2;
            uint8_t mMainVar3 = 0;

            VariableFloat mUnknown0 {};
            VariableFloat mUnknown1 {};

            Point mPos {0, 0};

            std::string mEmitImage;
            float mUnknownEmitRate = 0.;
            float mUnknown2 = 0.;
            float mRotation = 0.;
            int32_t mMaxQuantity = 0;

            float mTimeBeforeFadeOut = 0.;
            float mFadeInTime = 0.;
            float mLifeDuration = 0.;

            VariableFloat mEmitRate;
            VariableFloat mEmitAreaMultiplier;

            VariableFloat mInitialRotation;
            VariableFloat mRotationVelocity;
            float mRotationUnknown = 0.;

            VariableFloat mMinScale;
            VariableFloat mScaleVelocity;
            float mMaxRandScale = 0;

            VariableFloat mColourRed;
            VariableFloat mColourGreen;
            VariableFloat mColourBlue;

            VariableFloat mOpacity;

            VariableFloat mMinVelocityX;
            VariableFloat mMinVelocityY;
            float mMaxVelocityX = 0.;
            float mMaxVelocityY = 0.;
            float mAccelerationX = 0.;
            float mAccelerationY = 0.;

            float mDirectionSpeed = 0.;
            float mDirectionRandomSpeed = 0.;
            float mDirectionAcceleration = 0.;
            float mDirectionAngle = 0.;
            float mDirectionRandomAngle = 0.;

            float mUnknownA = 0.;
            float mUnknownB = 0.;
        };
        typedef std::variant<RodEntry, PolygonEntry, CircleEntry, BrickEntry, TeleportEntry, EmitterEntry> Entry;

        struct Element {
            int32_t magic{};
            int32_t eType{};
            GenericDataFlags flags{};
            GenericData generic{};
            Entry entry{};
        };

        struct Level {
            bool valid = false;
            uint32_t version{};
            uint8_t sync_f{};
            uint32_t entries{};
            std::vector<Element> Elements{};
        };
    }

    class Level {
    public:
        static LevelTypes::Level LoadLevel(const FileRef& lvl);
        static LevelTypes::Level LoadLevel(const void* buf, uint32_t size);
        static LevelTypes::Level LoadLevel(const std::filesystem::path& path);
        static LevelTypes::Level LoadLevel(const Pak& pak, const std::filesystem::path& path);

        static FileRef BuildLevel(const LevelTypes::Level& lvl);
    private:
    };

#pragma endregion

/// Logging ///

    enum log_mode_e {
        LogDefault,  // Enable logging, Default verbosity
        LogVerbose,  // Enable logging, Increased verbosity
        LogDisable   // Disable logging entirely
    };

    void change_logging(log_mode_e log_mode);

}

#endif //LIBPEGGLE_H
