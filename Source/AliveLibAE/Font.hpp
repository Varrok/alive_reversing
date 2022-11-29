#pragma once

#include "stdlib.hpp"
#include "Math.hpp"
#include "../relive_lib/Primitives.hpp"
#include "../relive_lib/ResourceManagerWrapper.hpp"

struct File_Font final
{
    s16 mWidth;
    s16 mHeight;
    s16 field_4_color_depth;
    s16 field_6_palette_size;
    u16 field_8_palette[32/2];
    u8 field_28_pixel_buffer[1];
};
//ALIVE_ASSERT_SIZEOF(File_Font, 0x28);
// Can vary in size

struct Font_AtlasEntry final
{
    u8 x;
    u8 mY;
    u8 mWidth;
    u8 mHeight;
    std::string mCharName;
};

class FontContext final
{
public:
    const Font_AtlasEntry* field_8_atlas_array = nullptr;
    FontResource field_C_resource_id;

    void LoadFontType(FontType resourceID);

};


class AliveFont final
{
public:
    AliveFont();
    AliveFont(s32 maxCharLength, const PalResource& pal, FontContext* fontContext);
    void Load(s32 maxCharLength, const PalResource& pal, FontContext* fontContext);
    ~AliveFont();
	
    s32 MeasureTextWidth(const char_type* text);
    s32 MeasureCharacterWidth(char_type character);
    s32 MeasureScaledTextWidth(const char_type* text, FP scale);

    s32 DrawString(PrimHeader** ppOt, const char_type* text, s32 x, s16 y, TPageAbr abr, s32 bSemiTrans, s32 blendMode, Layer layer, u8 r, u8 g, u8 b, s32 polyOffset, FP scale, s32 maxRenderWidth, s16 colorRandomRange);

    const char_type* SliceText(const char_type* text, s32 left, FP scale, s32 right);

public:
    Poly_FT4* field_24_fnt_poly_array = nullptr;
    s32 field_30_poly_count = 0;
    FontContext* field_34_FontContext = nullptr;
};

extern s8 sDisableFontFlicker;
extern u8 sFontDrawScreenSpace;
