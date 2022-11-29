#pragma once

#include "BaseAliveGameObject.hpp"
#include "../relive_lib/data_conversion/relive_tlvs.hpp" // reliveChoice only

class MovingBomb final : public BaseAliveGameObject
{
public:
    MovingBomb(relive::Path_MovingBomb* pTlv, const Guid& tlvId);
    ~MovingBomb();
    
    virtual void VUpdate() override;
    virtual void VRender(PrimHeader** ppOt) override;
    virtual void VOnThrowableHit(BaseGameObject* pFrom) override;
    virtual s16 VTakeDamage(BaseGameObject* pFrom) override;
    virtual void VScreenChanged() override;

private:
    void BlowUp();
    void FollowLine();
    s16 HitObject();

private:
    enum class States : s16
    {
        eTriggeredByAlarm_0 = 0,
        eTriggeredBySwitch_1 = 1,
        eMoving_2 = 2,
        eStopMoving_3 = 3,
        eWaitABit_4 = 4,
        eToMoving_5 = 5,
        eBlowingUp_6 = 6,
        eKillMovingBomb_7 = 7
    };
    States field_118_state = States::eTriggeredByAlarm_0;
    Guid field_11C_tlvInfo;
    s32 field_120_timer = 0;
    FP field_124_speed = {};
    s16 field_128_start_moving_switch_id = 0;
    s16 field_12A_min = 0;
    s16 field_12C_max = 0;
    s32 field_130_sound_channels = 0;
    relive::reliveChoice field_136_persist_offscreen = relive::reliveChoice::eNo;
};
