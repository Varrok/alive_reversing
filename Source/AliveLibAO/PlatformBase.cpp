#include "stdafx_ao.h"
#include "Function.hpp"
#include "PlatformBase.hpp"
#include "Map.hpp"
#include "Collisions.hpp"
#include "Game.hpp"
#include "stdlib.hpp"

namespace AO {

void PlatformBase::VRemove(BaseAliveGameObject* pObj)
{
    field_114_count--;
    LOG_INFO("Remove " << (u64) pObj << " count " << field_114_count << " gnFrame " << gnFrameCount_507670);
}

void PlatformBase::AddDynamicCollision(s32 frameTableOffset, s32 maxW, s32 maxH, u8** ppAnimData, Path_TLV* pTlv, Map* pMap, s32 tlvInfo)
{
    field_A8_xpos = FP_FromInteger(pTlv->field_10_top_left.field_0_x);
    field_AC_ypos = FP_FromInteger(pTlv->field_10_top_left.field_2_y);

    field_128_tlvInfo = tlvInfo;

    field_124_pMap = pMap;

    field_B4_velx = FP_FromInteger(0);
    field_B8_vely = FP_FromInteger(0);

    field_10C = 0;
    field_C8_yOffset = 0;

    field_110 = 0;
    field_114_count = 0;

    Animation_Init_417FD0(
        frameTableOffset,
        maxW,
        maxH,
        ppAnimData,
        1);

    if (field_BC_sprite_scale == FP_FromInteger(1))
    {
        field_10_anim.field_C_layer = Layer::eLayer_BeforeShadow_25;
        field_C6_scale = 1;
    }
    else
    {
        field_10_anim.field_C_layer = Layer::eLayer_BeforeShadow_Half_6;
        field_C6_scale = 0;
    }

    FrameInfoHeader* pFrameHeader = field_10_anim.Get_FrameHeader(0);
    field_AC_ypos += FP_NoFractional(FP_FromInteger(-pFrameHeader->field_8_data.points[1].y) * field_BC_sprite_scale);
    field_A8_xpos = FP_FromInteger((pTlv->field_10_top_left.field_0_x + pTlv->field_14_bottom_right.field_0_x) / 2);

    field_120_pCollisionLine = sCollisions_DArray_504C6C->Add_Dynamic_Collision_Line(
        pTlv->field_10_top_left.field_0_x,
        pTlv->field_10_top_left.field_2_y,
        pTlv->field_14_bottom_right.field_0_x,
        pTlv->field_10_top_left.field_2_y,
        eLineTypes::eUnknown_32
    );

    field_118_x_offset = FP_GetExponent(FP_FromInteger(pTlv->field_10_top_left.field_0_x) - field_A8_xpos);
    field_11A_width_offset = FP_GetExponent(FP_FromInteger(pTlv->field_14_bottom_right.field_0_x) - field_A8_xpos);
    field_11C_y_offset = FP_GetExponent(FP_FromInteger(pTlv->field_10_top_left.field_2_y) - field_AC_ypos);
    field_11E_height_offset = FP_GetExponent(FP_FromInteger(pTlv->field_10_top_left.field_2_y) - field_AC_ypos);

    if (!ObjListPlatforms_50766C->Push_Back(this))
    {
        mFlags.Set(Options::eListAddFailed_Bit1);
    }
}

PlatformBase::~PlatformBase()
{
    ObjListPlatforms_50766C->Remove_Item(this);

    if (field_120_pCollisionLine)
    {
        if (gMap.mCurrentLevel == field_B2_lvl_number && gMap.mCurrentPath == field_B0_path_number)
        {
            Rect_Clear(&field_120_pCollisionLine->field_0_rect);
        }
    }
}

void PlatformBase::SyncCollisionLinePosition()
{
    field_120_pCollisionLine->field_0_rect.x = FP_GetExponent(field_A8_xpos + FP_FromInteger(field_118_x_offset));
    field_120_pCollisionLine->field_0_rect.w = FP_GetExponent(field_A8_xpos + FP_FromInteger(field_11A_width_offset));
    field_120_pCollisionLine->field_0_rect.y = FP_GetExponent(field_AC_ypos + FP_FromInteger(field_11C_y_offset));
    field_120_pCollisionLine->field_0_rect.h = FP_GetExponent(field_AC_ypos + FP_FromInteger(field_11E_height_offset));
}

void PlatformBase::KeepThingsOnPlatform(FP xpos)
{
    for (s32 i = 0; i < gBaseAliveGameObjects_4FC8A0->Size(); i++)
    {
        BaseAliveGameObject* pObjIter = gBaseAliveGameObjects_4FC8A0->ItemAt(i);
        if (!pObjIter)
        {
            break;
        }

        if (pObjIter->field_F8_pLiftPoint == this)
        {
            pObjIter->field_A8_xpos += xpos;
            pObjIter->field_AC_ypos = FP_FromInteger(field_120_pCollisionLine->field_0_rect.y);
        }
    }
}

void PlatformBase::VAdd(BaseAliveGameObject* pObj)
{
    field_114_count++;
    LOG_INFO("Add " << (u64) pObj << " count " << field_114_count << " gnFrame " << gnFrameCount_507670);
    field_110 = 1;

    if (field_120_pCollisionLine)
    {
        field_120_pCollisionLine->field_0_rect.x = FP_GetExponent(field_A8_xpos + FP_FromInteger(field_118_x_offset));
        field_120_pCollisionLine->field_0_rect.w = FP_GetExponent(field_A8_xpos + FP_FromInteger(field_11A_width_offset));
        field_120_pCollisionLine->field_0_rect.y = FP_GetExponent(field_AC_ypos + FP_FromInteger(field_11C_y_offset));
        field_120_pCollisionLine->field_0_rect.h = FP_GetExponent(field_AC_ypos + FP_FromInteger(field_11E_height_offset));
    }
}

} // namespace AO
