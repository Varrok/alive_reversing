#pragma once

#include "../AliveLibCommon/FunctionFwd.hpp"

namespace AO {

enum class CameraPos : s16;

class ScopedSeq final
{
public:
    ScopedSeq(s32 ambianceId, CameraPos direction);
    ~ScopedSeq();

    s16 field_4_seq_id;
    s16 field_6_pad;
    s32 field_8_channel_mask;
};
//ALIVE_ASSERT_SIZEOF(ScopedSeq, 0xC);

} // namespace AO
