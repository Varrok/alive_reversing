#pragma once

#include "FunctionFwd.hpp"

struct FixedPoint;
using FP = FixedPoint;

void Math_ForceLink();

EXPORT unsigned int  CC Math_FixedPoint_Multiply_496C50(signed int op1, signed int op2);
EXPORT unsigned int CC Math_FixedPoint_Divide_496B70(signed int op1, signed int op2);
EXPORT short CC Math_RandomRange_496AB0(signed short min, signed short max);
BYTE Math_NextRandom();

EXPORT FP CC Math_Cosine_496CD0(BYTE v);
EXPORT FP CC Math_Sine_496DD0(BYTE v);

namespace Test
{
    void Math_Tests();
}
