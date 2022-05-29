#pragma once

#include "../AliveLibCommon/FunctionFwd.hpp"
#include "BaseAnimatedWithPhysicsGameObject.hpp"

class Sparks final : public ::BaseAnimatedWithPhysicsGameObject
{
public:
    Sparks(FP xpos, FP ypos, FP scale);
    
    virtual void VUpdate() override;
    virtual void VScreenChanged() override;

private:
    EXPORT void vUpdate_416570();
    EXPORT void vScreenChanged_416720();

private:
    s32 field_F4_padding;
    s16 field_F8_padding;
    s16 field_FA_16_random;
};
ALIVE_ASSERT_SIZEOF(Sparks, 0xFC);
