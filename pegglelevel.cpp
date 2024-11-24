#include "binstream.h"
#include "iohelper.h"
#include "libpeggle.h"
#include "logma.h"
#include "utils.h"
#include <cstdlib>

namespace Peggle {
#pragma region libpeggle_Level

    struct LevelTypes::Entry {
        union EntryData {
            LevelTypes::RodEntry* Rod;
            LevelTypes::PolygonEntry* Polygon;
            LevelTypes::CircleEntry* Circle;
            LevelTypes::BrickEntry* Brick;
            LevelTypes::TeleportEntry* Teleport;
            LevelTypes::EmitterEntry* Emitter;
        };
    private:
        LevelTypes::LevelEntryType Type{};
        EntryData Data{};

    public:
        explicit Entry(const LevelEntryType type) {
            Type = type;
            switch (type) {
                case Rod: { Data.Rod = new RodEntry; break; }
                case Polygon: { Data.Polygon = new PolygonEntry; break; }
                case Circle: { Data.Circle = new CircleEntry; break; }
                case Brick: { Data.Brick = new BrickEntry; break; }
                case Teleporter: { Data.Teleport = new TeleportEntry; break; }
                case Emitter: { Data.Emitter = new EmitterEntry; break; }
                default: break;
            }
        }

        static LevelTypes::RodEntry* GetRod(const Entry* entry) {
            if (entry->Type == LevelEntryType::Rod)
                return entry->Data.Rod;
            return nullptr;
        }
        static LevelTypes::PolygonEntry* GetPolygon(const Entry* entry) {
            if (entry->Type == LevelEntryType::Polygon)
                return entry->Data.Polygon;
            return nullptr;
        }
        static LevelTypes::CircleEntry* GetCircle(const Entry* entry) {
            if (entry->Type == LevelEntryType::Circle)
                return entry->Data.Circle;
            return nullptr;
        }
        static LevelTypes::BrickEntry* GetBrick(const Entry* entry) {
            if (entry->Type == LevelEntryType::Brick)
                return entry->Data.Brick;
            return nullptr;
        }
        static LevelTypes::TeleportEntry* GetTeleporter(const Entry* entry) {
            if (entry->Type == LevelEntryType::Teleporter)
                return entry->Data.Teleport;
            return nullptr;
        }
        static LevelTypes::EmitterEntry* GetEmitter(const Entry* entry) {
            if (entry->Type == LevelEntryType::Emitter)
                return entry->Data.Emitter;
            return nullptr;
        }

        static LevelTypes::RodEntry* GetRod(const Entry& entry) {
            if (entry.Type == LevelEntryType::Rod)
                return entry.Data.Rod;
            return nullptr;
        }
        static LevelTypes::PolygonEntry* GetPolygon(const Entry& entry) {
            if (entry.Type == LevelEntryType::Polygon)
                return entry.Data.Polygon;
            return nullptr;
        }
        static LevelTypes::CircleEntry* GetCircle(const Entry& entry) {
            if (entry.Type == LevelEntryType::Circle)
                return entry.Data.Circle;
            return nullptr;
        }
        static LevelTypes::BrickEntry* GetBrick(const Entry& entry) {
            if (entry.Type == LevelEntryType::Brick)
                return entry.Data.Brick;
            return nullptr;
        }
        static LevelTypes::TeleportEntry* GetTeleporter(const Entry& entry) {
            if (entry.Type == LevelEntryType::Teleporter)
                return entry.Data.Teleport;
            return nullptr;
        }
        static LevelTypes::EmitterEntry* GetEmitter(const Entry& entry) {
            if (entry.Type == LevelEntryType::Emitter)
                return entry.Data.Emitter;
            return nullptr;
        }

        static LevelEntryType GetType(const Entry& entry) {
            return entry.Type;
        }
        static LevelEntryType GetType(const Entry* entry) {
            return entry->Type;
        }
    };

    namespace LevelHelpers {
        /// fwd declarations ///
        std::string read_string(binstream& bs);
        void write_string(binstream& bs, const std::string& str);

        LevelTypes::VariableFloat read_variable_float(binstream& bs);
        void write_variable_float(binstream& bs, const LevelTypes::VariableFloat& vf);

        LevelTypes::Point read_point(binstream& bs);
        void write_point(binstream& bs, const LevelTypes::Point& p);

        LevelTypes::RodEntry read_entry_rod(binstream& bs);
        void write_entry_rod(binstream& bs, const LevelTypes::RodEntry& entry);
        LevelTypes::PolygonEntry read_entry_polygon(binstream& bs);
        void write_entry_polygon(binstream& bs, uint32_t version, const LevelTypes::PolygonEntry& entry);
        LevelTypes::CircleEntry read_entry_circle(binstream& bs, uint32_t version);
        void write_entry_circle(binstream& bs, uint32_t version, const LevelTypes::CircleEntry& entry);
        LevelTypes::BrickEntry read_entry_brick(binstream& bs, uint32_t version);
        void write_entry_brick(binstream& bs, uint32_t version, const LevelTypes::BrickEntry& entry);
        LevelTypes::TeleportEntry read_entry_teleport(binstream& bs, uint32_t version);
        void write_entry_teleport(binstream& bs, uint32_t version, const LevelTypes::TeleportEntry& entry);
        LevelTypes::EmitterEntry read_entry_emitter(binstream& bs);
        void write_entry_emitter(binstream& bs, const LevelTypes::EmitterEntry& entry);

        LevelTypes::Entry read_entry(binstream& bs, int32_t eType, uint32_t version);
        void write_entry(binstream& bs, uint32_t version, const LevelTypes::Entry& entry);

        LevelTypes::MovementInfo read_movement(binstream& bs);
        void write_movement(binstream& bs, const LevelTypes::MovementInfo& m);

        LevelTypes::MovementLink read_movement_link(binstream& bs);
        void write_movement_link(binstream& bs, const LevelTypes::MovementLink& l);

        LevelTypes::PegInfo read_peginfo(binstream& bs);
        void write_peginfo(binstream& bs, const LevelTypes::PegInfo& p);

        LevelTypes::GenericData read_generic(binstream& bs, LevelTypes::GenericDataFlags flags);
        void write_generic(binstream& bs, const LevelTypes::GenericDataFlags& flags, const LevelTypes::GenericData& generic);

        LevelTypes::Element read_element(binstream& bs, uint32_t version);
        void write_element(binstream& bs, uint32_t version, const LevelTypes::Element& element);

        /// implementations ///
        std::string read_string(binstream& bs) {
            const auto len = bs.read<int16_t>();
            std::string res;
            if (len == 0) return res;
            const char* data = static_cast<char*>(bs.bytes(len));
            res.assign(data, len);
            return res;
        }
        void write_string(binstream &bs, const std::string& str) {
            const auto len = static_cast<int16_t>(str.length());
            bs.write(len);
            if (len == 0) return;
            bs.write(str.c_str(), len);
        }

