#pragma once

#include "../relive_lib/BaseGameObject.hpp"
#include "../relive_lib/Primitives.hpp"
#include "../relive_lib/Layer.hpp"

namespace AO {

class ScreenClipper final : public ::BaseGameObject
{
public:
    ScreenClipper(PSX_Point xy, PSX_Point wh, Layer layer);
    ~ScreenClipper();
    
    virtual void VRender(PrimHeader** ppOt) override;
    virtual void VScreenChanged() override;
    virtual void VUpdate() override;

    void Update_Clip_Rect(PSX_Point xy, PSX_Point wh);

private:

    Prim_PrimClipper mClippers[2] = {};
    PSX_RECT mRect = {};

public:
    Layer mOtLayer = Layer::eLayer_0;

};

} // namespace AO
