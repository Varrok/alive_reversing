#include "stdafx.h"
#include "AnimationBase.hpp"
#include "Function.hpp"
#include "stdlib.hpp"
#include "Sys_common.hpp"

void AnimationBase::VDecode()
{
    ALIVE_FATAL("Should never be called");
}

void AnimationBase::VRender(s32 /*xpos*/, s32 /*ypos*/, PrimHeader** /*pOt*/, s16 /*width*/, s32 /*height*/)
{
    ALIVE_FATAL("Should never be called");
    //    return 0;
}

s8 AnimationBase::Animation_v_40BEE0(s16 /*a2*/, s16 /*a3*/, s32 /*a4*/, s16 /*a5*/, s16 /*op1*/)
{
    
    LOG_INFO("Animation_v_40BEE0");
    return 0;
}

void AnimationBase::AnimateAll(DynamicArrayT<AnimationBase>* pAnims)
{
    for (auto i = 0; i < pAnims->Size(); i++)
    {
        AnimationBase* pAnim = pAnims->ItemAt(i);
        if (!pAnim)
        {
            break;
        }

        if (!pAnim->field_4_flags.Get(AnimFlags::eBit2_Animate))
        {
            continue;
        }

        if (pAnim->field_E_frame_change_counter > 0)
        {
            pAnim->field_E_frame_change_counter--;
            if (pAnim->field_E_frame_change_counter == 0)
            {
                pAnim->VDecode();
            }
        }
    }
}