        LevelTypes::VariableFloat read_variable_float(binstream& bs) {
            LevelTypes::VariableFloat res = {};
            const auto var1 = bs.read<char>();

            if (var1 > 0) {
                res.mIsVariable = false;
                res.mStaticVariable = bs.read<float>();
            } else {
                res.mIsVariable = true;
                res.mVariableValue = read_string(bs);
            }

            return res;
        }
        void write_variable_float(binstream& bs, const LevelTypes::VariableFloat& vf) {
            const auto var1 = static_cast<int8_t>(!vf.mIsVariable);
            bs.write(var1);
            if (vf.mIsVariable)
                write_string(bs, vf.mVariableValue);
            else
                bs.write(vf.mStaticVariable);
        }

        LevelTypes::Point read_point(binstream& bs) {
            return LevelTypes::Point{
                bs.read<float>(),
                bs.read<float>()
            };
        }
        void write_point(binstream& bs, const LevelTypes::Point& p) {
            bs.write(p.x);
            bs.write(p.y);
        }

        // entry pain city //

        LevelTypes::RodEntry read_entry_rod(binstream &bs) {
            LevelTypes::RodEntry entry = {};

            entry.mFlags = {};
            entry.mFlags.asByte = bs.read<uint8_t>();

            entry.mPointA = read_point(bs);
            entry.mPointB = read_point(bs);

            if (entry.mFlags.v0)
                entry.mE = bs.read<float>();
            if (entry.mFlags.v1)
                entry.mF = bs.read<float>();

            return entry;
        }
        void write_entry_rod(binstream &bs, const LevelTypes::RodEntry &entry) {
            bs.write(entry.mFlags.asByte);

            write_point(bs, entry.mPointA);
            write_point(bs, entry.mPointB);

            if (entry.mFlags.v0)
                bs.write(entry.mE);
            if (entry.mFlags.v1)
                bs.write(entry.mF);
        }

        LevelTypes::PolygonEntry read_entry_polygon(binstream &bs, const uint32_t version) {
            LevelTypes::PolygonEntry entry = {};

            entry.mFlagsA = {};
            entry.mFlagsA.asByte = bs.read<uint8_t>();
            entry.mFlagsB = {};
            if (version > 0x23)
                entry.mFlagsB.asByte = bs.read<uint8_t>();

            if (entry.mFlagsA.v2)
                entry.mRotation = bs.read<float>();
            if (entry.mFlagsA.v3)
                entry.mUnk1 = bs.read<float>();
            if (entry.mFlagsA.v5)
                entry.mScale = bs.read<float>();
            if (entry.mFlagsA.v1)
                entry.mNormalDir = bs.read<uint8_t>();
            if (entry.mFlagsA.v4)
                entry.mPos = read_point(bs);

            const auto numPoints = bs.read<int32_t>();
            for (int i = 0; i < numPoints; ++i)
                entry.mPoints.emplace_back(read_point(bs));

            if (entry.mFlagsB.v0)
                entry.mUnk2 = bs.read<uint8_t>();
            if (entry.mFlagsB.v1)
                entry.mGrowType = bs.read<int32_t>();

            return entry;
        }
        void write_entry_polygon(binstream &bs, const uint32_t version, const LevelTypes::PolygonEntry &entry) {
            bs.write(entry.mFlagsA.asByte);
            if (version > 0x23)
                bs.write(entry.mFlagsB.asByte);

            if (entry.mFlagsA.v2)
                bs.write(entry.mRotation);
            if (entry.mFlagsA.v3)
                bs.write(entry.mUnk1);
            if (entry.mFlagsA.v5)
                bs.write(entry.mScale);
            if (entry.mFlagsA.v1)
                bs.write(entry.mNormalDir);
            if (entry.mFlagsA.v4)
                write_point(bs, entry.mPos);

            const auto mNumPoints = static_cast<int32_t>(entry.mPoints.size());
            bs.write(mNumPoints);
            for (const auto& point : entry.mPoints)
                write_point(bs, point);

            if (entry.mFlagsB.v0)
                bs.write(entry.mUnk2);
            if (entry.mFlagsB.v1)
                bs.write(entry.mGrowType);
        }

        LevelTypes::CircleEntry read_entry_circle(binstream& bs, const uint32_t version) {
            LevelTypes::CircleEntry entry = {};

            entry.mFlagsA = {};
            entry.mFlagsA.asByte = bs.read<uint8_t>();
            entry.mFlagsB = {};
            if (version >= 0x52)
                entry.mFlagsB.asByte = bs.read<uint8_t>();

            if (entry.mFlagsA.v1)
                entry.mPos = read_point(bs);
            entry.mRadius = bs.read<float>();

            return entry;
        }
        void write_entry_circle(binstream& bs, const uint32_t version, const LevelTypes::CircleEntry& entry) {
            bs.write(entry.mFlagsA.asByte);
            if (version >= 0x52)
                bs.write(entry.mFlagsB.asByte);

            if (entry.mFlagsA.v1)
                bs.write(entry.mPos);
            bs.write(entry.mRadius);
        }

