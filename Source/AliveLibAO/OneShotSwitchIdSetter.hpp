#pragma once

#include "../AliveLibCommon/FunctionFwd.hpp"
#include "Map.hpp"
#include "BaseGameObject.hpp"

namespace AO {

struct Path_OneShotSwitchIdSetter final : public Path_TLV
{
    s16 field_18_ids_to_clear[8];
};
ALIVE_ASSERT_SIZEOF(Path_OneShotSwitchIdSetter, 0x28);

class OneShotSwitchIdSetter final : public BaseGameObject
{
public:
    OneShotSwitchIdSetter(Path_OneShotSwitchIdSetter* pTlv, s32 tlvInfo);
    ~OneShotSwitchIdSetter();

    virtual void VScreenChanged() override;

    EXPORT void VScreenChanged_432F00();

    s32 field_10_tlvInfo;
};
ALIVE_ASSERT_SIZEOF(OneShotSwitchIdSetter, 0x14);

} // namespace AO
