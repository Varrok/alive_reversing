#pragma once

#include "../relive_lib/BaseAnimatedWithPhysicsGameObject.hpp"
#include "../AliveLibCommon/Function.hpp"
#include "../relive_lib/data_conversion/relive_tlvs.hpp"

enum class WheelStates : s16
{
    eIdle_0 = 0,
    eTurning_1 = 1,
};

struct WorkWheelSaveState final
{
    AETypes mType;
    s16 padding_1;
    Guid field_4_tlvInfo;
    s16 field_8_snd_counter;
    s16 padding_2;
    WheelStates field_C_state;
    s16 padding_3;
};
//ALIVE_ASSERT_SIZEOF_ALWAYS(WorkWheelSaveState, 0x10);

class WorkWheel final : public ::BaseAnimatedWithPhysicsGameObject
{
public:
    void LoadAnimations();

    virtual void VUpdate() override;
    virtual void VScreenChanged() override;
    virtual s32 VGetSaveState(u8* pSaveBuffer) override;
    virtual void VStartTurning();
    virtual void VStopTurning(s16 bResetSwitch);

    WorkWheel(relive::Path_WorkWheel* pTlv, const Guid& tlvId);
    ~WorkWheel();

public:
    static s32 CreateFromSaveState(const u8* pState);

private:
    Guid mTlvInfo;
    s16 mSwitchId = 0;
    WheelStates mState = WheelStates::eIdle_0;
    s16 mActivationTime = 0;
    u16 mTurningTime = 0;
    s16 mTurnOffTime = 0;
    relive::reliveChoice mTurnOffWhenStopped = relive::reliveChoice::eNo;
};
