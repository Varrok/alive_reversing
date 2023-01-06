#include "stdafx_ao.h"
#include "Input.hpp"
#include "Game.hpp"
#include "../AliveLibAE/Input.hpp"
#include "../relive_lib/BitField.hpp"
#include "../relive_lib/BaseGameAutoPlayer.hpp"

namespace AO {

InputObject sInputObject = {};
u16 sCurrentControllerIndex = 0;
s32 sJoystickEnabled = 0;
u8 sInputEnabled = 0;
u32 sLastPressedKey = 0;

const InputCommands sInputKey_Right = eRight;
const InputCommands sInputKey_Left = eLeft;
const InputCommands sInputKey_Up = eUp;
const InputCommands sInputKey_Down = eDown;
const InputCommands sInputKey_Hop = eHop;
const InputCommands sInputKey_DoAction = eDoAction;
const InputCommands sInputKey_Run = eRun;
const InputCommands sInputKey_Sneak = eSneak;
const InputCommands sInputKey_FartRoll = eCrouchOrRoll;
const InputCommands sInputKey_ThrowItem = eThrowItem;

void InputObject::InitPad(u32 /*padCount*/)
{
    for (PSX_Pad& pad : mPads)
    {
        pad = {};
    }

    mbDemoPlaying &= ~1u;

    Input_Pads_Reset_4FA960();
    Input_EnableInput_4EDDD0();
}

u8 sPad1Buffer_507778[64] = {};
u8 sPad2Buffer_507738[64] = {};

bool Input_IsGameSpeakPressedDirectly(InputCommands gameSpeakId)
{
    return Input().IsAnyPressed(gameSpeakId);
}

std::optional<InputCommands> Input_IsAnyGameSpeakPressed()
{
    if (Input_IsGameSpeakPressed(InputCommands::eGameSpeak1))
    {
        return InputCommands::eGameSpeak1;
    }
    else if (Input_IsGameSpeakPressed(InputCommands::eGameSpeak2))
    {
        return InputCommands::eGameSpeak2;
    }
    else if (Input_IsGameSpeakPressed(InputCommands::eGameSpeak3))
    {
        return InputCommands::eGameSpeak3;
    }
    else if (Input_IsGameSpeakPressed(InputCommands::eGameSpeak4))
    {
        return InputCommands::eGameSpeak4;
    }
    else if (Input_IsGameSpeakPressed(InputCommands::eGameSpeak5))
    {
        return InputCommands::eGameSpeak5;
    }
    else if (Input_IsGameSpeakPressed(InputCommands::eGameSpeak6))
    {
        return InputCommands::eGameSpeak6;
    }
    else if (Input_IsGameSpeakPressed(InputCommands::eGameSpeak7))
    {
        return InputCommands::eGameSpeak7;
    }
    else if (Input_IsGameSpeakPressed(InputCommands::eGameSpeak8))
    {
        return InputCommands::eGameSpeak8;
    }

    return std::nullopt;
}

bool Input_IsGameSpeakPressedIndirectly(InputCommands gameSpeakId)
{
    auto held = Input().GetHeld();
    auto pressed = Input().GetPressed();

    bool correctSpeakBtnHeld = false;
    if (gameSpeakId >= InputCommands::eGameSpeak1 && gameSpeakId <= InputCommands::eGameSpeak4)
    {
        correctSpeakBtnHeld = held & InputCommands::eLeftGameSpeak;
    }
    else if (gameSpeakId >= InputCommands::eGameSpeak5 && gameSpeakId <= InputCommands::eGameSpeak8)
    {
        correctSpeakBtnHeld = held & InputCommands::eRightGameSpeak;
    }

    if(correctSpeakBtnHeld)
    {
        switch(gameSpeakId)
        {
            case InputCommands::eGameSpeak1:
            {
                return pressed & InputCommands::eHop;
                break;
            }
            case InputCommands::eGameSpeak2:
            {
                return pressed & InputCommands::eDoAction;
                break;
            }
            case InputCommands::eGameSpeak3:
            {
                return pressed & InputCommands::eCrouchOrRoll;
                break;
            }
            case InputCommands::eGameSpeak4:
            {
                return pressed & InputCommands::eThrowItem;
                break;
            }
            case InputCommands::eGameSpeak5:
            {
                return pressed & InputCommands::eDoAction;
                break;
            }
            case InputCommands::eGameSpeak6:
            {
                return pressed & InputCommands::eHop;
                break;
            }
            case InputCommands::eGameSpeak7:
            {
                return pressed & InputCommands::eThrowItem;
                break;
            }
            case InputCommands::eGameSpeak8:
            {
                return pressed & InputCommands::eCrouchOrRoll;
                break;
            }
            default: break;
        }
    }
    return false;
}

bool Input_IsGameSpeakPressed(InputCommands gameSpeakId)
{
    if (Input_IsGameSpeakPressedDirectly(gameSpeakId))
    {
        return true;
    }
    else
    {
        return Input_IsGameSpeakPressedIndirectly(gameSpeakId);
    }
}

static BitField32<AO::InputCommands> AEInputCommandsToAOInputCommands(const BitField32<::InputCommands::Enum>& aeInput)
{
    BitField32<AO::InputCommands> r;
    r.Set(AO::InputCommands::eUp, aeInput.Get(::InputCommands::Enum::eUp));
    r.Set(AO::InputCommands::eRight, aeInput.Get(::InputCommands::Enum::eRight));
    r.Set(AO::InputCommands::eDown, aeInput.Get(::InputCommands::Enum::eDown));
    r.Set(AO::InputCommands::eLeft, aeInput.Get(::InputCommands::Enum::eLeft));
    r.Set(AO::InputCommands::eRun, aeInput.Get(::InputCommands::Enum::eRun));
    r.Set(AO::InputCommands::eHop, aeInput.Get(::InputCommands::Enum::eHop));
    r.Set(AO::InputCommands::eSneak, aeInput.Get(::InputCommands::Enum::eSneak));
    r.Set(AO::InputCommands::eThrowItem, aeInput.Get(::InputCommands::Enum::eThrowItem));
    r.Set(AO::InputCommands::eCrouchOrRoll, aeInput.Get(::InputCommands::Enum::eFartOrRoll));
    r.Set(AO::InputCommands::eDoAction, aeInput.Get(::InputCommands::Enum::eDoAction));
    r.Set(AO::InputCommands::eBack, aeInput.Get(::InputCommands::Enum::eBack));
    r.Set(AO::InputCommands::eUnPause_OrConfirm, aeInput.Get(::InputCommands::Enum::eUnPause_OrConfirm));
    r.Set(AO::InputCommands::ePause, aeInput.Get(::InputCommands::Enum::ePause));
    r.Set(AO::InputCommands::eLeftGameSpeak, aeInput.Get(::InputCommands::Enum::eLeftGameSpeak));
    r.Set(AO::InputCommands::eRightGameSpeak, aeInput.Get(::InputCommands::Enum::eRightGameSpeak));
    r.Set(AO::InputCommands::eGameSpeak1, aeInput.Get(::InputCommands::Enum::eGameSpeak1));
    r.Set(AO::InputCommands::eGameSpeak2, aeInput.Get(::InputCommands::Enum::eGameSpeak2));
    r.Set(AO::InputCommands::eGameSpeak3, aeInput.Get(::InputCommands::Enum::eGameSpeak3));
    r.Set(AO::InputCommands::eGameSpeak4, aeInput.Get(::InputCommands::Enum::eGameSpeak4));
    r.Set(AO::InputCommands::eGameSpeak5, aeInput.Get(::InputCommands::Enum::eGameSpeak5));
    r.Set(AO::InputCommands::eGameSpeak6, aeInput.Get(::InputCommands::Enum::eGameSpeak6));
    r.Set(AO::InputCommands::eGameSpeak7, aeInput.Get(::InputCommands::Enum::eGameSpeak7));
    r.Set(AO::InputCommands::eGameSpeak8, aeInput.Get(::InputCommands::Enum::eGameSpeak8));
    r.Set(AO::InputCommands::eCheatMode, aeInput.Get(::InputCommands::Enum::eCheatMode));
    return r;
}

static BitField32<::InputCommands::Enum> AOInputCommandsToAEInputCommands(const BitField32<AO::InputCommands>& aoInput)
{
    BitField32<::InputCommands::Enum> r;
    r.Set(::InputCommands::Enum::eUp, aoInput.Get(AO::InputCommands::eUp));
    r.Set(::InputCommands::Enum::eRight, aoInput.Get(AO::InputCommands::eRight));
    r.Set(::InputCommands::Enum::eDown, aoInput.Get(AO::InputCommands::eDown));
    r.Set(::InputCommands::Enum::eLeft, aoInput.Get(AO::InputCommands::eLeft));
    r.Set(::InputCommands::Enum::eRun, aoInput.Get(AO::InputCommands::eRun));
    r.Set(::InputCommands::Enum::eHop, aoInput.Get(AO::InputCommands::eHop));
    r.Set(::InputCommands::Enum::eSneak, aoInput.Get(AO::InputCommands::eSneak));
    r.Set(::InputCommands::Enum::eThrowItem, aoInput.Get(AO::InputCommands::eThrowItem));
    r.Set(::InputCommands::Enum::eFartOrRoll, aoInput.Get(AO::InputCommands::eCrouchOrRoll));
    r.Set(::InputCommands::Enum::eDoAction, aoInput.Get(AO::InputCommands::eDoAction));
    r.Set(::InputCommands::Enum::eBack, aoInput.Get(AO::InputCommands::eBack));
    r.Set(::InputCommands::Enum::eUnPause_OrConfirm, aoInput.Get(AO::InputCommands::eUnPause_OrConfirm));
    r.Set(::InputCommands::Enum::ePause, aoInput.Get(AO::InputCommands::ePause));

    // OG issue - LCD screens says hold alt + shift which is wrong
    r.Set(::InputCommands::Enum::eChant, aoInput.Get(AO::InputCommands::eLeftGameSpeak) && aoInput.Get(AO::InputCommands::eRightGameSpeak));

    r.Set(::InputCommands::Enum::eGameSpeak1, aoInput.Get(AO::InputCommands::eGameSpeak1));
    r.Set(::InputCommands::Enum::eGameSpeak2, aoInput.Get(AO::InputCommands::eGameSpeak2));
    r.Set(::InputCommands::Enum::eGameSpeak3, aoInput.Get(AO::InputCommands::eGameSpeak3));
    r.Set(::InputCommands::Enum::eGameSpeak4, aoInput.Get(AO::InputCommands::eGameSpeak4));
    r.Set(::InputCommands::Enum::eGameSpeak5, aoInput.Get(AO::InputCommands::eGameSpeak5));
    r.Set(::InputCommands::Enum::eGameSpeak6, aoInput.Get(AO::InputCommands::eGameSpeak6));
    r.Set(::InputCommands::Enum::eGameSpeak7, aoInput.Get(AO::InputCommands::eGameSpeak7));
    r.Set(::InputCommands::Enum::eGameSpeak8, aoInput.Get(AO::InputCommands::eGameSpeak8));
    r.Set(::InputCommands::Enum::eCheatMode, aoInput.Get(AO::InputCommands::eCheatMode));

    // needed for remapping Speak I and Speak II on controllers
    r.Set(::InputCommands::Enum::eLeftGameSpeak, aoInput.Get(AO::InputCommands::eLeftGameSpeak));
    r.Set(::InputCommands::Enum::eRightGameSpeak, aoInput.Get(AO::InputCommands::eRightGameSpeak));

    return r;
}

static const char_type* AEInputCommandToAEInputString(::InputCommands::Enum input_command)
{
    if (input_command & ::InputCommands::Enum::eUp)
    {
        return kUp;
    }

    if (input_command & ::InputCommands::Enum::eDown)
    {
        return kDown;
    }

    if (input_command & ::InputCommands::Enum::eLeft)
    {
        return kLeft;
    }

    if (input_command & ::InputCommands::Enum::eRight)
    {
        return kRight;
    }

    if (input_command & ::InputCommands::Enum::eRun)
    {
        return kRun;
    }

    if (input_command & ::InputCommands::Enum::eHop)
    {
        return kJump;
    }

    if (input_command & ::InputCommands::Enum::eSneak)
    {
        return kSneak;
    }

    if (input_command & ::InputCommands::Enum::eThrowItem)
    {
        return kThrow;
    }

    if (input_command & ::InputCommands::Enum::eFartOrRoll)
    {
        return kFart; // TODO: wrong ?
    }

    if (input_command & ::InputCommands::Enum::eDoAction)
    {
        return kAction;
    }

    if (input_command & ::InputCommands::Enum::eChant)
    {
        return kChant;
    }

    if (input_command & ::InputCommands::Enum::eLeftGameSpeak)
    {
        return kSpeak1;
    }

    if (input_command & ::InputCommands::Enum::eRightGameSpeak)
    {
        return kSpeak2;
    }

    if (input_command & ::InputCommands::Enum::eUnPause_OrConfirm)
    {
        return kConfirm;
    }

    if (input_command & ::InputCommands::Enum::eBack)
    {
        return kBack;
    }

    // TODO: Game speaks
    /*
#define kAllYa "\x10"
#define kSorry "\x11"
#define kStopIt "\x12"

#define kFart "\x0a"
#define kHello "\x0b"
#define kFollowMe "\x0c"
#define kWait "\x0d"
#define kWork "\x0e"
#define kAnger "\x0f"

#define kHoistZTurn "\x1b"
#define kDPad "\x1a"
*/

    return kUp;
}


static BitField32<::InputCommands::Enum> MakeAEInputBits(u32 bits)
{
    BitField32<::InputCommands::Enum> r;
    r.Raw().all = bits;
    return r;
}

static BitField32<AO::InputCommands> MakeAOInputBits(u32 bits)
{
    BitField32<AO::InputCommands> r;
    r.Raw().all = bits;
    return r;
}

void InputObject::Update(BaseGameAutoPlayer& gameAutoPlayer)
{
    const u8 byte_4BB428[16] = {0u, 64u, 0u, 32u, 192u, 0u, 224u, 0u, 128u, 96u, 0u, 0u, 160u, 0u, 0u, 0u};

    for (s32 i = 0; i < 2; i++)
    {
        mPads[i].mPreviousDir = mPads[i].mDir;
        mPads[i].field_B = mPads[i].field_3;
        mPads[i].mPreviousInput = mPads[i].mRawInput;
    }

    // Do AE input reading
    ::Input().Update(gameAutoPlayer);

    for (auto controllerId = 0; controllerId < 2; controllerId ++) //todo constant for controllers num
    {
        // Convert from AE bit flags to AO bit flags
        mPads[controllerId].mRawInput = static_cast<u32>(AEInputCommandsToAOInputCommands(MakeAEInputBits(::Input().mPads[controllerId].mRawInput)).Raw().all);

        // TODO: This one probably needs its own conversion
        mPads[controllerId].mDir = ::Input().mPads[controllerId].mDir;

        mPads[controllerId].mPreviousInput = static_cast<u32>(AEInputCommandsToAOInputCommands(MakeAEInputBits(::Input().mPads[controllerId].mPreviousInput)).Raw().all);
        mPads[controllerId].mPressed = static_cast<u32>(AEInputCommandsToAOInputCommands(MakeAEInputBits(::Input().mPads[controllerId].mPressed)).Raw().all);
        mPads[controllerId].mReleased = static_cast<u32>(AEInputCommandsToAOInputCommands(MakeAEInputBits(::Input().mPads[controllerId].mReleased)).Raw().all);
    }



    // Handle demo input (AO impl)
    if (mbDemoPlaying & 1)
    {
        // Stop if any button on any pad is pressed
        if (mPads[sCurrentControllerIndex].mRawInput)
        {
            mbDemoPlaying &= ~1u;
            return;
        }

        if (static_cast<s32>(sGnFrame) >= mCommandDuration)
        {
            const u32 command = (*mpDemoRes)[mDemoCommandIndex++];
            mCommand = command >> 16;
            mCommandDuration = sGnFrame + (command & 0xFFFF);

            // End demo/quit command
            if (command & 0x8000)
            {
                mbDemoPlaying &= ~1u;
            }
        }

        // Will do nothing if we hit the end command..
        if (mbDemoPlaying & 1)
        {
            mPads[0].mRawInput = static_cast<u32>(mCommand);
        }

        for (s32 i = 0; i < 2; i++)
        {
            mPads[i].mReleased = ~mPads[i].mRawInput & mPads[i].mPreviousInput;
            mPads[i].mPressed = ~mPads[i].mPreviousInput & mPads[i].mRawInput;
            mPads[i].mDir = byte_4BB428[(mPads[i].mRawInput >> 12) & 0xF];
            mPads[i].field_3 = byte_4BB428[(mPads[i].mRawInput >> 4) & 0xF];
        }

        return;
    }
}

void InputObject::Shutdown()
{
}


void InputObject::SetDemoRes(u32** ppDemoRes)
{
    mDemoCommandIndex = 2051;
    mpDemoRes = ppDemoRes;
    mbDemoPlaying |= 1u;
    mCommandDuration = 0;
}

s32 InputObject::IsDemoPlaying()
{
    return mbDemoPlaying & 1;
}

static s32 PadIndexToInt(InputObject::PadIndex idx)
{
    switch (idx)
    {
        case InputObject::PadIndex::Active:
            return sCurrentControllerIndex;

        case InputObject::PadIndex::Second:
            return 1;

        case InputObject::PadIndex::First:
        default:
            return 0;
    }
}

InputObject::PadIndex InputObject::CurrentController() const
{
    if (sCurrentControllerIndex == 0)
    {
        return PadIndex::First;
    }
    return PadIndex::Second;
}


void InputObject::SetCurrentController(PadIndex padIdx)
{
    sCurrentControllerIndex = padIdx == InputObject::PadIndex::First ? 0 : 1;
}


bool InputObject::JoyStickEnabled() const
{
    return ::Input_JoyStickEnabled();
}

bool InputObject::IsAnyHeld(u32 command) const
{
    return IsAnyHeld(PadIndex::Active, command);
}

bool InputObject::IsAnyHeld(PadIndex padIx, u32 command) const
{
    return (mPads[PadIndexToInt(padIx)].mRawInput & command) != 0;
}

bool InputObject::IsAnyPressed(u32 command) const
{
    return IsAnyPressed(PadIndex::Active, command);
}

bool InputObject::IsAnyPressed(PadIndex padIx, u32 command) const
{
    return (mPads[PadIndexToInt(padIx)].mPressed & command) != 0;
}

bool InputObject::IsAnyReleased(u32 command) const
{
    return IsAnyReleased(PadIndex::Active, command);
}

bool InputObject::IsAnyReleased(PadIndex padIx, u32 command) const
{
    return (mPads[PadIndexToInt(padIx)].mReleased & command) != 0;
}

u8 InputObject::Dir() const
{
    return Input().mPads[sCurrentControllerIndex].mDir >> 5;
}

u32 InputObject::GetHeld() const
{
    return GetHeld(PadIndex::Active);
}

u32 InputObject::GetHeld(PadIndex padIx) const
{
    return mPads[PadIndexToInt(padIx)].mRawInput;
}

u32 InputObject::GetPressed() const
{
    return GetPressed(PadIndex::Active);
}

u32 InputObject::GetPressed(PadIndex padIx) const
{
    return Input().mPads[PadIndexToInt(padIx)].mPressed;
}

u32 InputObject::Input_Read_Pad(u32 padIdx)
{
    return ::Input_Read_Pad(padIdx);
}

u32 InputObject::GetReleased() const
{
    return Input().mPads[sCurrentControllerIndex].mReleased;
}

bool Input_IsChanting()
{
    return ::Input_IsChanting();
}

void Input_SetKeyState(s32 key, s8 bIsDown)
{
    ::Input_SetKeyState(key, bIsDown);
}

s8 Input_IsVKPressed(s32 key)
{
    return Input_IsVKPressed_4EDD40(key);
}

// from the MainMenu class
extern s32 gJoystickAvailable;

void Input_Init()
{
    ::Input_Init();

    if (Input_JoyStickAvailable())
    {
        gJoystickAvailable = 1;
    }
}

void Input_DisableInput()
{
    Input_DisableInputForPauseMenuAndDebug_4EDDC0();
}

// Only really needed for SHIFT being pressed for the AO ddcheat, may as well just remove the requirement
// for shift to be pressed because it is a pain anyway.
void Input_GetCurrentKeyStates()
{
}

void Input_InitKeyStateArray()
{
    Input_InitKeyStateArray_4EDD60();
}

const char_type* Input_GetButtonString(InputCommands inputCommand, bool forceKeyboardLookupIfGamepadFails)
{
    const auto aeBits = static_cast<::InputCommands::Enum>(AOInputCommandsToAEInputCommands(MakeAOInputBits(inputCommand)).Raw().all);
    s32 controller_type = forceKeyboardLookupIfGamepadFails ? 2 : 1;

    return ::Input_GetButtonString_492530(
        AEInputCommandToAEInputString(aeBits),
        ::Input_JoyStickEnabled() ? controller_type : 0);
}

const char_type* Input_GetButtonString_44F1C0(InputCommands inputCommand)
{
    return Input_GetButtonString(inputCommand);
}

s32 Input_Remap(InputCommands inputCmd)
{
    return Input_Remap_492680(static_cast<::InputCommands::Enum>(AOInputCommandsToAEInputCommands(MakeAOInputBits(inputCmd)).Raw().all));
}

s8 Input_GetLastPressedKey()
{
    // AE impl
    return static_cast<s8>(::Input_GetLastPressedKey_492610());

    // AO impl
    //if (!Sys_IsAnyKeyDown())
    //{
    //    return 0;
    //}

    //const s8 result = static_cast<s8>(sLastPressedKey);
    //sIsAKeyDown = 0;
    //sLastPressedKey = 0;
    //return result;
}

s32 Input_Enable_48E6A0()
{
    // AE impl
    ::Input_EnableInput_4EDDD0();
    return 0;

    // AO impl
    //sInputEnabled = 1;
    //return 0;
}

void Input_Reset()
{
    // Funcs below call AE impls in standalone
    Input_Enable_48E6A0();
    Input_InitKeyStateArray();
}

bool Input_JoyStickAvailable()
{
    return ::Input_JoyStickAvailable();
}

void Input_SetJoyStickEnabled(bool enabled)
{
    // Use AE var
    return ::Input_SetJoyStickEnabled(enabled);

    // Use AO var
    //sJoystickEnabled = enabled;
}

u32 dword_508A64 = 0;

s32 Input_SaveSettingsIni()
{
    // Call AE func both in standalone and DLL
    Input_SaveSettingsIni_Common();
    return 1;

    // AO approach
    //FILE* iniFileHandle = fopen("abe.ini", "w");
    //if (!iniFileHandle)
    //{
    //    return 0;
    //}

    //dword_508A64 = 1;
    //fprintf(iniFileHandle, "[Control Layout]\n");
    //if (Input_JoyStickEnabled())
    //{
    //    fprintf(iniFileHandle, "controller = Game Pad\n");

    //}
    //else
    //{
    //    fprintf(iniFileHandle, "controller = Keyboard\n");
    //}
    //
    //const auto oldJoystickEnabled = Input_JoyStickEnabled();

    //Input_SetJoyStickEnabled(false);

    //fprintf(iniFileHandle, "[Keyboard]\n");
    //fprintf(iniFileHandle, "run = %s\n", Input_GetButtonString_44F1C0(InputCommands::eRun));
    //fprintf(iniFileHandle, "sneak = %s\n", Input_GetButtonString_44F1C0(InputCommands::eSneak));
    //fprintf(iniFileHandle, "jump = %s\n", Input_GetButtonString_44F1C0(InputCommands::eHop));
    //fprintf(iniFileHandle, "action = %s\n", Input_GetButtonString_44F1C0(InputCommands::eDoAction));
    //fprintf(iniFileHandle, "throw = %s\n", Input_GetButtonString_44F1C0(InputCommands::eThrowItem));
    //fprintf(iniFileHandle, "crouch = %s\n", Input_GetButtonString_44F1C0(InputCommands::eCrouchOrRoll));

    //Input_SetJoyStickEnabled(true);

    //fprintf(iniFileHandle, "[Game Pad]\n");
    //fprintf(iniFileHandle, "run = %s\n", Input_GetButtonString_44F1C0(InputCommands::eRun));
    //fprintf(iniFileHandle, "sneak = %s\n", Input_GetButtonString_44F1C0(InputCommands::eSneak));
    //fprintf(iniFileHandle, "jump = %s\n", Input_GetButtonString_44F1C0(InputCommands::eHop));
    //fprintf(iniFileHandle, "action = %s\n", Input_GetButtonString_44F1C0(InputCommands::eDoAction));
    //fprintf(iniFileHandle, "throw = %s\n", Input_GetButtonString_44F1C0(InputCommands::eThrowItem));
    //fprintf(iniFileHandle, "crouch = %s\n", Input_GetButtonString_44F1C0(InputCommands::eCrouchOrRoll));

    //Input_SetJoyStickEnabled(oldJoystickEnabled);

    //fclose(iniFileHandle);
    //dword_508A64 = 0;
    //return 1;
}

InputObject& Input()
{
    return sInputObject;
}

} // namespace AO
