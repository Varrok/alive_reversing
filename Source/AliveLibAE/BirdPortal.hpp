#pragma once

#include "BaseAnimatedWithPhysicsGameObject.hpp"
#include "Path.hpp"
#include "../AliveLibCommon/Function.hpp"

enum class LevelIds : s16;

enum class PortalType : s16
{
    eAbe_0 = 0,
    eWorker_1 = 1,
    eShrykull_2 = 2,
    // eMudTeleport_3 = 3, // TODO: not used ??
};

enum class PortalSide : s16
{
    eRight_0 = 0,
    eLeft_1 = 1,
};

struct Path_BirdPortal final : public Path_TLV
{
    PortalSide mEnterSide;
    LevelIds mExitLevel;
    s16 mExitPath;
    s16 mExitCamera;
    Scale_short mScale;
    s16 mMovieId;
    PortalType mPortalType;
    s16 mMudCountForShrykull;
    s16 mCreatePortalSwitchId;
    s16 mDeletePortalSwitchId;
};
ALIVE_ASSERT_SIZEOF_ALWAYS(Path_BirdPortal, 0x24);

struct Path_BirdPortalExit final : public Path_TLV
{
    PortalSide mExitSide;
    Scale_short mScale;
};
ALIVE_ASSERT_SIZEOF_ALWAYS(Path_BirdPortal, 0x24);

struct BirdPortal_State final
{
    AETypes mAEType;
    u8 mState;
    u8 mMudCountForShrykull;
    s32 mTlvInfo;
};
ALIVE_ASSERT_SIZEOF_ALWAYS(BirdPortal_State, 8);

class BirdPortalTerminator final : public ::BaseAnimatedWithPhysicsGameObject
{
public:
    BirdPortalTerminator(FP xpos, FP ypos, FP scale, PortalType portalType);
    virtual void VScreenChanged() override;
    
    void Fadeout();
};
ALIVE_ASSERT_SIZEOF(BirdPortalTerminator, 0xF4);

class OrbWhirlWind;
enum Event : s16;

class BirdPortal final : public BaseGameObject
{
public:
    BirdPortal(relive::Path_BirdPortal* pTlv, s32 tlvInfo);
    ~BirdPortal();
    
    virtual void VUpdate() override;
    virtual void VRender(PrimHeader** ppOt) override;
    virtual void VScreenChanged() override;
    virtual void VStopAudio() override;
    virtual s32 VGetSaveState(u8* pSaveBuffer) override;

    // New virtuals

    virtual s16 VPortalClipper(s16 bUnknown);
    virtual void VKillPortalClipper();
    virtual void VMudSaved();
    virtual bool VActivePortal();
    virtual void VGiveShrykull(s16 bPlaySound);
    virtual bool VAbeInsidePortal();
    virtual void VExitPortal();
    virtual bool VPortalExit_AbeExitting();
    virtual void VIncreaseTimerAndKillPortalClipper();
    virtual void VGetMapChange(EReliveLevelIds* level, u16* path, u16* camera, CameraSwapEffects* screenChangeEffect, u16* movieId);

    static s32 CreateFromSaveState(const u8* pBuffer);

private:
    void CreateDovesAndShrykullNumber();
    void CreateTerminators();
    s16 IsScaredAway();
    void KillTerminators();
    Event GetEvent();

private:
    s32 mTlvInfo = 0;

public:
    PortalType mPortalType = PortalType::eAbe_0;
    PortalSide mEnterSide = PortalSide::eRight_0;

public:
    enum class PortalStates : s16
    {
        CreatePortal_0 = 0,
        IdlePortal_1 = 1,
        JoinDovesInCenter_2 = 2,
        KillDoves_3 = 3,
        CreateTerminators_4 = 4,
        ExpandTerminators_5 = 5,
        ActivePortal_6 = 6,
        ShrykullGetDoves_7 = 7,
        Unused_8 = 8,
        GetShrykull_9 = 9,
        CollapseTerminators_10 = 10,
        StopSound_11 = 11,
        CreateFlash1_12 = 12,
        CreateFlash2_13 = 13,
        CreateFlash3_14 = 14,
        KillPortal_15 = 15,
        AbeInsidePortal_16 = 16,
        PortalExit_SetPosition_17 = 17,
        PortalExit_CreateTerminators_18 = 18,
        PortalExit_ExpandTerminators_19 = 19,
        PortalExit_AbeExitting_20 = 20,
        KillPortalClipper_21 = 21,
        FadeoutTerminators_22 = 22,
    };
    PortalStates mState = PortalStates::CreatePortal_0;

public:
    FP mXPos = {};
    FP mYPos = {};
    FP mExitX = {};
    FP mExitY = {};

public:
    FP mHitY = {};

private:
    s32 mThrowableIndicatorId = 0; // AE only
    s32 mDoveIds[6] = {}; // AE only
    s32 mTimer = 0;
    FP mSpriteScale = {};
    s16 mMovieId = 0;
    s16 mDeletePortalSwitchId = 0; // AE only
    s16 mDovesExist = 0;
    s32 mTerminatorId1 = 0; // object pointer in AO
    s32 mTerminatorId2 = 0; // object pointer in AO
    s32 mScreenClipperId1 = 0; // object pointer in AO
    s32 mScreenClipperId2 = 0; // object pointer in AO
    EReliveLevelIds mExitLevel = EReliveLevelIds::eNone;
    s16 mExitPath = 0;
    s16 mExitCamera = 0;
    s16 mMudCountForShrykull = 0;
    s16 mReceivedDovesCount = 0;
    OrbWhirlWind* mOrbWhirlWind = nullptr;
    EReliveLevelIds mCurrentLevel = EReliveLevelIds::eNone;
    s16 mCurrentPath = 0;
    s32 mSfxPlaying = 0;
};
ALIVE_ASSERT_SIZEOF(BirdPortal, 0x100);
