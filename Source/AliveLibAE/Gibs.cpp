#include "stdafx.h"
#include "Gibs.hpp"
#include "Function.hpp"
#include "Game.hpp"
#include "Abe.hpp"
#include "Math.hpp"
#include "stdlib.hpp"
#include "ScreenManager.hpp"
#include "AnimResources.hpp"

const TintEntry kGibTints_55C744[] = {
    {EReliveLevelIds::eMines, 87u, 103u, 67u},
    {EReliveLevelIds::eNecrum, 87u, 103u, 67u},
    {EReliveLevelIds::eMudomoVault, 87u, 103u, 67u},
    {EReliveLevelIds::eMudancheeVault, 87u, 103u, 67u},
    {EReliveLevelIds::eFeeCoDepot, 87u, 103u, 67u},
    {EReliveLevelIds::eBarracks, 87u, 103u, 67u},
    {EReliveLevelIds::eMudancheeVault_Ender, 87u, 103u, 67u},
    {EReliveLevelIds::eBonewerkz, 87u, 103u, 67u},
    {EReliveLevelIds::eBrewery, 87u, 103u, 67u},
    {EReliveLevelIds::eBrewery_Ender, 87u, 103u, 67u},
    {EReliveLevelIds::eMudomoVault_Ender, 87u, 103u, 67u},
    {EReliveLevelIds::eFeeCoDepot_Ender, 87u, 103u, 67u},
    {EReliveLevelIds::eBarracks_Ender, 87u, 103u, 67u},
    {EReliveLevelIds::eBonewerkz_Ender, 87u, 103u, 67u},
    {EReliveLevelIds::eNone, 87u, 103u, 67u}};

ALIVE_VAR(1, 0x550e80, s16, sGibRandom_550E80, 13);

FP Random_40FAF0(FP scale)
{
    return FP_FromRaw((Math_NextRandom() - 128) << sGibRandom_550E80) * scale;
}

