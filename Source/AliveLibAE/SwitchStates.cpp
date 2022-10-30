#include "stdafx.h"
#include "SwitchStates.hpp"
#include "Function.hpp"

SwitchStates sSwitchStates_5C1A28 = {};

void SwitchStates_SetRange(u16 start, u16 end)
{
    if (start <= end)
    {
        memset(&sSwitchStates_5C1A28.mData[start], 0, end - start + 1);
    }
}

void SwitchStates_Set(u16 idx, s8 value)
{
    sSwitchStates_5C1A28.mData[idx] = value;
}

s32 SwitchStates_Get(u16 idx)
{
    if (idx >= ALIVE_COUNTOF(sSwitchStates_5C1A28.mData))
    {
        LOG_WARNING("switch id value is %d and exceeds 255. if you see this in a custom level consider lowering the switch id value.", idx);
        return 0;
    }

    if (idx == 0)
    {
        return 0;
    }

    if (idx == 1)
    {
        return 1;
    }

    return sSwitchStates_5C1A28.mData[idx];
}

void SwitchStates_Add(u16 idx, s8 value)
{
    sSwitchStates_5C1A28.mData[idx] += value;
}

void SwitchStates_Do_Operation(s16 idx, relive::reliveSwitchOp operation)
{
    if (idx >= 2)
    {
        switch (operation)
        {
            case relive::reliveSwitchOp::eSetTrue:
                SwitchStates_Set(idx, 1);
                break;

            case relive::reliveSwitchOp::eSetFalse:
                SwitchStates_Set(idx, 0);
                break;

            case relive::reliveSwitchOp::eToggle:
                if (SwitchStates_Get(idx))
                {
                    // Its on, so turn off
                    SwitchStates_Set(idx, 0);
                }
                else
                {
                    // Its off, so turn on
                    SwitchStates_Set(idx, 1);
                }
                break;

            case relive::reliveSwitchOp::eIncrement:
                SwitchStates_Add(idx, 1);
                break;

            case relive::reliveSwitchOp::eDecrement:
                SwitchStates_Add(idx, -1);
                break;
        }
    }
}
