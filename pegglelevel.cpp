#include "binstream.h"
#include "iohelper.h"
#include "libpeggle.h"
#include "logma.h"
#include "utils.h"
#include <stdlib.h>

namespace Peggle {

#pragma region libpeggle_Level

#define visit_switch(var_name) std::visit([&]<typename T0>(T0&& var_name)
#define visit_run(v) , entry)
#define visit_header(type) using T = std::decay_t<T0>;if constexpr (std::is_same_v<T, type>)
#define visit_case(type) else if constexpr (std::is_same_v<T, type>)
#define visit_default() else

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
            LevelTypes::Entry entry = {};
            switch (static_cast<LevelTypes::LevelEntryTypes>(eType)) {
                case LevelTypes::Rod: {
                    entry = read_entry_rod(bs); break;
                }
                case LevelTypes::Polygon: {
                    entry = read_entry_polygon(bs, version); break;
                }
                case LevelTypes::Circle: {
                    entry = read_entry_circle(bs, version); break;
                }
                case LevelTypes::Brick: {
                    entry = read_entry_brick(bs, version); break;
                }
                case LevelTypes::Teleport: {
                    entry = read_entry_teleport(bs, version); break;
                }
                case LevelTypes::Emitter: {
                    entry = read_entry_emitter(bs); break;
                }
                default: {
                    // todo: raise exception for invalid entry type
                }
            }
            return entry;
        }
        void write_entry(binstream& bs, uint32_t version, const LevelTypes::Entry& entry) {
            visit_switch(e) {
                visit_header(LevelTypes::RodEntry) {
                    write_entry_rod(bs, e);
                }
                visit_case(LevelTypes::PolygonEntry) {
                    write_entry_polygon(bs, version, e);
                }
                visit_case(LevelTypes::CircleEntry) {
                    write_entry_circle(bs, version, e);
                }
                visit_case(LevelTypes::BrickEntry) {
                    write_entry_brick(bs, version, e);
                }
                visit_case(LevelTypes::TeleportEntry) {
                    write_entry_teleport(bs, version, e);
                }
                visit_case(LevelTypes::EmitterEntry) {
                    write_entry_emitter(bs, e);
                }
                visit_default() {
                    // todo: throw error for invalid type
                }
            }
            visit_run(entry);

            /*// this looks horrible :henyaStaree:
            std::visit([]<typename T0>(T0&& arg) {
                using T = std::decay_t<T0>;
                if constexpr (std::is_same_v<T, LevelTypes::RodEntry>) {

                }
                if constexpr (std::is_same_v<T, LevelTypes::PolygonEntry>) {

                }
                else {

                }
            }, entry);*/

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
            element.entry = read_entry(bs, element.eType, version);
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
            write_entry(bs, version, element.entry);
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

#undef visit_switch
#undef visit_run
#undef visit_header
#undef visit_case
#undef visit_default

#pragma endregion

}