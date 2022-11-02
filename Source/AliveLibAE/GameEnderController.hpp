#pragma once

#include "../relive_lib/BaseGameObject.hpp"
#include "../AliveLibCommon/Function.hpp"

void CreateGameEnderController_43B7A0();

enum class GameEnderControllerStates : s16
{
    eInit_0 = 0,
    eDetermineEnding_1 = 1,
    eFinish_2 = 2,
    eBadEnding_3 = 3,
    eGoodEnding_4 = 4,
    eAngelicEnding_5 = 5,
    eAngelicEndingCredits_6 = 6,
    ePadding_7 = 7,
    ePadding_8 = 8,
    ePadding_9 = 9,
};

struct GameEnderControllerSaveState final
{
    ReliveTypes field_0_type;
    s16 field_2_padding;
    Guid field_4_obj_id;
    s32 field_8_timer;
    GameEnderControllerStates field_C_state;
    s16 field_E_padding;
};

class GameEnderController final : public BaseGameObject
{
public:
    static s32 CreateFromSaveState(const u8* pBuffer);
    GameEnderController();
    
    virtual void VScreenChanged() override;
    virtual void VUpdate() override;
    virtual s32 VGetSaveState(u8* pSaveBuffer) override;

private:
    s32 field_20_timer = 0;
    GameEnderControllerStates field_24_state = GameEnderControllerStates::eInit_0;
};

extern s16 sFeeco_Restart_KilledMudCount_5C1BC6;
extern s16 sFeecoRestart_SavedMudCount_5C1BC8;