        LevelTypes::BrickEntry read_entry_brick(binstream& bs, const uint32_t version) {
            LevelTypes::BrickEntry entry = {};

            entry.mFlagsA = {};
            entry.mFlagsA.asByte = bs.read<uint8_t>();

            entry.mFlagsB = {};
            if (version >= 0x23)
                entry.mFlagsB.asByte = bs.read<uint8_t>();

            if (entry.mFlagsA.v2)
                entry.mUnk1 = bs.read<float>();
            if (entry.mFlagsA.v3)
                entry.mUnk2 = bs.read<float>();
            if (entry.mFlagsA.v5)
                entry.mUnk3 = bs.read<float>();
            if (entry.mFlagsA.v1)
                entry.mUnk4 = bs.read<uint8_t>();
            if (entry.mFlagsA.v4)
                entry.mPos = read_point(bs);

            if (entry.mFlagsB.v0)
                entry.mUnk5 = bs.read<uint8_t>();
            if (entry.mFlagsB.v1)
                entry.mUnk6 = bs.read<int32_t>();
            if (entry.mFlagsB.v2)
                entry.mUnk7 = bs.read<int16_t>();

            entry.mFlagsC = {};
            entry.mFlagsC.asShort = bs.read<uint16_t>();

            if (entry.mFlagsC.v8)
                entry.mUnk8 = bs.read<float>();
            if (entry.mFlagsC.v9)
                entry.mUnk9 = bs.read<float>();
            if (entry.mFlagsC.v2) {
                entry.mType = bs.read<uint8_t>();
                if (entry.mType == 5)
                    entry.mCurved = false;
            }
            if (entry.mFlagsC.v3)
                entry.mCurvedPoints = bs.read<uint8_t>() + 2;
            if (entry.mFlagsC.v5)
                entry.mLeftAngle = bs.read<float>();
            if (entry.mFlagsC.v6) {
                entry.mRightAngle = bs.read<float>();
                entry.mUnk10 = bs.read<float>();
            }
            if (entry.mFlagsC.v4)
                entry.mSectorAngle = bs.read<float>();
            if (entry.mFlagsC.v7)
                entry.mWidth = bs.read<float>();

            entry.mTextureFlip = entry.mFlagsC.v10;

            entry.mLength = bs.read<float>();
            entry.mAngle = bs.read<float>();

            // entry.mUnk12.asInt = bs.read<uint32_t>();
            entry.mUnk12 = bs.read<uint32_t>();

            return entry;
        }
        void write_entry_brick(binstream& bs, const uint32_t version, const LevelTypes::BrickEntry& entry) {
            bs.write(entry.mFlagsA.asByte);
            if (version >= 0x23)
                bs.write(entry.mFlagsB.asByte);

            if (entry.mFlagsA.v2)
                bs.write(entry.mUnk1);
            if (entry.mFlagsA.v3)
                bs.write(entry.mUnk2);
            if (entry.mFlagsA.v5)
                bs.write(entry.mUnk3);
            if (entry.mFlagsA.v1)
                bs.write(entry.mUnk4);
            if (entry.mFlagsA.v4)
                write_point(bs, entry.mPos);

            if (entry.mFlagsB.v0)
                bs.write(entry.mUnk5);
            if (entry.mFlagsB.v1)
                bs.write(entry.mUnk6);
            if (entry.mFlagsB.v2)
                bs.write(entry.mUnk7);

            bs.write(entry.mFlagsC.asShort);

            if (entry.mFlagsC.v8)
                bs.write(entry.mUnk8);
            if (entry.mFlagsC.v9)
                bs.write(entry.mUnk9);
            if (entry.mFlagsC.v2)
                bs.write(entry.mType);
            if (entry.mFlagsC.v3) {
                const uint8_t points = entry.mCurvedPoints - 2;
                bs.write(points);
            }
            if (entry.mFlagsC.v5)
                bs.write(entry.mLeftAngle);
            if (entry.mFlagsC.v6) {
                bs.write(entry.mRightAngle);
                bs.write(entry.mUnk10);
            }
            if (entry.mFlagsC.v4)
                bs.write(entry.mSectorAngle);
            if (entry.mFlagsC.v7)
                bs.write(entry.mWidth);

            bs.write(entry.mLength);
            bs.write(entry.mAngle);

            // bs.write(entry.mUnk12.asInt);
            bs.write(entry.mUnk12);
        }

        LevelTypes::TeleportEntry read_entry_teleport(binstream& bs, const uint32_t version) {
            LevelTypes::TeleportEntry entry = {};

            entry.mFlags = {};
            entry.mFlags.asByte = bs.read<uint8_t>();

            entry.mWidth = bs.read<int32_t>();
            entry.mHeight = bs.read<int32_t>();

            if (entry.mFlags.v1)
                entry.mUnk0 = bs.read<int16_t>();
            if (entry.mFlags.v3)
                entry.mUnk1 = bs.read<int32_t>();
            if (entry.mFlags.v5)
                entry.mUnk2 = bs.read<int32_t>();
            if (entry.mFlags.v4) {
                // auto* sub = static_cast<LevelTypes::Element*>(malloc(sizeof LevelTypes::Element));
                auto* sub = new LevelTypes::Element;
                *sub = read_element(bs, version);
                entry.mEntry = sub;
            }
            if (entry.mFlags.v2)
                entry.mPos = read_point(bs);
            if (entry.mFlags.v6) {
                entry.mUnk3 = bs.read<float>();
                entry.mUnk4 = bs.read<float>();
            }

            return entry;
        }
        void write_entry_teleport(binstream& bs, const uint32_t version, const LevelTypes::TeleportEntry& entry) {
            bs.write(entry.mFlags.asByte);

            bs.write(entry.mWidth);
            bs.write(entry.mHeight);

            if (entry.mFlags.v1)
                bs.write(entry.mUnk0);
            if (entry.mFlags.v3)
                bs.write(entry.mUnk1);
            if (entry.mFlags.v5)
                bs.write(entry.mUnk2);
            if (entry.mFlags.v4)
                write_element(bs, version, *entry.mEntry);
            if (entry.mFlags.v2)
                write_point(bs, entry.mPos);
            if (entry.mFlags.v6) {
                bs.write(entry.mUnk3);
                bs.write(entry.mUnk4);
            }
        }

