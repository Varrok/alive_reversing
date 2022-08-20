#pragma once

#include "../AliveLibCommon/Function.hpp"
#include "../relive_lib/BaseGameObject.hpp"
#include "FixedPoint.hpp"

enum class EReliveLevelIds : s16;

namespace AO {

class BaseAliveGameObject;

enum class BulletType : s16
{
    ePossessedSlig_0 = 0,
    eNormalBullet_1 = 1,
    eZBullet_2 = 2
};

class Bullet final : public BaseGameObject
{
public:
    Bullet(BaseAliveGameObject* pParent, BulletType type, FP xpos, FP ypos, FP xDist, s32 unused, FP scale, s32 numberOfBullets);

    virtual void VUpdate() override;

    BaseAliveGameObject* ShootObject(PSX_RECT* pRect);
    static bool InZBulletCover(FP xpos, FP ypos, const PSX_RECT& objRect);

private:
    void PlayBulletSounds(s16 volume);

public:
    BulletType field_10_type = BulletType::ePossessedSlig_0;
    PathLine* mLine = nullptr;
    FP mXPos = {};
    FP mYPos = {};
    FP field_20_x_distance = {};
    EReliveLevelIds mCurrentLevel = EReliveLevelIds::eNone;
    s16 mCurrentPath = 0;
    FP mScale = {};
    BaseAliveGameObject* field_30_pParent = nullptr;
    s16 field_34_number_of_bullets = 0;
    s32 field_24_unused = 0;
};
ALIVE_ASSERT_SIZEOF(Bullet, 0x38);

} // namespace AO