Gibs::Gibs(GibType gibType, FP xpos, FP ypos, FP xOff, FP yOff, FP scale, bool bMakeSmaller)
    : BaseAnimatedWithPhysicsGameObject(0)
{
    field_F4_not_used = nullptr;

    AnimRecord headGib = {};
    AnimRecord armGib = {};
    AnimRecord bodyGib = {};

    switch (gibType)
    {
        case GibType::Abe_0:
            [[fallthrough]];

        case GibType::Mud_3:
            headGib = AnimRec(AnimId::Abe_Head_Gib);
            armGib = AnimRec(AnimId::Abe_Arm_Gib);
            bodyGib = AnimRec(AnimId::Abe_Body_Gib);
            break;

        case GibType::Slig_1:
            headGib = AnimRec(AnimId::Slig_Head_Gib);
            armGib = AnimRec(AnimId::Slig_Arm_Gib);
            bodyGib = AnimRec(AnimId::Slig_Body_Gib);
            break;

        case GibType::Slog_2:
            headGib = AnimRec(AnimId::Slog_Head_Gib);
            armGib = AnimRec(AnimId::Slog_Body_Gib); // No arms
            bodyGib = AnimRec(AnimId::Slog_Body_Gib);
            break;

        case GibType::BlindMud_4:
            headGib = AnimRec(AnimId::BlindMud_Head_Gib);
            armGib = AnimRec(AnimId::BlindMud_Arm_Gib);
            bodyGib = AnimRec(AnimId::BlindMud_Body_Gib);
            break;

        case GibType::Metal_5:
            // No body parts, all metal
            headGib = AnimRec(AnimId::Metal_Gib);
            armGib = AnimRec(AnimId::Metal_Gib);
            bodyGib = AnimRec(AnimId::Metal_Gib);
            break;

        case GibType::Glukkon_6:
            headGib = AnimRec(AnimId::Glukkon_Head_Gib);
            armGib = AnimRec(AnimId::Glukkon_Arm_Gib);
            bodyGib = AnimRec(AnimId::Glukkon_Body_Gib);
            break;

        case GibType::Aslik_7:
            headGib = AnimRec(AnimId::Aslik_Head_Gib);
            armGib = AnimRec(AnimId::Aslik_Arm_Gib);
            bodyGib = AnimRec(AnimId::Aslik_Body_Gib);
            break;

        case GibType::Dripik_8:
            headGib = AnimRec(AnimId::Dripik_Head_Gib);
            armGib = AnimRec(AnimId::Dripik_Arm_Gib);
            bodyGib = AnimRec(AnimId::Dripik_Body_Gib);
            break;

        case GibType::Phleg_9:
            headGib = AnimRec(AnimId::Phleg_Head_Gib);
            armGib = AnimRec(AnimId::Phleg_Arm_Gib);
            bodyGib = AnimRec(AnimId::Phleg_Body_Gib);
            break;

        case GibType::Fleech_10:
            headGib = AnimRec(AnimId::Fleech_Head_Gib);
            armGib = AnimRec(AnimId::Fleech_Body_Gib); // No arms
            bodyGib = AnimRec(AnimId::Fleech_Body_Gib);
            break;
    }

    // TODO: It is assumed all 3 gib parts have the same resource id - might not be true for mods
    u8** ppAnimData = Add_Resource(ResourceManager::Resource_Animation, headGib.mResourceId);

    // TODO: It is assumed all 3 gib parts use the same pal - might not be true for mods
    u8** ppRes = nullptr;
    if (headGib.mPalOverride != PalId::Default)
    {
        ppRes = Add_Resource(ResourceManager::Resource_Palt, PalRec(headGib.mPalOverride).mResourceId);
    }

    // The base class renders the head gib
    Animation_Init(
        headGib.mFrameTableOffset,
        headGib.mMaxW,
        headGib.mMaxH,
        ppAnimData,
        1);

    if (mBaseGameObjectFlags.Get(BaseGameObject::eListAddFailed_Bit1))
    {
        return;
    }

    mBaseAnimatedWithPhysicsGameObject_SpriteScale = scale;
    mBaseAnimatedWithPhysicsGameObject_XPos = xpos;
    mBaseAnimatedWithPhysicsGameObject_YPos = ypos + FP_FromInteger(2);

    field_100_timer = sGnFrame + 91;

    if (scale == FP_FromInteger(1))
    {
        field_F8_z = FP_FromInteger(0);
        mBaseAnimatedWithPhysicsGameObject_Anim.mRenderLayer = Layer::eLayer_FG1_37;
        mBaseAnimatedWithPhysicsGameObject_Scale = Scale::Fg;
    }
    else if (scale == FP_FromDouble(0.5))
    {
        field_F8_z = FP_FromInteger(100);
        mBaseAnimatedWithPhysicsGameObject_Anim.mRenderLayer = Layer::eLayer_Foreground_Half_17;
        mBaseAnimatedWithPhysicsGameObject_Scale = Scale::Bg;
    }
    else
    {
        // Not a valid scale
        mBaseGameObjectFlags.Set(BaseGameObject::eDead);
    }

    field_5D6_bMakeSmaller = bMakeSmaller;
    mBaseAnimatedWithPhysicsGameObject_VelX = xOff + Random_40FAF0(scale);

    // OG Bug? WTF?? Looks like somehow they didn't condition this param correctly
    // because mBaseAnimatedWithPhysicsGameObject_VelY and field_FC_dz are always overwritten
    if (!field_5D6_bMakeSmaller)
    {
        mBaseAnimatedWithPhysicsGameObject_VelY = yOff + Random_40FAF0(scale);
        field_FC_dz = FP_Abs(Random_40FAF0(scale) / FP_FromInteger(2));
    }

    sGibRandom_550E80 = 12;

    mBaseAnimatedWithPhysicsGameObject_VelY = (yOff + Random_40FAF0(scale)) / FP_FromInteger(2);
    field_FC_dz = FP_Abs(Random_40FAF0(scale) / FP_FromInteger(4));

    if (gibType == GibType::Abe_0)
    {
        SetTint(sTintTable_Abe_554D20, gMap.mCurrentLevel);
    }
    else if (gibType == GibType::Mud_3)
    {
        SetTint(kGibTints_55C744, gMap.mCurrentLevel);
    }
    else if (gibType == GibType::BlindMud_4)
    {
        mBaseAnimatedWithPhysicsGameObject_RGB.SetRGB(63, 63, 63);
    }

    field_5D4_parts_used_count = 4;

    GibPart* pPart = &field_104_parts[0];
    for (s16 i = 0; i < field_5D4_parts_used_count; i++)
    {
        if (i < 2)
        {
            // 2 arm parts
            if (!pPart->field_18_animation.Init(
                    armGib.mFrameTableOffset,
                    gAnimations,
                    this,
                    armGib.mMaxW,
                    armGib.mMaxH,
                    ppAnimData))
            {
                field_5D4_parts_used_count = i;
                mBaseGameObjectFlags.Set(BaseGameObject::eDead);
                return;
            }
        }
        else
        {
            // 2 body parts
            if (!pPart->field_18_animation.Init(
                    bodyGib.mFrameTableOffset,
                    gAnimations,
                    this,
                    bodyGib.mMaxW,
                    bodyGib.mMaxH,
                    ppAnimData))
            {
                field_5D4_parts_used_count = i;
                mBaseGameObjectFlags.Set(BaseGameObject::eDead);
                return;
            }
        }

        pPart->field_18_animation.mRenderLayer = mBaseAnimatedWithPhysicsGameObject_Anim.mRenderLayer;
        pPart->field_18_animation.field_14_scale = scale;
        pPart->field_18_animation.mAnimFlags.Clear(AnimFlags::eBit16_bBlending);

        pPart->field_18_animation.mRed = static_cast<u8>(mBaseAnimatedWithPhysicsGameObject_RGB.r);
        pPart->field_18_animation.mGreen = static_cast<u8>(mBaseAnimatedWithPhysicsGameObject_RGB.g);
        pPart->field_18_animation.mBlue = static_cast<u8>(mBaseAnimatedWithPhysicsGameObject_RGB.b);

        pPart->field_0_x = mBaseAnimatedWithPhysicsGameObject_XPos;
        pPart->field_4_y = mBaseAnimatedWithPhysicsGameObject_YPos;
        pPart->field_8_z = field_F8_z;

        pPart->field_C_dx = xOff + Random_40FAF0(scale);

        if (field_5D6_bMakeSmaller)
        {
            pPart->field_10_dy = (yOff + Random_40FAF0(scale)) / FP_FromInteger(2);
            pPart->field_14_dz = FP_Abs(Random_40FAF0(scale) / FP_FromInteger(4));
        }
        else
        {
            pPart->field_10_dy = yOff + Random_40FAF0(scale);
            pPart->field_14_dz = FP_Abs(Random_40FAF0(scale) / FP_FromInteger(2));
        }

        pPart->field_18_animation.mAnimFlags.Clear(AnimFlags::eBit15_bSemiTrans);

        if (ppRes)
        {
            pPart->field_18_animation.LoadPal(ppRes, 0);
        }

        pPart++;
    }
}

