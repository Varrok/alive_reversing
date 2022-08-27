#pragma once

#include "BaseAnimatedWithPhysicsGameObject.hpp"
#include "../AliveLibCommon/Function.hpp"
#include "Path.hpp"

struct Path_ParamiteWebLine final : public Path_TLV
{
    Scale_short mScale;
    s16 field_12_pad;
};
ALIVE_ASSERT_SIZEOF_ALWAYS(Path_ParamiteWebLine, 0x14);

class ParamiteWebLine final : public ::BaseAnimatedWithPhysicsGameObject
{
public:
    ParamiteWebLine(relive::Path_ParamiteWebLine* pTlv, s32 tlvInfo);
    ~ParamiteWebLine();

    void Wobble(s16 ypos);

    virtual void VUpdate() override;
    virtual void VScreenChanged() override;
    virtual PSX_RECT VGetBoundingRect(s32 pointIdx) override;
    virtual void VRender(PrimHeader** ppOt) override;

private:
    s16 field_F4_anim_segment_count = 0;
    s16 field_F6_piece_length = 0;
    s16 field_F8_top = 0;
    s16 field_FA_bottom = 0;
    AnimationUnknown* field_FC_pRes = nullptr;
    s32 field_100_tlv_info = 0;
    s16 field_104_wobble_idx = 0;
    s16 field_106_wobble_pos = 0;
    Animation field_108_anim_flare = {};
    s16 field_1A0_pulse_position = 0;
    s16 field_1A2_pulse_position_speed = 0;
    s16 field_1A4_delay_counter = 0;
};
ALIVE_ASSERT_SIZEOF(ParamiteWebLine, 0x1AC);