        LevelTypes::EmitterEntry read_entry_emitter(binstream& bs) {
            LevelTypes::EmitterEntry entry = {};

            entry.mMainVar = bs.read<int32_t>();

            entry.mFlags = {};
            entry.mFlags.asShort = bs.read<uint16_t>();

            entry.mImage = read_string(bs);
            entry.mWidth = bs.read<int32_t>();
            entry.mHeight = bs.read<int32_t>();

            if (entry.mMainVar == 2) {
                entry.mMainVar0 = bs.read<int32_t>();
                entry.mMainVar1 = bs.read<float>();
                entry.mMainVar2 = read_string(bs);
                entry.mMainVar3 = bs.read<uint8_t>();

                if (entry.mFlags.hasUnk5) {
                    entry.mUnknown0 = read_variable_float(bs);
                    entry.mUnknown1 = read_variable_float(bs);
                }
            }

            if (entry.mFlags.hasPosition)
                entry.mPos = read_point(bs);

            entry.mEmitImage = read_string(bs);
            entry.mUnknownEmitRate = bs.read<float>();
            entry.mUnknown2 = bs.read<float>();
            entry.mRotation = bs.read<float>();
            entry.mMaxQuantity = bs.read<int32_t>();

            entry.mTimeBeforeFadeOut = bs.read<float>();
            entry.mFadeInTime = bs.read<float>();
            entry.mLifeDuration = bs.read<float>();

            entry.mEmitRate = read_variable_float(bs);
            entry.mEmitAreaMultiplier = read_variable_float(bs);

            if (entry.mFlags.hasChangeRotation) {
                entry.mInitialRotation = read_variable_float(bs);
                entry.mRotationVelocity = read_variable_float(bs);
                entry.mRotationUnknown = bs.read<float>();
            }

            if (entry.mFlags.hasChangeScale) {
                entry.mMinScale = read_variable_float(bs);
                entry.mScaleVelocity = read_variable_float(bs);
                entry.mMaxRandScale = bs.read<float>();
            }

            if (entry.mFlags.hasChangeColor) {
                entry.mColourRed = read_variable_float(bs);
                entry.mColourGreen = read_variable_float(bs);
                entry.mColourBlue = read_variable_float(bs);
            }

            if (entry.mFlags.hasChangeOpacity)
                entry.mOpacity = read_variable_float(bs);

            if (entry.mFlags.hasChangeVelocity) {
                entry.mMinVelocityX = read_variable_float(bs);
                entry.mMinVelocityY = read_variable_float(bs);
                entry.mMaxVelocityX = bs.read<float>();
                entry.mMaxVelocityY = bs.read<float>();
                entry.mAccelerationX = bs.read<float>();
                entry.mAccelerationY = bs.read<float>();
            }

            if (entry.mFlags.hasChangeDirection) {
                entry.mDirectionSpeed = bs.read<float>();
                entry.mDirectionRandomSpeed = bs.read<float>();
                entry.mDirectionAcceleration = bs.read<float>();
                entry.mDirectionAngle = bs.read<float>();
                entry.mDirectionRandomAngle = bs.read<float>();
            }

            if (entry.mFlags.hasChangeUnknown) {
                entry.mUnknownA = bs.read<float>();
                entry.mUnknownB = bs.read<float>();
            }

            return entry;
        }
        void write_entry_emitter(binstream& bs, const LevelTypes::EmitterEntry& entry) {
            bs.write(entry.mMainVar);

            bs.write(entry.mFlags.asShort);

            write_string(bs, entry.mImage);
            bs.write(entry.mWidth);
            bs.write(entry.mHeight);

            if (entry.mMainVar == 2) {
                bs.write(entry.mMainVar0);
                bs.write(entry.mMainVar1);
                write_string(bs, entry.mMainVar2);
                bs.write(entry.mMainVar3);

                if (entry.mFlags.hasUnk5) {
                    write_variable_float(bs, entry.mUnknown0);
                    write_variable_float(bs, entry.mUnknown1);
                }
            }

            if (entry.mFlags.hasPosition)
                write_point(bs, entry.mPos);

            write_string(bs, entry.mEmitImage);
            bs.write(entry.mUnknownEmitRate);
            bs.write(entry.mUnknown2);
            bs.write(entry.mRotation);
            bs.write(entry.mMaxQuantity);

            bs.write(entry.mTimeBeforeFadeOut);
            bs.write(entry.mFadeInTime);
            bs.write(entry.mLifeDuration);

            write_variable_float(bs, entry.mEmitRate);
            write_variable_float(bs, entry.mEmitAreaMultiplier);

            if (entry.mFlags.hasChangeRotation) {
                write_variable_float(bs, entry.mInitialRotation);
                write_variable_float(bs, entry.mRotationVelocity);
                bs.write(entry.mRotationUnknown);
            }

            if (entry.mFlags.hasChangeScale) {
                write_variable_float(bs, entry.mMinScale);
                write_variable_float(bs, entry.mScaleVelocity);
                bs.write(entry.mMaxRandScale);
            }

            if (entry.mFlags.hasChangeColor) {
                write_variable_float(bs, entry.mColourRed);
                write_variable_float(bs, entry.mColourGreen);
                write_variable_float(bs, entry.mColourBlue);
            }

            if (entry.mFlags.hasChangeOpacity)
                write_variable_float(bs, entry.mOpacity);

            if (entry.mFlags.hasChangeVelocity) {
                write_variable_float(bs, entry.mMinVelocityX);
                write_variable_float(bs, entry.mMinVelocityY);
                bs.write(entry.mMaxVelocityX);
                bs.write(entry.mMaxVelocityY);
                bs.write(entry.mAccelerationX);
                bs.write(entry.mAccelerationY);
            }

            if (entry.mFlags.hasChangeDirection) {
                bs.write(entry.mDirectionSpeed);
                bs.write(entry.mDirectionRandomSpeed);
                bs.write(entry.mDirectionAcceleration);
                bs.write(entry.mDirectionAngle);
                bs.write(entry.mDirectionRandomAngle);
            }

            if (entry.mFlags.hasChangeUnknown) {
                bs.write(entry.mUnknownA);
                bs.write(entry.mUnknownB);
            }
        }

        LevelTypes::Entry read_entry(binstream& bs, const int32_t eType, const uint32_t version) {
            const auto entry_type = static_cast<LevelTypes::LevelEntryType>(eType);
            const auto entry = LevelTypes::Entry(entry_type);
            switch (entry_type) {
                case LevelTypes::Rod: {
                    const auto rod = LevelTypes::Entry::GetRod(entry);
                    *rod = read_entry_rod(bs); break;
                }
                case LevelTypes::Polygon: {
                    const auto polygon = LevelTypes::Entry::GetPolygon(entry);
                    *polygon = read_entry_polygon(bs, version); break;
                }
                case LevelTypes::Circle: {
                    const auto circle = LevelTypes::Entry::GetCircle(entry);
                    *circle = read_entry_circle(bs, version); break;
                }
                case LevelTypes::Brick: {
                    const auto brick = LevelTypes::Entry::GetBrick(entry);
                    *brick = read_entry_brick(bs, version); break;
                }
                case LevelTypes::Teleporter: {
                    const auto teleporter = LevelTypes::Entry::GetTeleporter(entry);
                    *teleporter = read_entry_teleport(bs, version); break;
                }
                case LevelTypes::Emitter: {
                    const auto emitter = LevelTypes::Entry::GetEmitter(entry);
                    *emitter = read_entry_emitter(bs); break;
                }
                default: {
                    // todo: raise exception for invalid entry type
                }
            }
            return entry;
        }
        void write_entry(binstream& bs, const uint32_t version, const LevelTypes::Entry& entry) {
            switch (LevelTypes::Entry::GetType(entry)) {
                case LevelTypes::Rod: {
                    const auto rod = LevelTypes::Entry::GetRod(entry);
                    write_entry_rod(bs, *rod); break;
                }
                case LevelTypes::Polygon: {
                    const auto polygon = LevelTypes::Entry::GetPolygon(entry);
                    write_entry_polygon(bs, version, *polygon); break;
                }
                case LevelTypes::Circle: {
                    const auto circle = LevelTypes::Entry::GetCircle(entry);
                    write_entry_circle(bs, version, *circle); break;
                }
                case LevelTypes::Brick: {
                    const auto brick = LevelTypes::Entry::GetBrick(entry);
                    write_entry_brick(bs, version, *brick); break;
                }
                case LevelTypes::Teleporter: {
                    const auto teleporter = LevelTypes::Entry::GetTeleporter(entry);
                    write_entry_teleport(bs, version, *teleporter); break;
                }
                case LevelTypes::Emitter: {
                    const auto emitter = LevelTypes::Entry::GetEmitter(entry);
                    write_entry_emitter(bs, *emitter); break;
                }
                default: {
                    // todo: raise exception for invalid entry type
                }
            }
        }