Gibs::~Gibs()
{
    for (s32 i = 0; i < field_5D4_parts_used_count; i++)
    {
        field_104_parts[i].field_18_animation.VCleanUp();
    }
}

void Gibs::VUpdate()
{
    mBaseAnimatedWithPhysicsGameObject_XPos += mBaseAnimatedWithPhysicsGameObject_VelX;
    mBaseAnimatedWithPhysicsGameObject_YPos += mBaseAnimatedWithPhysicsGameObject_VelY;
    field_F8_z += field_FC_dz;

    mBaseAnimatedWithPhysicsGameObject_VelY += FP_FromDouble(0.25);

    if (field_F8_z + FP_FromInteger(100) < FP_FromInteger(15))
    {
        const FP dz = -field_FC_dz;
        field_FC_dz = dz;
        field_F8_z += dz;
    }

    for (s32 i = 0; i < field_5D4_parts_used_count; i++)
    {
        field_104_parts[i].field_0_x += field_104_parts[i].field_C_dx;
        field_104_parts[i].field_4_y += field_104_parts[i].field_10_dy;
        field_104_parts[i].field_8_z += field_104_parts[i].field_14_dz;

        field_104_parts[i].field_10_dy += FP_FromDouble(0.25);

        if (field_104_parts[i].field_8_z + FP_FromInteger(100) < FP_FromInteger(15))
        {
            const FP dz = -field_104_parts[i].field_14_dz;
            field_104_parts[i].field_14_dz = dz;
            field_104_parts[i].field_8_z += dz;
        }
    }

    if (static_cast<s32>(sGnFrame) > field_100_timer)
    {
        mBaseGameObjectFlags.Set(BaseGameObject::eDead);
    }
}

