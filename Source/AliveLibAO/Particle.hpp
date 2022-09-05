#pragma once

#include "BaseAnimatedWithPhysicsGameObject.hpp"
#include "../AliveLibCommon/Function.hpp"

namespace AO {

class Particle final : public BaseAnimatedWithPhysicsGameObject
{
public:
    Particle(FP xpos, FP ypos, AnimId animId, u8** ppAnimData);

    virtual void VUpdate() override;

    FP field_E4_scale_amount = {};
};
ALIVE_ASSERT_SIZEOF(Particle, 0xE8);

Particle* New_DestroyOrCreateObject_Particle(FP xpos, FP ypos, FP scale);
void New_Smoke_Particles(FP xpos, FP ypos, FP scale, s16 count, s16 type);
void New_Orb_Particle(FP xpos, FP ypos, FP scale, Layer layer);
void New_TintShiny_Particle(FP xpos, FP ypos, FP scale, Layer layer);
void New_ShootingZFire_Particle(FP xpos, FP ypos, FP scale);
void New_ShootingFire_Particle(FP xpos, FP ypos, s8 direction, FP scale);

} // namespace AO