        LevelTypes::MovementInfo read_movement(binstream& bs) {
            LevelTypes::MovementInfo res = {};

            res.mMovementShape = bs.read<int8_t>();
            res.mType = abs(res.mMovementShape);
            // document mReverse as negative shape

            res.mAnchorPoint = read_point(bs);

            res.mTimePeriod = bs.read<int16_t>();

            res.mFlags = {};
            res.mFlags.asShort = bs.read<int16_t>();

            if (res.mFlags.hasOffset)
                res.mOffset = bs.read<int16_t>();
            if (res.mFlags.hasRadius1)
                res.mRadius1 = bs.read<int16_t>();
            if (res.mFlags.hasStartPhase)
                res.mStartPhase = bs.read<float>();
            if (res.mFlags.hasMovementRotation)
                res.mMoveRotation = bs.read<float>();
            if (res.mFlags.hasRadius2)
                res.mRadius2 = bs.read<int16_t>();
            if (res.mFlags.hasPause1)
                res.mPause1 = bs.read<int16_t>();
            if (res.mFlags.hasPause2)
                res.mPause2 = bs.read<int16_t>();
            if (res.mFlags.hasPhase1)
                res.mPhase1 = bs.read<uint8_t>();
            if (res.mFlags.hasPhase2)
                res.mPhase2 = bs.read<uint8_t>();
            if (res.mFlags.hasPostDelayPhase)
                res.mPostDelayPhase = bs.read<float>();
            if (res.mFlags.hasMaxAngle)
                res.mMaxAngle = bs.read<float>();
            if (res.mFlags.hasUnknown8)
                res.mUnknown8 = bs.read<float>();
            if (res.mFlags.hasRotation)
                res.mRotation = bs.read<float>();
            if (res.mFlags.hasSubMovement) {
                res.mSubMovementOffsetX = bs.read<float>();
                res.mSubMovementOffsetY = bs.read<float>();
                // auto* sub = static_cast<LevelTypes::MovementInfo*>(malloc(sizeof LevelTypes::MovementInfo));
                auto* sub = new LevelTypes::MovementLink;
                *sub = read_movement_link(bs);
                res.mSubMovementLink = sub;
            }
            if (res.mFlags.hasObject) {
                res.mObjectX = bs.read<float>();
                res.mObjectY = bs.read<float>();
            }

            // log_debug("%d %f %f %f %f %d\n",
            //     res.mMovementShape,
            //     res.mAnchorPointX, res.mAnchorPointY,
            //     res.mObjectX, res.mObjectY,
            //     res.mFlags.asShort
            //     );

            return res;
        }
        void write_movement(binstream& bs, const LevelTypes::MovementInfo& m) {
            bs.write(m.mMovementShape);

            write_point(bs, m.mAnchorPoint);

            bs.write(m.mTimePeriod);

            bs.write(m.mFlags.asShort);

            if (m.mFlags.hasOffset)
                bs.write(m.mOffset);
            if (m.mFlags.hasRadius1)
                bs.write(m.mRadius1);
            if (m.mFlags.hasStartPhase)
                bs.write(m.mStartPhase);
            if (m.mFlags.hasMovementRotation)
                bs.write(m.mMoveRotation);
            if (m.mFlags.hasRadius2)
                bs.write(m.mRadius2);
            if (m.mFlags.hasPause1)
                bs.write(m.mPause1);
            if (m.mFlags.hasPause2)
                bs.write(m.mPause2);
            if (m.mFlags.hasPhase1)
                bs.write(m.mPhase1);
            if (m.mFlags.hasPhase2)
                bs.write(m.mPhase2);
            if (m.mFlags.hasPostDelayPhase)
                bs.write(m.mPostDelayPhase);
            if (m.mFlags.hasMaxAngle)
                bs.write(m.mMaxAngle);
            if (m.mFlags.hasUnknown8)
                bs.write(m.mUnknown8);
            if (m.mFlags.hasRotation)
                bs.write(m.mRotation);
            if (m.mFlags.hasSubMovement) {
                bs.write(m.mSubMovementOffsetX);
                bs.write(m.mSubMovementOffsetY);
                write_movement_link(bs, *m.mSubMovementLink);
            }
            if (m.mFlags.hasObject) {
                bs.write(m.mObjectX);
                bs.write(m.mObjectY);
            }
        }

        LevelTypes::MovementLink read_movement_link(binstream& bs) {
            LevelTypes::MovementLink link = {};
            link.InternalLinkId = bs.read<int32_t>();
            if (link.InternalLinkId == 1)
                link.InternalMovement = read_movement(bs);
            return link;
        }
        void write_movement_link(binstream& bs, const LevelTypes::MovementLink& l) {
            bs.write(l.InternalLinkId);
            if (l.InternalLinkId == 1)
                write_movement(bs, l.InternalMovement);
        }

        LevelTypes::PegInfo read_peginfo(binstream& bs) {
            LevelTypes::PegInfo res = {};
            res.mType = bs.read<uint8_t>();
            res.mFlags = {};
            res.mFlags.asByte = bs.read<uint8_t>();

            if (res.mFlags.v1)
                res.mVariable = true;
            if (res.mFlags.v2)
                res.mUnk0 = bs.read<int32_t>();
            if (res.mFlags.v3)
                res.mCrumble = true;
            if (res.mFlags.v4)
                res.mUnk1 = bs.read<int32_t>();
            if (res.mFlags.v5)
                res.mUnk2 = bs.read<uint8_t>();
            if (res.mFlags.v7)
                res.mUnk3 = bs.read<uint8_t>();

            return res;
        }
        void write_peginfo(binstream& bs, const LevelTypes::PegInfo& p) {
            bs.write(p.mType);
            auto pc = p;
            pc.mFlags.v1 = p.mVariable;
            pc.mFlags.v3 = p.mCrumble;
            bs.write(pc.mFlags.asByte);

            if (p.mFlags.v2)
                bs.write(p.mUnk0);
            if (p.mFlags.v4)
                bs.write(p.mUnk1);
            if (p.mFlags.v5)
                bs.write(p.mUnk2);
            if (p.mFlags.v7)
                bs.write(p.mUnk3);
        }

