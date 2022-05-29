#include "stdafx_ao.h"
#include "Function.hpp"
#include "BulletShell.hpp"
#include "Math.hpp"
#include "ResourceManager.hpp"
#include "stdlib.hpp"
#include "Map.hpp"
#include "Collisions.hpp"
#include "Sfx.hpp"

namespace AO {

BulletShell::BulletShell(FP xpos, FP ypos, s32 direction, FP scale)
{
    field_4_typeId = Types::eNone_0;

    const AnimRecord& rec = AO::AnimRec(AnimId::Bullet_Shell);
    u8** ppRes = ResourceManager::GetLoadedResource_4554F0(ResourceManager::Resource_Animation, rec.mResourceId, 1, 0);
    Animation_Init_417FD0(rec.mFrameTableOffset, rec.mMaxW, rec.mMaxH, ppRes, 1);

    field_BC_sprite_scale = scale;

    if (scale == FP_FromInteger(1))
    {
        field_10_anim.field_C_layer = Layer::eLayer_Foreground_36;
    }
    else
    {
        field_10_anim.field_C_layer = Layer::eLayer_Foreground_Half_17;
    }

    field_CC_bApplyShadows &= ~1u;
    field_10_anim.field_4_flags.Set(AnimFlags::eBit5_FlipX, direction & 1);

    field_EC_hitCount = 0;

    field_A8_xpos = xpos;
    field_AC_ypos = ypos;

    if (direction)
    {
        field_B4_velx = FP_FromInteger(Math_RandomRange_450F20(-6, -3));
    }
    else
    {
        field_B4_velx = FP_FromInteger(Math_RandomRange_450F20(3, 6));
    }
    field_B8_vely = FP_FromInteger(Math_RandomRange_450F20(-4, -1));
    field_F0_speed = FP_FromInteger(1);
}

void BulletShell::VUpdate()
{
    VUpdate_4628C0();
}

void BulletShell::VUpdate_4628C0()
{
    field_A8_xpos += field_B4_velx;
    field_AC_ypos += field_B8_vely;

    field_B8_vely += field_F0_speed;

    FP hitX = {};
    FP hitY = {};
    if (sCollisions_DArray_504C6C->RayCast_40C410(
            field_A8_xpos,
            field_AC_ypos - field_B8_vely,
            field_A8_xpos,
            field_AC_ypos,
            &field_E4_pLine,
            &hitX,
            &hitY,
            field_BC_sprite_scale != FP_FromDouble(0.5) ? 7 : 0x70)
        == 1)
    {
        if (field_E4_pLine->field_8_type == eLineTypes ::eFloor_0 ||
            field_E4_pLine->field_8_type == eLineTypes::eBackgroundFloor_4)
        {
            field_AC_ypos = hitY - FP_FromInteger(1);
            field_B8_vely = -(field_B8_vely * FP_FromDouble(0.3));
            field_B4_velx = (field_B4_velx * FP_FromDouble(0.3));

            if (field_B4_velx < FP_FromInteger(0) && field_B4_velx > FP_FromInteger(-1))
            {
                field_B4_velx = FP_FromInteger(-1);
            }
            else if (field_B4_velx > FP_FromInteger(0) && field_B4_velx < FP_FromInteger(1))
            {
                field_B4_velx = FP_FromInteger(1);
            }

            s16 volume = 19 * (3 - field_EC_hitCount);
            if (volume <= 19)
            {
                volume = 19;
            }
            SFX_Play_43AD70(SoundEffect::BulletShell_7, volume, 0);
            field_EC_hitCount++;
        }
    }

    if (!gMap.Is_Point_In_Current_Camera_4449C0(
            field_B2_lvl_number,
            field_B0_path_number,
            field_A8_xpos,
            field_AC_ypos,
            0))
    {
        mFlags.Set(BaseGameObject::eDead);
    }

    if (field_EC_hitCount >= 3)
    {
        mFlags.Set(BaseGameObject::eDead);
    }
}

} // namespace AO