void Gibs::VRender(PrimHeader** ppOt)
{
    if (sNum_CamSwappers_5C1B66 > 0)
    {
        // Don't do anything during screen change
        return;
    }

    mBaseAnimatedWithPhysicsGameObject_SpriteScale = FP_FromInteger(100) / (field_F8_z + FP_FromInteger(100));
    if (field_5D6_bMakeSmaller)
    {
        mBaseAnimatedWithPhysicsGameObject_SpriteScale /= FP_FromInteger(2);
    }

    // Head part rendering
    BaseAnimatedWithPhysicsGameObject::VRender(ppOt);

   const FP camXPos = pScreenManager->CamXPos();
    const FP camYPos = pScreenManager->CamYPos();

    for (s32 i = 0; i < field_5D4_parts_used_count; i++)
    {
        // Part is within camera X?
        if (field_104_parts[i].field_0_x >= camXPos && field_104_parts[i].field_0_x <= camXPos + FP_FromInteger(640))
        {
            // Part is within camera Y?
            if (field_104_parts[i].field_4_y >= camYPos && field_104_parts[i].field_4_y <= camYPos + FP_FromInteger(240))
            {
                field_104_parts[i].field_18_animation.field_14_scale = FP_FromInteger(100) / (field_104_parts[i].field_8_z + FP_FromInteger(100));

                if (field_5D6_bMakeSmaller)
                {
                    field_104_parts[i].field_18_animation.field_14_scale /= FP_FromInteger(2);
                }

                if (field_104_parts[i].field_18_animation.field_14_scale < FP_FromInteger(1))
                {
                    field_104_parts[i].field_18_animation.mRenderLayer = Layer::eLayer_Foreground_Half_17;
                }
                else
                {
                    field_104_parts[i].field_18_animation.mRenderLayer = Layer::eLayer_FG1_37;
                }

                if (field_104_parts[i].field_18_animation.field_14_scale <= FP_FromInteger(1))
                {
                    const s32 xpos = FP_GetExponent(field_104_parts[i].field_0_x - camXPos);
                    const s32 ypos = FP_GetExponent(field_104_parts[i].field_4_y - camYPos);

                    field_104_parts[i].field_18_animation.VRender(xpos, ypos, ppOt, 0, 0);

                    PSX_RECT frameRect = {};
                    field_104_parts[i].field_18_animation.Get_Frame_Rect(&frameRect);
                    pScreenManager->InvalidateRectCurrentIdx(
                        frameRect.x,
                        frameRect.y,
                        frameRect.w,
                        frameRect.h);
                }
            }
        }
    }
}