        LevelTypes::GenericData read_generic(binstream& bs, const LevelTypes::GenericDataFlags flags) {
            LevelTypes::GenericData generic = {};
            if (flags.isRolly)  // 0
                generic.mRolly = bs.read<float>();
            if (flags.isBouncy)  // 1
                generic.mBouncy = bs.read<float>();
            if (flags.unk0)  // 4
                generic.mUnk0 = bs.read<int32_t>();
            if (flags.hasSolidColor) {  // 8
                generic.mSolidColor = {};
                generic.mSolidColor.asInt = bs.read<int32_t>();
            }
            if (flags.hasOutlineColor) {  // 9
                generic.mOutlineColor = {};
                generic.mOutlineColor.asInt = bs.read<int32_t>();
            }
            if (flags.hasImage)  // 10
                generic.mImage = read_string(bs);
            if (flags.hasImageDX)  // 11
                generic.mImageDX = bs.read<float>();
            if (flags.hasImageDY)  // 12
                generic.mImageDY = bs.read<float>();
            if (flags.hasRotation)  // 13
                generic.mRotation = bs.read<float>();
            if (flags.unk1)  // 16
                generic.mUnk1 = bs.read<int32_t>();
            if (flags.hasID)  // 17
                generic.mID = read_string(bs);
            if (flags.unk2)  // 18
                generic.mUnk2 = bs.read<int32_t>();
            if (flags.hasSound)  // 19
                generic.mSound = bs.read<uint8_t>();
            if (flags.hasLogic)  // 21
                generic.mLogic = read_string(bs);
            if (flags.hasMaxBounceVelocity)  // 23
                generic.mMaxBounceVelocity = bs.read<float>();
            if (flags.hasSubID)  // 26
                generic.mSubID = bs.read<float>();
            if (flags.hasFlipperFlags)  // 27
                generic.mFlipperFlags = bs.read<uint8_t>();
            if (flags.hasPegInfo)
                generic.mPegInfo = read_peginfo(bs);
            if (flags.hasMovementInfo)
                generic.mMovementLink = read_movement_link(bs);

            return generic;
        }
        void write_generic(binstream& bs, const LevelTypes::GenericDataFlags& flags, const LevelTypes::GenericData& generic) {
            if (flags.isRolly)  // 0
                bs.write(generic.mRolly);
            if (flags.isBouncy)  // 1
                bs.write(generic.mBouncy);
            if (flags.unk0)  // 4
                bs.write(generic.mUnk0);
            if (flags.hasSolidColor)  // 8
                bs.write(generic.mSolidColor.asInt);
            if (flags.hasOutlineColor)  // 9
                bs.write(generic.mOutlineColor.asInt);
            if (flags.hasImage)  // 10
                write_string(bs, generic.mImage);
            if (flags.hasImageDX)  // 11
                bs.write(generic.mImageDX);
            if (flags.hasImageDY)  // 12
                bs.write(generic.mImageDY);
            if (flags.hasRotation)  // 13
                bs.write(generic.mRotation);
            if (flags.unk1)  // 16
                bs.write(generic.mUnk1);
            if (flags.hasID)  // 17
                write_string(bs, generic.mID);
            if (flags.unk2)  // 18
                bs.write(generic.mUnk2);
            if (flags.hasSound)  // 19
                bs.write(generic.mSound);
            if (flags.hasLogic)  // 21
                write_string(bs, generic.mLogic);
            if (flags.hasMaxBounceVelocity)  // 23
                bs.write(generic.mMaxBounceVelocity);
            if (flags.hasSubID)  // 26
                bs.write(generic.mSubID);
            if (flags.hasFlipperFlags)  // 27
                bs.write(generic.mFlipperFlags);
            if (flags.hasPegInfo)
                write_peginfo(bs, generic.mPegInfo);
            if (flags.hasMovementInfo)
                write_movement_link(bs, generic.mMovementLink);
        }

        LevelTypes::Element read_element(binstream& bs, const uint32_t version) {
            LevelTypes::Element element = {};
            // log_debug_raw(" - (start: %d, ", bs.tell());
            element.magic = bs.read<int32_t>();
            if (element.magic != 1) {
                // log_debug_raw("not an element)\n");
                return element;
            }
            element.eType = bs.read<int32_t>();
            // log_debug_raw("type: %d)\n", element.eType);
            element.flags = {};
            if (version == 4) {  // TODO: no idea what the lower limit actually is, try and find it in ida
                const auto low = bs.read<uint8_t>();
                const auto mid = bs.read<uint8_t>();
                const auto high = bs.read<uint8_t>();
                element.flags.asInt = (high << 16) | (mid << 8) | low;
            }
            else
                element.flags.asInt = bs.read<uint32_t>();
            element.generic = read_generic(bs, element.flags);
            const auto entry_type = static_cast<LevelTypes::LevelEntryType>(element.eType);
            element.entry = new LevelTypes::Entry(entry_type);
            *element.entry = read_entry(bs, element.eType, version);
            return element;
        }
        void write_element(binstream& bs, const uint32_t version, const LevelTypes::Element& element) {
            bs.write(element.magic);
            if (element.magic != 1)
                return;
            bs.write(element.eType);
            if (version == 4) {  // TODO: no idea what the lower limit actually is, try and find it in ida
                const uint32_t flags = element.flags.asInt;
                const uint8_t low = flags & 0xFF;
                const uint8_t mid = flags >> 8 & 0xFF;
                const uint8_t high = flags >> 16 & 0xFF;
                bs.write(low);
                bs.write(mid);
                bs.write(high);
            }
            else
                bs.write(element.flags.asInt);
            write_generic(bs, element.flags, element.generic);
            write_entry(bs, version, *element.entry);
        }
    }

    LevelTypes::Level Level::LoadLevel(const void* buf, const uint32_t size) {
        LevelTypes::Level lvl = {};
        auto bs = binstream(buf, size);  // initialize binstream...
        bs.seek(0);  // ...and go back to the start

        lvl.version = bs.read<uint32_t>();
        lvl.sync_f = bs.read<uint8_t>();
        lvl.entries = bs.read<uint32_t>();
        for (int i = 0; i < lvl.entries; ++i) {
            // log_debug("parsing element #%d\n", i);
            lvl.Elements.emplace_back(LevelHelpers::read_element(bs, lvl.version));
        }

        lvl.valid = true;

        return lvl;
    }

    LevelTypes::Level Level::LoadLevel(const FileRef& lvl) {
        return LoadLevel(lvl.Data, lvl.Size);
    }

    LevelTypes::Level Level::LoadLevel(const std::filesystem::path &path) {
        if (!exists(path))
            return LevelTypes::Level{};  // valid = false

        const auto& file = path;
        const auto file_size = std::filesystem::file_size(file);
        auto* file_data = static_cast<char*>(malloc(file_size));
        std::ifstream fs(file, std::ifstream::in | std::ifstream::binary);
        fs.read(file_data, static_cast<std::streamsize>(file_size));
        fs.close();

        auto lvl = LoadLevel(file_data, file_size);
        free(file_data);
        return lvl;
    }

