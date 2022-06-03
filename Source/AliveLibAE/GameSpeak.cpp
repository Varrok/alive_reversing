#include "stdafx.h"
#include "GameSpeak.hpp"
#include "stdlib.hpp"
#include "Function.hpp"

const s32 code_base_560F0C[12] = {
    0,
    1,
    10,
    100,
    1000,
    10000,
    100000,
    1000000,
    10000000,
    100000000,
    1000000000,
    0,
};

s16 Code_Length(u32 code)
{
    s16 i = 0;
    for (i = 0; code; ++i)
    {
        code /= 10u;
    }
    return i;
}

s32 Code_Convert(u16 code1, u16 code2)
{
    if (code2)
    {
        return code2 + code1 * code_base_560F0C[Code_Length(code2) + 1];
    }
    else
    {
        return code1;
    }
}


GameSpeakEvents Code_LookUp(u32 code, u16 idx, u16 code_len)
{
    if (!code)
    {
        return GameSpeakEvents::eUnknown_0;
    }

    u16 code_len_to_use = code_len;
    if (code_len == 0)
    {
        code_len_to_use = Code_Length(code);
    }

    return static_cast<GameSpeakEvents>(code / code_base_560F0C[code_len_to_use - idx] % 10);
}

ALIVE_VAR(1, 0x5BC11C, GameSpeak*, pEventSystem_5BC11C, nullptr);

GameSpeak::GameSpeak()
    : BaseGameObject(TRUE, 0)
{
    mFlags.Set(BaseGameObject::eSurviveDeathReset_Bit9); // Dont destroy on loading save
    field_2C_event_buffer[0] = -1;
    field_20_last_event = GameSpeakEvents::eNone_m1;
    SetType(AETypes::eGameSpeak_38);
    field_28_last_event_index = 0;
}

GameSpeakMatch GameSpeak::MatchBuffer(u8* pBuffer, s16 max_idx, s16 src_idx)
{
    if (src_idx == -1)
    {
        src_idx = static_cast<s16>(field_28_last_event_index - max_idx);
        if (src_idx < 0)
        {
            src_idx += ALIVE_COUNTOF(field_2C_event_buffer);
        }
    }

    s16 dst_idx = 0;
    while (1)
    {
        if (field_2C_event_buffer[src_idx] == static_cast<s8>(GameSpeakEvents::eNone_m1))
        {
            bool bContinue = true;
            while (src_idx != field_28_last_event_index)
            {
                src_idx++;
                if (src_idx == ALIVE_COUNTOF(field_2C_event_buffer))
                {
                    src_idx = 0;
                }

                if (field_2C_event_buffer[src_idx] != -1)
                {
                    bContinue = false;
                    break;
                }
            }

            if (bContinue)
            {
                return GameSpeakMatch::ePartMatch_2;
            }
        }

        if (pBuffer[dst_idx] != field_2C_event_buffer[src_idx])
        {
            return GameSpeakMatch::eNoMatch_0;
        }

        if (dst_idx == max_idx - 1)
        {
            return GameSpeakMatch::eFullMatch_1;
        }

        if (src_idx == field_28_last_event_index)
        {
            return GameSpeakMatch::ePartMatch_2;
        }

        src_idx++;
        if (src_idx == ALIVE_COUNTOF(field_2C_event_buffer))
        {
            src_idx = 0;
        }
        dst_idx++;
    }
}

s32 GameSpeak::FillBuffer(s32 code, u8* pBufffer)
{
    const s16 len = Code_Length(code);
    for (s16 idx = 0; idx < len; idx++)
    {
        pBufffer[idx] = static_cast<u8>(Code_LookUp(code, idx, len));
    }
    return len;
}

GameSpeak::~GameSpeak()
{
    pEventSystem_5BC11C = nullptr;
}

void GameSpeak::VRender(PrimHeader** /*ppOt*/)
{
    // Null @ 0x4DBF80
}


void GameSpeak::VScreenChanged()
{
    // Null @ 0x421AB0
}

void GameSpeak::VUpdate()
{
    if (field_20_last_event != GameSpeakEvents::eNone_m1 && sGnFrame_5C1B84 > field_24_last_event_frame)
    {
        PushEvent_Impl(GameSpeakEvents::eNone_m1);
    }
}

void GameSpeak::PushEvent(GameSpeakEvents event)
{
    PushEvent_Impl(event);
    field_24_last_event_frame = sGnFrame_5C1B84 + 60;
}

void GameSpeak::PushEvent_Impl(GameSpeakEvents event)
{
    field_28_last_event_index++;

    // Wrap around
    if (field_28_last_event_index >= ALIVE_COUNTOF(field_2C_event_buffer))
    {
        field_28_last_event_index = 0;
    }

    // TODO: This isn't ever used ??
    field_2C_event_buffer[field_28_last_event_index] = static_cast<s8>(event);
    field_20_last_event = event;
}
