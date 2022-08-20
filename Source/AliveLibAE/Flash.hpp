#pragma once

#include "../AliveLibCommon/Function.hpp"
#include "EffectBase.hpp"
#include "../relive_lib/Layer.hpp"

class Flash final : public EffectBase
{
public:
    Flash(Layer layer, s32 r, s32 g, s32 b, TPageAbr abr, s32 time);

    virtual void VUpdate() override;
private:
    s32 mFlashTime = 0;
};
ALIVE_ASSERT_SIZEOF(Flash, 0x7C);