    LevelTypes::Level Level::LoadLevel(const Pak& pak, const std::filesystem::path& path) {
        const auto ref_path = Utils::forward_slash_ify(path.generic_string());
        if (!pak.HasFile(ref_path))
            return LevelTypes::Level{};  // valid = false
        const auto lvl = pak.GetFile(ref_path);
        return LoadLevel(lvl);
    }

    FileRef Level::BuildLevel(const LevelTypes::Level &lvl) {
        if (!lvl.valid)
            return FileRef{};
        auto bs = binstream();
        bs.write(lvl.version);
        bs.write(lvl.sync_f);
        bs.write(lvl.entries);
        for (const auto& e : lvl.Elements)
            LevelHelpers::write_element(bs, lvl.version, e);
        auto* res = malloc(bs.size());
        memcpy(res, bs.buffer(), bs.size());
        return FileRef{
            FileState::OK,
            res,
            bs.size()
        };
    }

    LevelTypes::RodEntry* Level::AccessRod(LevelTypes::Entry& entry) {
        return LevelTypes::Entry::GetRod(entry);
    }
    LevelTypes::PolygonEntry* Level::AccessPolygon(LevelTypes::Entry& entry) {
        return LevelTypes::Entry::GetPolygon(entry);
    }
    LevelTypes::CircleEntry* Level::AccessCircle(LevelTypes::Entry& entry) {
        return LevelTypes::Entry::GetCircle(entry);
    }
    LevelTypes::BrickEntry* Level::AccessBrick(LevelTypes::Entry& entry) {
        return LevelTypes::Entry::GetBrick(entry);
    }
    LevelTypes::TeleportEntry* Level::AccessTeleporter(LevelTypes::Entry& entry) {
        return LevelTypes::Entry::GetTeleporter(entry);
    }
    LevelTypes::EmitterEntry* Level::AccessEmitter(LevelTypes::Entry& entry) {
        return LevelTypes::Entry::GetEmitter(entry);
    }

