#include "stdafx_ao.h"
#include "VGA.hpp"
#include "Function.hpp"
#include "../AliveLibAE/VGA.hpp"

namespace AO {

Bitmap sVGA_bmp_primary_A8A4C0 = {};

s32 VGA_FullScreenSet_490160(bool bFullScreen)
{
    return VGA_FullScreenSet_4F31F0(bFullScreen);
}

s32 VGA_DisplaySet_490230(u16 width, u16 height, s8 bpp, u8 backbufferCount, TSurfaceType** ppSurface)
{
    return VGA_DisplaySet_4F32C0(width, height, bpp, backbufferCount, ppSurface);
}

void VGA_Shutdown_4900E0()
{
    VGA_Shutdown_4F3170();
}

} // namespace AO
