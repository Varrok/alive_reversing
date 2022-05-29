#include "stdafx_ao.h"
#include "Explosion.hpp"
#include "Function.hpp"
#include "Map.hpp"
#include "Particle.hpp"
#include "Flash.hpp"
#include "ScreenShake.hpp"
#include "Gibs.hpp"
#include "Events.hpp"
#include "ParticleBurst.hpp"
#include "Midi.hpp"
#include "stdlib.hpp"
#include "AmbientSound.hpp"
#include "Slig.hpp"
#include "ResourceManager.hpp"
#undef min
#undef max

namespace AO {

Explosion::Explosion(FP xpos, FP ypos, FP exposion_size)
{
    field_4_typeId = Types::eExplosion_74;
    const AnimRecord& rec = AO::AnimRec(AnimId::Explosion);
    u8** ppRes = ResourceManager::GetLoadedResource_4554F0(ResourceManager::Resource_Animation, rec.mResourceId, 1, 0);
    Animation_Init_417FD0(rec.mFrameTableOffset, rec.mMaxW, rec.mMaxH, ppRes, 1);

    field_10_anim.field_4_flags.Clear(AnimFlags::eBit18_IsLastFrame);
    field_10_anim.field_B_render_mode = TPageAbr::eBlend_1;
    field_E4_explosion_size = exposion_size;

    field_BC_sprite_scale = exposion_size * FP_FromInteger(2);
    field_CC_bApplyShadows &= ~1u;
    field_AC_ypos = ypos;
    field_A8_xpos = xpos;

    ao_new<ScreenShake>(TRUE);

    PSX_RECT rect = {};
    rect.x = FP_GetExponent(FP_FromInteger(-10) * exposion_size);
    rect.y = FP_GetExponent(FP_FromInteger(-10) * exposion_size);
    rect.w = FP_GetExponent(FP_FromInteger(10) * exposion_size);
    rect.h = FP_GetExponent(FP_FromInteger(10) * exposion_size);

    DealBlastDamage_459160(&rect);

    SND_SEQ_PlaySeq_4775A0(SeqId::eExplosion1_21, 1, 1);
}

void Explosion::VUpdate()
{
    VUpdate_458D00();
}

void Explosion::VUpdate_458D00()
{
    Event_Broadcast_417220(kEvent_2, this);
    Event_Broadcast_417220(kEvent_14, this);
    Event_Broadcast_417220(kEventSuspiciousNoise_10, this);

    PSX_RECT rect = {};

    switch (field_10_anim.field_92_current_frame)
    {
        case 2:
            rect.x = FP_GetExponent(FP_FromInteger(-20) * field_E4_explosion_size);
            rect.w = FP_GetExponent(FP_FromInteger(20) * field_E4_explosion_size);
            rect.y = FP_GetExponent(FP_FromInteger(-20) * field_E4_explosion_size);
            rect.h = FP_GetExponent(FP_FromInteger(10) * field_E4_explosion_size);
            DealBlastDamage_459160(&rect);
            break;

        case 3:
        {
            ao_new<ParticleBurst>(field_A8_xpos, field_AC_ypos, 20, field_BC_sprite_scale, BurstType::eBigRedSparks_3);

            ao_new<Flash>(Layer::eLayer_Above_FG1_39, 255u, 255u, 255u);
            break;
        }

        case 4:
        {
            ao_new<Flash>(Layer::eLayer_Above_FG1_39, 255u, 255u, 255u, 1, TPageAbr::eBlend_1, 1);

            rect.x = FP_GetExponent(FP_FromInteger(-38) * field_E4_explosion_size);
            rect.w = FP_GetExponent(FP_FromInteger(38) * field_E4_explosion_size);
            rect.y = FP_GetExponent(FP_FromInteger(-38) * field_E4_explosion_size);
            rect.h = FP_GetExponent(FP_FromInteger(19) * field_E4_explosion_size);
            DealBlastDamage_459160(&rect);
            break;
        }

        case 6:
            rect.x = FP_GetExponent(FP_FromInteger(-60) * field_E4_explosion_size);
            rect.w = FP_GetExponent(FP_FromInteger(60) * field_E4_explosion_size);
            rect.y = FP_GetExponent(FP_FromInteger(-60) * field_E4_explosion_size);
            rect.h = FP_GetExponent(FP_FromInteger(30) * field_E4_explosion_size);
            DealBlastDamage_459160(&rect);
            break;

        case 8:
        {
            ao_new<ParticleBurst>(field_A8_xpos, field_AC_ypos, 20, field_BC_sprite_scale, BurstType::eBigRedSparks_3);

            ao_new<Flash>(Layer::eLayer_Above_FG1_39, 255u, 255u, 255u);
            break;
        }

        default:
            break;
    }

    if (field_10_anim.field_92_current_frame > 9)
    {
        field_BC_sprite_scale -= FP_FromDouble(0.2);
    }

    if (field_10_anim.field_92_current_frame == 1)
    {
        const AnimRecord& rec = AO::AnimRec(AnimId::Explosion);
        const auto ppRes = ResourceManager::GetLoadedResource_4554F0(ResourceManager::Resource_Animation, rec.mResourceId, 1, 0);
        if (ppRes)
        {
            auto pParticle = ao_new<Particle>(field_A8_xpos, field_AC_ypos, rec.mFrameTableOffset, rec.mMaxW, rec.mMaxH, ppRes);
            if (pParticle)
            {
                if (pParticle->mFlags.Get(BaseGameObject::eListAddFailed_Bit1))
                {
                    pParticle->mFlags.Set(Options::eDead);
                }

                pParticle->field_CC_bApplyShadows &= ~1u;
                pParticle->field_10_anim.field_4_flags.Clear(AnimFlags::eBit5_FlipX);
                pParticle->field_10_anim.field_B_render_mode = TPageAbr::eBlend_1;
                pParticle->field_BC_sprite_scale = field_BC_sprite_scale * FP_FromDouble(0.25);
            }
            else
            {
                pParticle = nullptr;
            }
        }
    }

    if (field_10_anim.field_4_flags.Get(AnimFlags::eBit12_ForwardLoopCompleted))
    {
        mFlags.Set(BaseGameObject::eDead);
    }
}

void Explosion::VScreenChanged()
{
    VScreenChanged_459470();
}

void Explosion::VScreenChanged_459470()
{
    if (gMap.mOverlayId != gMap.GetOverlayId())
    {
        mFlags.Set(BaseGameObject::eDead);
    }
}

void Explosion::DealBlastDamage_459160(PSX_RECT* pRect)
{
    if (!gBaseAliveGameObjects_4FC8A0)
    {
        return;
    }

    PSX_RECT expandedRect = {};
    expandedRect.x = std::min(pRect->x, pRect->w);
    expandedRect.w = std::max(pRect->x, pRect->w);

    expandedRect.y = std::min(pRect->y, pRect->h);
    expandedRect.h = std::max(pRect->y, pRect->h);

    expandedRect.x += FP_GetExponent(field_A8_xpos);
    expandedRect.y += FP_GetExponent(field_AC_ypos);

    expandedRect.w += FP_GetExponent(field_A8_xpos);
    expandedRect.h += FP_GetExponent(field_AC_ypos);

    if ((expandedRect.x % 1024) < 256)
    {
        expandedRect.x -= 656;
    }
    if ((expandedRect.w % 1024) > 624)
    {
        expandedRect.w += 656;
    }
    if (expandedRect.y % 480 < 120)
    {
        expandedRect.y -= 240;
    }
    if (expandedRect.h % 480 > 360)
    {
        expandedRect.h += 240;
    }

    for (s32 idx = 0; idx < gBaseAliveGameObjects_4FC8A0->Size(); idx++)
    {
        auto pObj = gBaseAliveGameObjects_4FC8A0->ItemAt(idx);
        if (!pObj)
        {
            break;
        }

        if (pObj->mFlags.Get(Options::eIsBaseAliveGameObject_Bit6))
        {
            PSX_RECT rect = {};
            pObj->VGetBoundingRect(&rect, 1);

            if (PSX_Rects_overlap_no_adjustment(&rect, &expandedRect) && field_E4_explosion_size == pObj->field_BC_sprite_scale)
            {
                pObj->VTakeDamage(this);
            }
        }
    }

    auto pTlv = static_cast<Path_Slig*>(gMap.TLV_Get_At_446260(
        expandedRect.x,
        expandedRect.y,
        expandedRect.w,
        expandedRect.h,
        TlvTypes::Slig_24));

    if (pTlv)
    {
        if (!pTlv->field_0_flags.Get(TLV_Flags::eBit2_Destroyed) && pTlv->field_1A_start_state == Path_Slig::StartState::Sleeping_2)
        {
            pTlv->field_0_flags.Set(TLV_Flags::eBit2_Destroyed);
            const CameraPos dir = gMap.GetDirection_444A40(
                static_cast<s32>(gMap.mCurrentLevel),
                gMap.mCurrentPath,
                FP_FromInteger(pTlv->field_10_top_left.field_0_x),
                FP_FromInteger(pTlv->field_10_top_left.field_2_y));

            if (dir == CameraPos::eCamLeft_3)
            {
                ao_new<Gibs>(GibType::Slig_1, field_A8_xpos + FP_FromInteger(656), field_AC_ypos, FP_FromInteger(0), FP_FromInteger(0), FP_FromInteger(1));
            }
            else if (dir == CameraPos::eCamRight_4)
            {
                ao_new<Gibs>(GibType::Slig_1, field_A8_xpos - FP_FromInteger(656), field_AC_ypos, FP_FromInteger(0), FP_FromInteger(0), FP_FromInteger(1));
            }
            Stop_slig_sounds_476A20(dir, 0);
        }
    }
}

} // namespace AO