    LevelTypes::RodEntry Level::CloneRod(LevelTypes::Entry& entry) {
        const auto e = AccessRod(entry);
        if (!e) return {};
        LevelTypes::RodEntry res = {};

        res.valid = true;
        res.mFlags = e->mFlags;
        res.mPointA = e->mPointA;
        res.mPointB = e->mPointB;
        res.mE = e->mE;
        res.mF = e->mF;

        return res;
    }
    LevelTypes::PolygonEntry Level::ClonePolygon(LevelTypes::Entry& entry) {
        const auto e = AccessPolygon(entry);
        if (!e) return {};
        LevelTypes::PolygonEntry res = {};

        res.valid = true;
        res.mFlagsA = e->mFlagsA;
        res.mFlagsB = e->mFlagsB;
        res.mRotation = e->mRotation;
        res.mUnk1 = e->mUnk1;
        res.mScale = e->mScale;
        res.mNormalDir = e->mNormalDir;
        res.mPos = e->mPos;
        res.mPoints = e->mPoints;
        res.mUnk2 = e->mUnk2;
        res.mGrowType = e->mGrowType;

        return res;

    }
    LevelTypes::CircleEntry Level::CloneCircle(LevelTypes::Entry& entry) {
        const auto e = AccessCircle(entry);
        if (!e) return {};
        LevelTypes::CircleEntry res = {};

        res.valid = true;
        res.mFlagsA = e->mFlagsA;
        res.mFlagsB = e->mFlagsB;
        res.mPos = e->mPos;
        res.mRadius = e->mRadius;

        return res;

    }
    LevelTypes::BrickEntry Level::CloneBrick(LevelTypes::Entry& entry) {
        const auto e = AccessBrick(entry);
        if (!e) return {};
        LevelTypes::BrickEntry res = {};

        res.valid = true;
        res.mFlagsA = e->mFlagsA;
        res.mFlagsB = e->mFlagsB;
        res.mFlagsC = e->mFlagsC;
        res.mUnk1 = e->mUnk1;
        res.mUnk2 = e->mUnk2;
        res.mUnk3 = e->mUnk3;
        res.mUnk4 = e->mUnk4;
        res.mPos = e->mPos;
        res.mUnk5 = e->mUnk5;
        res.mUnk6 = e->mUnk6;
        res.mUnk7 = e->mUnk7;
        res.mUnk8 = e->mUnk8;
        res.mUnk9 = e->mUnk9;
        res.mType = e->mType;
        res.mCurved = e->mCurved;
        res.mCurvedPoints = e->mCurvedPoints;
        res.mLeftAngle = e->mLeftAngle;
        res.mRightAngle = e->mRightAngle;
        res.mUnk10 = e->mUnk10;
        res.mSectorAngle = e->mSectorAngle;
        res.mWidth = e->mWidth;
        res.mLength = e->mLength;
        res.mAngle = e->mAngle;
        res.mTextureFlip = e->mTextureFlip;
        res.mUnk12 = e->mUnk12;

        return res;

    }
    LevelTypes::TeleportEntry Level::CloneTeleporter(LevelTypes::Entry& entry) {
        const auto e = AccessTeleporter(entry);
        if (!e) return {};
        LevelTypes::TeleportEntry res = {};

        res.valid = true;
        res.mFlags = e->mFlags;
        res.mWidth = e->mWidth;
        res.mHeight = e->mHeight;
        res.mUnk0 = e->mUnk0;
        res.mUnk1 = e->mUnk1;
        res.mUnk2 = e->mUnk2;

        res.mEntry = new LevelTypes::Element;
        *res.mEntry = CloneElement(*e->mEntry);

        res.mPos = e->mPos;
        res.mUnk3 = e->mUnk3;
        res.mUnk4 = e->mUnk4;

        return res;
    }
    LevelTypes::EmitterEntry Level::CloneEmitter(LevelTypes::Entry& entry) {
        const auto e = AccessEmitter(entry);
        if (!e) return {};
        LevelTypes::EmitterEntry res = {};

        res.valid = true;
        res.mMainVar = e->mMainVar;
        res.mFlags = e->mFlags;
        res.mImage = e->mImage;
        res.mWidth = e->mWidth;
        res.mHeight = e->mHeight;
        res.mMainVar0 = e->mMainVar0;
        res.mMainVar1 = e->mMainVar1;
        res.mMainVar2 = e->mMainVar2;
        res.mMainVar3 = e->mMainVar3;
        res.mUnknown0 = e->mUnknown0;
        res.mUnknown1 = e->mUnknown1;
        res.mPos = e->mPos;
        res.mEmitImage = e->mEmitImage;
        res.mUnknownEmitRate = e->mUnknownEmitRate;
        res.mUnknown2 = e->mUnknown2;
        res.mRotation = e->mRotation;
        res.mMaxQuantity = e->mMaxQuantity;
        res.mTimeBeforeFadeOut = e->mTimeBeforeFadeOut;
        res.mFadeInTime = e->mFadeInTime;
        res.mLifeDuration = e->mLifeDuration;
        res.mEmitRate = e->mEmitRate;
        res.mEmitAreaMultiplier = e->mEmitAreaMultiplier;
        res.mInitialRotation = e->mInitialRotation;
        res.mRotationVelocity = e->mRotationVelocity;
        res.mRotationUnknown = e->mRotationUnknown;
        res.mMinScale = e->mMinScale;
        res.mScaleVelocity = e->mScaleVelocity;
        res.mMaxRandScale = e->mMaxRandScale;
        res.mColourRed = e->mColourRed;
        res.mColourGreen = e->mColourGreen;
        res.mColourBlue = e->mColourBlue;
        res.mOpacity = e->mOpacity;
        res.mMinVelocityX = e->mMinVelocityX;
        res.mMinVelocityY = e->mMinVelocityY;
        res.mMaxVelocityX = e->mMaxVelocityX;
        res.mMaxVelocityY = e->mMaxVelocityY;
        res.mAccelerationX = e->mAccelerationX;
        res.mAccelerationY = e->mAccelerationY;
        res.mDirectionSpeed = e->mDirectionSpeed;
        res.mDirectionRandomSpeed = e->mDirectionRandomSpeed;
        res.mDirectionAcceleration = e->mDirectionAcceleration;
        res.mDirectionAngle = e->mDirectionAngle;
        res.mDirectionRandomAngle = e->mDirectionRandomAngle;
        res.mUnknownA = e->mUnknownA;
        res.mUnknownB = e->mUnknownB;

        return res;
    }
    LevelTypes::GenericData Level::CloneGenericData(const LevelTypes::GenericData& generic) {
        LevelTypes::GenericData res = {};

        res.mRolly = generic.mRolly;
        res.mBouncy = generic.mBouncy;
        res.mPegInfo = generic.mPegInfo;
        res.mMovementLink = CloneMovementLink(generic.mMovementLink);
        res.mUnk0 = generic.mUnk0;
        res.mSolidColor = generic.mSolidColor;
        res.mOutlineColor = generic.mOutlineColor;
        res.mImage = generic.mImage;
        res.mImageDX = generic.mImageDX;
        res.mImageDY = generic.mImageDY;
        res.mRotation = generic.mRotation;
        res.mUnk1 = generic.mUnk1;
        res.mID = generic.mID;
        res.mUnk2 = generic.mUnk2;
        res.mSound = generic.mSound;
        res.mLogic = generic.mLogic;
        res.mMaxBounceVelocity = generic.mMaxBounceVelocity;
        res.mSubID = generic.mSubID;
        res.mFlipperFlags = generic.mFlipperFlags;

        return res;
    }
    LevelTypes::MovementLink Level::CloneMovementLink(const LevelTypes::MovementLink& movement) {
        LevelTypes::MovementLink res = {};

        res.InternalLinkId = movement.InternalLinkId;

        res.InternalMovement.mInternalLinkID = movement.InternalMovement.mInternalLinkID;
        res.InternalMovement.mMovementShape = movement.InternalMovement.mMovementShape;
        res.InternalMovement.mType = movement.InternalMovement.mType;
        res.InternalMovement.mReverse = movement.InternalMovement.mReverse;
        res.InternalMovement.mAnchorPoint = movement.InternalMovement.mAnchorPoint;
        res.InternalMovement.mTimePeriod = movement.InternalMovement.mTimePeriod;
        res.InternalMovement.mFlags = movement.InternalMovement.mFlags;
        res.InternalMovement.mOffset = movement.InternalMovement.mOffset;
        res.InternalMovement.mRadius1 = movement.InternalMovement.mRadius1;
        res.InternalMovement.mStartPhase = movement.InternalMovement.mStartPhase;
        res.InternalMovement.mMoveRotation = movement.InternalMovement.mMoveRotation;
        res.InternalMovement.mRadius2 = movement.InternalMovement.mRadius2;
        res.InternalMovement.mPause1 = movement.InternalMovement.mPause1;
        res.InternalMovement.mPause2 = movement.InternalMovement.mPause2;
        res.InternalMovement.mPhase1 = movement.InternalMovement.mPhase1;
        res.InternalMovement.mPhase2 = movement.InternalMovement.mPhase2;
        res.InternalMovement.mPostDelayPhase = movement.InternalMovement.mPostDelayPhase;
        res.InternalMovement.mMaxAngle = movement.InternalMovement.mMaxAngle;
        res.InternalMovement.mUnknown8 = movement.InternalMovement.mUnknown8;
        res.InternalMovement.mRotation = movement.InternalMovement.mRotation;
        res.InternalMovement.mSubMovementOffsetX = movement.InternalMovement.mSubMovementOffsetX;
        res.InternalMovement.mSubMovementOffsetY = movement.InternalMovement.mSubMovementOffsetY;
        if (movement.InternalMovement.mSubMovementLink) {
            res.InternalMovement.mSubMovementLink = new LevelTypes::MovementLink;
            *res.InternalMovement.mSubMovementLink = CloneMovementLink(*movement.InternalMovement.mSubMovementLink);
        }
        res.InternalMovement.mSubMovementLink = movement.InternalMovement.mSubMovementLink;
        res.InternalMovement.mObjectX = movement.InternalMovement.mObjectX;
        res.InternalMovement.mObjectY = movement.InternalMovement.mObjectY;

        return res;
    }

    LevelTypes::Element Level::CloneElement(const LevelTypes::Element& element) {
        LevelTypes::Element res = {};

        res.magic = element.magic;
        res.eType = element.eType;
        res.flags = element.flags;
        res.generic = CloneGenericData(element.generic);

        const auto entry = element.entry;
        switch (res.eType) {  // TODO: get this working
            case LevelTypes::Rod: {
                const auto rod = LevelTypes::Entry::GetRod(entry);
                *rod = CloneRod(*entry); break;
            }
            case LevelTypes::Polygon: {
                const auto polygon = LevelTypes::Entry::GetPolygon(entry);
                *polygon = ClonePolygon(*entry); break;
            }
            case LevelTypes::Circle: {
                const auto circle = LevelTypes::Entry::GetCircle(entry);
                *circle = CloneCircle(*entry); break;
            }
            case LevelTypes::Brick: {
                const auto brick = LevelTypes::Entry::GetBrick(entry);
                *brick = CloneBrick(*entry); break;
            }
            case LevelTypes::Teleporter: {
                const auto teleporter = LevelTypes::Entry::GetTeleporter(entry);
                *teleporter = CloneTeleporter(*entry); break;
            }
            case LevelTypes::Emitter: {
                const auto emitter = LevelTypes::Entry::GetEmitter(entry);
                *emitter = CloneEmitter(*entry); break;
            }
            default: break;
        }

        return res;
    }

#pragma endregion

}