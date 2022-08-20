#include "stdafx_ao.h"
#include "Function.hpp"
#include "UXB.hpp"
#include "Sfx.hpp"
#include "ResourceManager.hpp"
#include "stdlib.hpp"
#include "Game.hpp"
#include "BaseBomb.hpp"
#include "../relive_lib/Events.hpp"
#include "../relive_lib/ScreenManager.hpp"
#include <math.h>

namespace AO {

UXB::UXB(Path_UXB* pTlv, s32 tlvInfo)
    : BaseAliveGameObject()
{
    mBaseGameObjectTypeId = ReliveTypes::eUXB;

    const AnimRecord rec = AO::AnimRec(AnimId::UXB_Active);
    u8** ppRes2 = ResourceManager::GetLoadedResource(ResourceManager::Resource_Animation, rec.mResourceId, 1, 0);
    Animation_Init(AnimId::UXB_Active, ppRes2);

    mAnim.mFlags.Set(AnimFlags::eBit15_bSemiTrans);
    mAnim.mRenderMode = TPageAbr::eBlend_0;

    mBaseGameObjectFlags.Set(Options::eInteractive_Bit8);
    field_1BC_flags.Clear(flags_1BC::eUnused_Bit0);
    field_10C_state = UXBState::eDelay_0;

    field_1B4_pattern_length = pTlv->mPatternLength;
    if (pTlv->mPatternLength < 1u || pTlv->mPatternLength > 4u)
    {
        field_1B4_pattern_length = 1;
    }

    field_1B8_pattern = pTlv->mPattern;
    if (!pTlv->mPattern) // If no pattern set, go to a default one.
    {
        field_1B8_pattern = 11111;
    }

    field_1B6_pattern_index = 0;

    // Single out a single digit, and use that digit as the new amount of red blinks before a green one.
    field_1BA_red_blink_count = (field_1B8_pattern / static_cast<s32>(pow(10, field_1B4_pattern_length - 1))) % 10;

    if (pTlv->mScale == Scale_short::eHalf_1)
    {
        mSpriteScale = FP_FromDouble(0.5);
        mAnim.mRenderLayer = Layer::eLayer_RollingBallBombMineCar_Half_16;
        mScale = Scale::Bg;
    }
    else
    {
        mSpriteScale = FP_FromInteger(1);
        mAnim.mRenderLayer = Layer::eLayer_RollingBallBombMineCar_35;
        mScale = Scale::Fg;
    }

    InitBlinkAnim();

    if (pTlv->field_1_unknown) // Stores the activated/deactivated state for UXB
    {
        if (pTlv->mStartState == UXBStartState::eOn_0)
        {
            u8** ppRes = ResourceManager::GetLoadedResource(ResourceManager::Resource_Palt, AOResourceID::kGrenflshAOResID, 0, 0);
            field_11C_anim.LoadPal(ppRes, 0);
            field_1BC_flags.Clear(flags_1BC::eIsRed_Bit1);

            field_11C_anim.Set_Animation_Data(AnimId::Bomb_RedGreenTick, nullptr);

            if (gMap.Is_Point_In_Current_Camera(
                    mCurrentLevel,
                    mCurrentPath,
                    mXPos,
                    mYPos,
                    0))
            {
                SfxPlayMono(SoundEffect::GreenTick_3, 35, 0);
            }

            mAnim.Set_Animation_Data(AnimId::UXB_Disabled, nullptr);

            field_10C_state = UXBState::eDeactivated_3;
            field_10E_starting_state = UXBState::eDelay_0;
        }
        else
        {
            field_10E_starting_state = UXBState::eDeactivated_3;
        }
    }
    else
    {
        if (pTlv->mStartState == UXBStartState::eOn_0)
        {
            field_10E_starting_state = UXBState::eDelay_0;
        }
        else
        {
            u8** ppPal = ResourceManager::GetLoadedResource(ResourceManager::Resource_Palt, AOResourceID::kGrenflshAOResID, 0, 0);
            field_11C_anim.LoadPal(ppPal, 0);
            field_1BC_flags.Clear(flags_1BC::eIsRed_Bit1);

            field_11C_anim.Set_Animation_Data(AnimId::Bomb_RedGreenTick, nullptr);

            mAnim.Set_Animation_Data(AnimId::UXB_Disabled, nullptr);

            field_10E_starting_state = UXBState::eDeactivated_3;
            field_10C_state = UXBState::eDeactivated_3;
        }
    }

    mXPos = FP_FromInteger(pTlv->mTopLeft.x + 12);
    mYPos = FP_FromInteger(pTlv->mTopLeft.y + 24);

    field_114_tlvInfo = tlvInfo;
    field_118_next_state_frame = sGnFrame;
    field_110_disabled_resources = static_cast<s16>(pTlv->mDisabledResources);

    ResourceManager::GetLoadedResource(ResourceManager::Resource_Animation, AOResourceID::kAbebombAOResID, 1, 0);
    ResourceManager::GetLoadedResource(ResourceManager::Resource_Animation, AOResourceID::kDebrisID00AOResID, 1, 0);
    ResourceManager::GetLoadedResource(ResourceManager::Resource_Animation, AOResourceID::kBgexpldAOResID, 1, 0);
    ResourceManager::GetLoadedResource(ResourceManager::Resource_Palt, AOResourceID::kGrenflshAOResID, 1, 0);

    if (!(field_110_disabled_resources & 1))
    {
        ResourceManager::GetLoadedResource(ResourceManager::Resource_Animation, AOResourceID::kAbeblowAOResID, 1, 0);
    }

    if (!(field_110_disabled_resources & 4))
    {
        ResourceManager::GetLoadedResource(ResourceManager::Resource_Animation, AOResourceID::kElmblowAOResID_217, 1, 0);
    }

    if (!(field_110_disabled_resources & 2))
    {
        ResourceManager::GetLoadedResource(ResourceManager::Resource_Animation, AOResourceID::kSlogBlowAOResID, 1, 0);
    }

    if (gMap.mCurrentLevel == EReliveLevelIds::eStockYards || gMap.mCurrentLevel == EReliveLevelIds::eStockYardsReturn)
    {
        field_1BC_flags.Clear(flags_1BC::eIsRed_Bit1);
        mRGB.SetRGB(80, 90, 110);
        ResourceManager::GetLoadedResource(ResourceManager::Resource_Palt, AOResourceID::kAbeblowAOResID, 1, 0);
        ResourceManager::GetLoadedResource(ResourceManager::Resource_Palt, AOResourceID::kSlogBlowAOResID, 1, 0);
    }

    const FP gridSnap = ScaleToGridSize(mSpriteScale);
    mBaseGameObjectFlags.Set(Options::eInteractive_Bit8);

    mCollectionRect.x = mXPos - (gridSnap / FP_FromInteger(2));
    mCollectionRect.y = mYPos - gridSnap;
    mCollectionRect.w = mXPos + (gridSnap / FP_FromInteger(2));
    mCollectionRect.h = mYPos;
}

void UXB::InitBlinkAnim()
{
    const AnimRecord& tickRec = AO::AnimRec(AnimId::Bomb_RedGreenTick);
    u8** ppRes = ResourceManager::GetLoadedResource(ResourceManager::Resource_Animation, tickRec.mResourceId, 1, 0);
    if (field_11C_anim.Init(
            AnimId::Bomb_RedGreenTick,
            this,
            ppRes))
    {
        field_11C_anim.mFlags.Set(AnimFlags::eBit15_bSemiTrans);
        field_11C_anim.mFlags.Set(AnimFlags::eBit16_bBlending);

        field_11C_anim.mRenderLayer = mAnim.mRenderLayer;
        field_11C_anim.field_14_scale = mSpriteScale;
        field_11C_anim.mRed = 128;
        field_11C_anim.mGreen = 128;
        field_11C_anim.mBlue = 128;
        field_11C_anim.mRenderMode = TPageAbr::eBlend_1;
    }
    else
    {
        mBaseGameObjectFlags.Set(Options::eListAddFailed_Bit1);
    }
}


UXB::~UXB()
{
    if (field_10C_state != UXBState::eExploding_2 || static_cast<s32>(sGnFrame) < field_118_next_state_frame)
    {
        Path::TLV_Reset(field_114_tlvInfo, -1, 0, 0);
    }
    else
    {
        Path::TLV_Reset(field_114_tlvInfo, -1, 0, 1);
    }

    ResourceManager::FreeResource_455550(ResourceManager::GetLoadedResource(ResourceManager::Resource_Animation, AOResourceID::kAbebombAOResID, 0, 0));
    ResourceManager::FreeResource_455550(ResourceManager::GetLoadedResource(ResourceManager::Resource_Animation, AOResourceID::kDebrisID00AOResID, 0, 0));
    ResourceManager::FreeResource_455550(ResourceManager::GetLoadedResource(ResourceManager::Resource_Animation, AOResourceID::kBgexpldAOResID, 0, 0));
    ResourceManager::FreeResource_455550(ResourceManager::GetLoadedResource(ResourceManager::Resource_Palt, AOResourceID::kGrenflshAOResID, 0, 0));

    if (!(field_110_disabled_resources & 1))
    {
        ResourceManager::FreeResource_455550(ResourceManager::GetLoadedResource(ResourceManager::Resource_Animation, AOResourceID::kAbeblowAOResID, 0, 0));
    }

    if (!(field_110_disabled_resources & 4))
    {
        ResourceManager::FreeResource_455550(ResourceManager::GetLoadedResource(ResourceManager::Resource_Animation, AOResourceID::kElmblowAOResID_217, 0, 0));
    }

    if (!(field_110_disabled_resources & 2))
    {
        ResourceManager::FreeResource_455550(ResourceManager::GetLoadedResource(ResourceManager::Resource_Animation, AOResourceID::kSlogBlowAOResID, 0, 0));
    }

    ResourceManager::FreeResource_455550(ResourceManager::GetLoadedResource(ResourceManager::Resource_Palt, AOResourceID::kAbeblowAOResID, 0, 0));
    ResourceManager::FreeResource_455550(ResourceManager::GetLoadedResource(ResourceManager::Resource_Palt, AOResourceID::kSlogBlowAOResID, 0, 0));

    field_11C_anim.VCleanUp();

    mBaseGameObjectFlags.Clear(Options::eInteractive_Bit8);
}

void UXB::VScreenChanged()
{
    if (gMap.mCurrentLevel != gMap.mNextLevel || gMap.mCurrentPath != gMap.mNextPath)
    {
        if (field_10E_starting_state == UXBState::eDeactivated_3 && field_10C_state != UXBState::eDeactivated_3)
        {
            Path::TLV_Reset(field_114_tlvInfo, 1, 1u, 0);
            mBaseGameObjectFlags.Set(BaseGameObject::eDead);
        }
        else if (field_10E_starting_state != UXBState::eDelay_0 || field_10C_state != UXBState::eDeactivated_3)
        {
            Path::TLV_Reset(field_114_tlvInfo, 0, 1u, 0);
            mBaseGameObjectFlags.Set(BaseGameObject::eDead);
        }
        else
        {
            Path::TLV_Reset(field_114_tlvInfo, 1, 1u, 0);
            mBaseGameObjectFlags.Set(BaseGameObject::eDead);
        }
    }
}

s16 UXB::VTakeDamage(BaseGameObject* pFrom)
{
    if (mBaseGameObjectFlags.Get(BaseGameObject::eDead))
    {
        return 0;
    }

    switch (pFrom->mBaseGameObjectTypeId)
    {
        case ReliveTypes::eAbe:
            if (field_10C_state == UXBState::eDeactivated_3)
            {
                return 0;
            }
            break;

        case ReliveTypes::eAbilityRing:
        case ReliveTypes::eExplosion:
        case ReliveTypes::eShrykull:
            break;

        default:
            return 0;
    }

    mBaseGameObjectFlags.Set(BaseGameObject::eDead);

    relive_new BaseBomb(
        mXPos,
        mYPos,
        0,
        mSpriteScale);

    field_10C_state = UXBState::eExploding_2;
    field_118_next_state_frame = sGnFrame;

    return 1;
}

void UXB::VOnThrowableHit(BaseGameObject* /*pFrom*/)
{
    relive_new BaseBomb(
        mXPos,
        mYPos,
        0,
        mSpriteScale);

    mBaseGameObjectFlags.Set(BaseGameObject::eDead);
    field_10C_state = UXBState::eExploding_2;
    field_118_next_state_frame = sGnFrame;
}

void UXB::VOnPickUpOrSlapped()
{
    if (field_10C_state != UXBState::eExploding_2)
    {
        if (field_10C_state != UXBState::eDeactivated_3 || field_118_next_state_frame > static_cast<s32>(sGnFrame))
        {
            if (field_1BA_red_blink_count)
            {
                field_10C_state = UXBState::eExploding_2;
                field_118_next_state_frame = sGnFrame + 2;
            }
            else
            {
                field_11C_anim.Set_Animation_Data(AnimId::Bomb_RedGreenTick, nullptr);
                if (gMap.Is_Point_In_Current_Camera(
                        mCurrentLevel,
                        mCurrentPath,
                        mXPos,
                        mYPos,
                        0))
                {
                    SfxPlayMono(SoundEffect::GreenTick_3, 35, 0);
                }
                mAnim.Set_Animation_Data(AnimId::UXB_Toggle, nullptr);
                field_10C_state = UXBState::eDeactivated_3;
                field_118_next_state_frame = sGnFrame + 10;
            }
        }
        else
        {
            field_10C_state = UXBState::eDelay_0;
            mBaseGameObjectUpdateDelay = 6;
            mAnim.Set_Animation_Data(AnimId::UXB_Active, nullptr);
            if (gMap.Is_Point_In_Current_Camera(
                    mCurrentLevel,
                    mCurrentPath,
                    mXPos,
                    mYPos,
                    0))
            {
                SfxPlayMono(SoundEffect::RedTick_4, 35, 0);
            }
        }
    }
}

void UXB::VUpdate()
{
    switch (field_10C_state)
    {
        case UXBState::eDelay_0:
            if (IsColliding())
            {
                field_10C_state = UXBState::eExploding_2;
                field_118_next_state_frame = sGnFrame + 2;
            }
            else if (field_118_next_state_frame <= static_cast<s32>(sGnFrame))
            {
                field_10C_state = UXBState::eActive_1;
                field_11C_anim.Set_Animation_Data(AnimId::Bomb_Flash, nullptr);
                field_118_next_state_frame = sGnFrame + 2;
            }
            break;

        case UXBState::eActive_1:
            if (IsColliding())
            {
                field_10C_state = UXBState::eExploding_2;
                field_118_next_state_frame = sGnFrame + 2;
            }
            else if (field_118_next_state_frame <= static_cast<s32>(sGnFrame))
            {
                if (field_1BA_red_blink_count)
                {
                    field_1BA_red_blink_count--;
                    if (field_1BA_red_blink_count == 0)
                    {
                        field_11C_anim.LoadPal(ResourceManager::GetLoadedResource(ResourceManager::Resource_Palt, AOResourceID::kGrenflshAOResID, 0, 0), 0);
                        field_1BC_flags.Clear(flags_1BC::eIsRed_Bit1);
                    }
                }
                else
                {
                    const FrameInfoHeader* pFrameInfo = field_11C_anim.Get_FrameHeader(-1);

                    const FrameHeader* pFrameHeader = reinterpret_cast<FrameHeader*>(&(*field_11C_anim.field_20_ppBlock)[pFrameInfo->field_0_frame_header_offset]);

                    field_11C_anim.LoadPal(
                        field_11C_anim.field_20_ppBlock,
                        pFrameHeader->field_0_clut_offset);

                    field_1BC_flags.Set(flags_1BC::eIsRed_Bit1);

                    field_1B6_pattern_index++;

                    if (field_1B6_pattern_index >= field_1B4_pattern_length)
                    {
                        field_1B6_pattern_index = 0;
                    }

                    // Single out a single digit, and use that digit as the new amount of red blinks before a green one.
                    field_1BA_red_blink_count = (field_1B8_pattern / static_cast<s32>(pow(10, field_1B4_pattern_length - field_1B6_pattern_index - 1))) % 10;
                }

                field_11C_anim.Set_Animation_Data(AnimId::Bomb_RedGreenTick, nullptr);
                
                if (field_1BC_flags.Get(flags_1BC::eIsRed_Bit1))
                {
                    if (gMap.Is_Point_In_Current_Camera(
                            mCurrentLevel,
                            mCurrentPath,
                            mXPos,
                            mYPos,
                            0))
                    {
                        SfxPlayMono(SoundEffect::RedTick_4, 35, 0);
                    }
                }
                else if (gMap.Is_Point_In_Current_Camera(
                             mCurrentLevel,
                             mCurrentPath,
                             mXPos,
                             mYPos,
                             0))
                {
                    SfxPlayMono(SoundEffect::GreenTick_3, 35, 0);
                }
                field_10C_state = UXBState::eDelay_0;
                field_118_next_state_frame = sGnFrame + 10; // UXB change color delay
            }
            break;

        case UXBState::eExploding_2:
            if (static_cast<s32>(sGnFrame) >= field_118_next_state_frame)
            {
                relive_new BaseBomb(
                    mXPos,
                    mYPos,
                    0,
                    mSpriteScale);
                mBaseGameObjectFlags.Set(BaseGameObject::eDead);
            }
            break;
    }

    if (field_10C_state != UXBState::eExploding_2)
    {
        if (EventGet(kEventDeathReset))
        {
            if (field_10E_starting_state != UXBState::eDeactivated_3 || field_10C_state == UXBState::eDeactivated_3)
            {
                if (field_10E_starting_state != UXBState::eDelay_0 || field_10C_state != UXBState::eDeactivated_3)
                {
                    Path::TLV_Reset(field_114_tlvInfo, 0, 1u, 0);
                }
                else
                {
                    Path::TLV_Reset(field_114_tlvInfo, 1, 1u, 0);
                }
            }
            else
            {
                Path::TLV_Reset(field_114_tlvInfo, 1, 1u, 0);
            }
            mBaseGameObjectFlags.Set(BaseGameObject::eDead);
        }
    }
}

s16 UXB::IsColliding()
{
    const PSX_RECT uxbBound = VGetBoundingRect();

    for (s32 i = 0; i < gBaseAliveGameObjects->Size(); i++)
    {
        BaseAliveGameObject* pObj = gBaseAliveGameObjects->ItemAt(i);
        if (!pObj)
        {
            break;
        }

        if (pObj->mBaseAliveGameObjectFlags.Get(Flags_10A::e10A_Bit4_SetOffExplosives))
        {
            if (pObj->mAnim.mFlags.Get(AnimFlags::eBit3_Render))
            {
                const PSX_RECT objBound = pObj->VGetBoundingRect();

                const s32 objX = FP_GetExponent(pObj->mXPos);
                const s32 objY = FP_GetExponent(pObj->mYPos);

                if (objX > uxbBound.x && objX < uxbBound.w && objY < uxbBound.h + 5 && uxbBound.x <= objBound.w && uxbBound.w >= objBound.x && uxbBound.h >= objBound.y && uxbBound.y <= objBound.h && pObj->mSpriteScale == mSpriteScale)
                {
                    return 1;
                }
            }
        }
    }
    return 0;
}

void UXB::VRender(PrimHeader** ppOt)
{
    if (gMap.Is_Point_In_Current_Camera(
            mCurrentLevel,
            mCurrentPath,
            mXPos,
            mYPos,
            0))
    {
        field_11C_anim.VRender(
            FP_GetExponent(mXPos
                           + FP_FromInteger(pScreenManager->mCamXOff)
                           - pScreenManager->mCamPos->x),
            FP_GetExponent(mYPos
                           + (FP_FromInteger(pScreenManager->mCamYOff) - FP_NoFractional(mSpriteScale * FP_FromInteger(12)))
                           - pScreenManager->mCamPos->y),
            ppOt,
            0,
            0);

        PSX_RECT rect = {};
        field_11C_anim.Get_Frame_Rect(&rect);
        pScreenManager->InvalidateRectCurrentIdx(
            rect.x,
            rect.y,
            rect.w,
            rect.h);

        BaseAnimatedWithPhysicsGameObject::VRender(ppOt);
    }
}

} // namespace AO
