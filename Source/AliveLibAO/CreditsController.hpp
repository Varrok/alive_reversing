#pragma once

#include "../AliveLibCommon/Function.hpp"
#include "../relive_lib/BaseGameObject.hpp"
#include "Map.hpp"

namespace relive
{
    struct Path_CreditsController;
}

namespace AO {

class CreditsController final : public ::BaseGameObject
{
public:
    
    virtual void VUpdate() override;

    CreditsController(relive::Path_CreditsController* pTlv, const Guid& tlvId);
    ~CreditsController();

    s32 mNextCameraTimer = 0;
    s16 mCurrentCamera = 0;
};
ALIVE_ASSERT_SIZEOF(CreditsController, 0x18);

ALIVE_VAR_EXTERN(s16, gCreditsControllerExists);

} // namespace AO
