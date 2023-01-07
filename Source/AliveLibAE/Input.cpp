#include "stdafx.h"
#include "Input.hpp"
#include "../relive_lib/logger.hpp"
#include "../relive_lib/Function.hpp"
#include "Game.hpp"
#include "../relive_lib/Sys.hpp"
#include "../relive_lib/Sound/Sound.hpp"
#include "../relive_lib/Events.hpp"
#include "PsxRender.hpp"
#include "GameAutoPlayer.hpp"
#include "../relive_lib/data_conversion/string_util.hpp"
#include <sstream>
#include <algorithm>
#include "../relive_lib/BitField.hpp"
#include <SDL_gamecontroller.h>

static SDL_GameController* pSDLController = nullptr;

// -- Variables -- //

const InputCommands::Enum sInputKey_Right = InputCommands::Enum::eRight;
const InputCommands::Enum sInputKey_Left = InputCommands::Enum::eLeft;
const InputCommands::Enum sInputKey_Up = InputCommands::Enum::eUp;
const InputCommands::Enum sInputKey_Down = InputCommands::Enum::eDown;
const InputCommands::Enum sInputKey_Hop = InputCommands::Enum::eHop;
const InputCommands::Enum sInputKey_DoAction = InputCommands::Enum::eDoAction;
const InputCommands::Enum sInputKey_Run = InputCommands::Enum::eRun;
const InputCommands::Enum sInputKey_Sneak = InputCommands::Enum::eSneak;
const InputCommands::Enum sInputKey_FartRoll = InputCommands::Enum::eFartOrRoll;
const InputCommands::Enum sInputKey_ThrowItem = InputCommands::Enum::eThrowItem;
const InputCommands::Enum sInputKey_GameSpeak2 = InputCommands::Enum::eGameSpeak2;
const InputCommands::Enum sInputKey_GameSpeak4 = InputCommands::Enum::eGameSpeak4;
const InputCommands::Enum sInputKey_GameSpeak3 = InputCommands::Enum::eGameSpeak3;
const InputCommands::Enum sInputKey_GameSpeak1 = InputCommands::Enum::eGameSpeak1;
const InputCommands::Enum sInputKey_GameSpeak6 = InputCommands::Enum::eGameSpeak6;
const InputCommands::Enum sInputKey_GameSpeak5 = InputCommands::Enum::eGameSpeak5;
const InputCommands::Enum sInputKey_GameSpeak8 = InputCommands::Enum::eGameSpeak8;
const InputCommands::Enum sInputKey_GameSpeak7 = InputCommands::Enum::eGameSpeak7;

const InputCommands::Enum sInputKey_Chant = InputCommands::Enum::eChant;

// Bitmask for all menu-exclusive (navigation, entering, etc.) input commands
const u32 AllMenuCommandsMask = (InputCommands::Enum::ePause | InputCommands::Enum::eUnPause_OrConfirm | InputCommands::Enum::eBack | InputCommands::Enum::eConfigure);

// Is a joystick plugged in and can be switched to?
bool sJoystickAvailable = false;

// Is the joystick selected as the active controller device (instead of keyboard)?
bool sJoystickEnabled = false;

s32 sJoystickNumButtons_5C2EFC = 0;
s32 sJoystickID_5C2F00 = 0;
s32 sJoystickCapFlags_5C2EDC = 0;
bool sJoyStateIsInit_5C2EE0 = 0;
s32 sJoyLastTick_5C2EEC = 0;
s32 sGamepadCapFlags_5C2EF8 = 0;


u8 sInputKeyStates_BD2F60[256] = {};
u8 sInputEnabled_BBB9D0 = 0;
InputObject sInputObject = {};
u16 sCurrentControllerIndex = 0;
s16 bLongerTimeoutToNextDemo = 0;
u32 sLastPressedKey = 0;
s32 sIsAKeyDown = 0;
s8 sAllowedGameKeys_5C9D30[256] = {};
const char_type* sKeyNames_5C9394[256] = {};
const char_type* sJoyButtonNames_5C9908[10] = {};

t_InputCallback sInputCallbackFunc_BD1870 = 0;

char_type sGamePadStr_555708[32] = {"Game Pad"};
char_type sGamePadStr_55E85C[32] = {"Game Pad"};

s32 bAbsX_5C2EE8 = 0;
s32 bAbsY_5C2EA4 = 0;
s32 bAutoX_5C2EE4 = 0;
s32 bAutoY_5C2EF0 = 0;

u32 sLastPad_Input_BD1878 = 0;
bool sReadPadEnable_BD1874 = false;

const InputBinding sDefaultKeyboardBindings_55EAD8[36] = {
    {VK_LEFT, InputCommands::Enum::eLeft},
    {VK_RIGHT, InputCommands::Enum::eRight},
    {VK_UP, InputCommands::Enum::eUp},
    {VK_DOWN, InputCommands::Enum::eDown},
    {VK_CONTROL, InputCommands::Enum::eDoAction},
    {VK_MENU, InputCommands::Enum::eSneak},
    {VK_SHIFT, InputCommands::Enum::eRun},
    {VK_SPACE, InputCommands::Enum::eHop},
    {'Z', InputCommands::Enum::eThrowItem},
    {'X', static_cast<InputCommands::Enum>(InputCommands::Enum::eUnPause_OrConfirm | InputCommands::Enum::eFartOrRoll)},
    {VK_ESCAPE, static_cast<InputCommands::Enum>(InputCommands::Enum::ePause | InputCommands::Enum::eBack)},
    {VK_RETURN, static_cast<InputCommands::Enum>(InputCommands::Enum::eUnPause_OrConfirm | InputCommands::Enum::eFartOrRoll)},
    {VK_TAB, InputCommands::Enum::eCheatMode},
    {'1', InputCommands::Enum::eGameSpeak1},
    {'2', InputCommands::Enum::eGameSpeak2},
    {'3', InputCommands::Enum::eGameSpeak3},
    {'4', InputCommands::Enum::eGameSpeak4},
    {'5', InputCommands::Enum::eGameSpeak5},
    {'6', InputCommands::Enum::eGameSpeak6},
    {'7', InputCommands::Enum::eGameSpeak7},
    {'8', InputCommands::Enum::eGameSpeak8},
    {'0', InputCommands::Enum::eChant},
    {VK_NUMPAD1, InputCommands::Enum::eGameSpeak1},
    {VK_NUMPAD2, InputCommands::Enum::eGameSpeak2},
    {VK_NUMPAD3, InputCommands::Enum::eGameSpeak3},
    {VK_NUMPAD4, InputCommands::Enum::eGameSpeak4},
    {VK_NUMPAD5, InputCommands::Enum::eGameSpeak5},
    {VK_NUMPAD6, InputCommands::Enum::eGameSpeak6},
    {VK_NUMPAD7, InputCommands::Enum::eGameSpeak7},
    {VK_NUMPAD8, InputCommands::Enum::eGameSpeak8},
    {VK_NUMPAD0, InputCommands::Enum::eChant},
    {'C', InputCommands::Enum::eConfigure},
    {VK_PRIOR, static_cast<InputCommands::Enum>(0x20000000)},
    {VK_NEXT, static_cast<InputCommands::Enum>(0x40000000)},
    {VK_DELETE, static_cast<InputCommands::Enum>(0x10000000)},
    {0, static_cast<InputCommands::Enum>(0)}
};

#define GAME_PAD_VK_BIT_FACE_BTN_LEFT 0
#define GAME_PAD_VK_BIT_FACE_BTN_BOTTOM 1
#define GAME_PAD_VK_BIT_FACE_BTN_RIGHT 2
#define GAME_PAD_VK_BIT_FACE_BTN_TOP 3
#define GAME_PAD_VK_BIT_LEFT_SHOULDER 4
#define GAME_PAD_VK_BIT_RIGHT_SHOULDER 5
#define GAME_PAD_VK_BIT_LEFT_TRIGGER 6
#define GAME_PAD_VK_BIT_RIGHT_TRIGGER 7
#define GAME_PAD_VK_BIT_BACK 8
#define GAME_PAD_VK_BIT_START 9
enum Flags_VKGamepad_Buttons_Pressed
{
    eBit1_face_btn_left = 0x1,
    eBit2_face_btn_bottom = 0x2,
    eBit3_face_btn_right = 0x4,
    eBit4_face_btn_top = 0x8,
    eBit5_left_shoulder = 0x10,
    eBit6_right_shoulder = 0x20,
    eBit7_left_trigger = 0x40,
    eBit8_right_trigger = 0x80,
    eBit9_back = 0x100,
    eBit10_start = 0x200
};

const s16 gamePadBindingsSize = 10;
const InputBinding sDefaultGamepadBindings_55EA2C[gamePadBindingsSize] = {
    {GAME_PAD_VK_BIT_FACE_BTN_LEFT, InputCommands::Enum::eDoAction},                                             // Square / X
    {GAME_PAD_VK_BIT_FACE_BTN_BOTTOM, static_cast<InputCommands::Enum>(InputCommands::Enum::eFartOrRoll | InputCommands::Enum::eUnPause_OrConfirm)}, // Cross / A
    {GAME_PAD_VK_BIT_FACE_BTN_RIGHT, InputCommands::Enum::eThrowItem},                                            // Circle / B
    {GAME_PAD_VK_BIT_FACE_BTN_TOP, static_cast<InputCommands::Enum>(InputCommands::Enum::eHop | InputCommands::Enum::eBack)},                     // Triangle / Y
    {GAME_PAD_VK_BIT_LEFT_SHOULDER, InputCommands::Enum::eLeftGameSpeak},                                               // L1 / LB
    {GAME_PAD_VK_BIT_RIGHT_SHOULDER, InputCommands::Enum::eRun},                                                  // R1 / RB
    {GAME_PAD_VK_BIT_LEFT_TRIGGER, InputCommands::Enum::eRightGameSpeak},                                               // L2 / LT
    {GAME_PAD_VK_BIT_RIGHT_TRIGGER, InputCommands::Enum::eSneak},                                                // R2 / RT
    {GAME_PAD_VK_BIT_BACK, InputCommands::Enum::eBack},
    {GAME_PAD_VK_BIT_START, static_cast<InputCommands::Enum>(InputCommands::Enum::ePause | InputCommands::Enum::eUnPause_OrConfirm)} // Start / Menu
};

u32 sKeyboardBindings_5C9930[256] = {};
u32 sGamePadBindings_5C98E0[gamePadBindingsSize] = {};


// For joysticks with very little buttons, depending on strength of joystick, will make abe
// automatically run/sneak.
void Input_StickControl_45FF60(f32 x, f32 y, u32* buttons)
{
    if (!bAutoX_5C2EE4 && !bAutoY_5C2EF0)
    {
        return;
    }

    if (bAutoX_5C2EE4)
    {
        if (bAbsX_5C2EE8)
        {
            x = -x;
        }

        if (bAutoX_5C2EE4 == 1)
        {
            if (x > 0.3f)
            {
                *buttons |= InputCommands::Enum::eRun;
            }
        }
        else if (bAutoX_5C2EE4 == 2)
        {
            if (x > 0.15f)
            {
                *buttons |= InputCommands::Enum::eSneak;
            }

            if (x < -0.25f)
            {
                *buttons |= InputCommands::Enum::eRun;
            }
        }
    }

    if (bAutoY_5C2EF0)
    {
        if (bAbsY_5C2EA4)
        {
            y = -y;
        }

        if (bAutoY_5C2EF0 == 1)
        {
            if (y > 0.3f)
            {
                *buttons |= InputCommands::Enum::eDoAction;
            }
        }
        else if (bAutoY_5C2EF0 == 2)
        {
            if (y > 0.15f)
            {
                *buttons |= InputCommands::Enum::eThrowItem;
            }

            if (y < -0.25f)
            {
                *buttons |= InputCommands::Enum::eDoAction;
            }
        }
    }
}

void Input_GetJoyState_SDL(f32* pX1, f32* pY1, f32* pX2, f32* pY2, BitField16<Flags_VKGamepad_Buttons_Pressed>& pButtons)
{
    f32 deadzone = 0.2f;

    *pX1 = 0;
    *pY1 = 0;
    *pX2 = 0;
    *pY2 = 0;

    if (pSDLController != nullptr)
    {
        sJoystickNumButtons_5C2EFC = SDL_JoystickNumButtons(SDL_GameControllerGetJoystick(pSDLController));
        sGamepadCapFlags_5C2EF8 = 0xFFFF;

        f32 f_LX = SDL_GameControllerGetAxis(pSDLController, SDL_CONTROLLER_AXIS_LEFTX) / 32767.0f;
        f32 f_LY = SDL_GameControllerGetAxis(pSDLController, SDL_CONTROLLER_AXIS_LEFTY) / 32767.0f;

        f32 f_RX = SDL_GameControllerGetAxis(pSDLController, SDL_CONTROLLER_AXIS_RIGHTX) / 32767.0f;
        f32 f_RY = SDL_GameControllerGetAxis(pSDLController, SDL_CONTROLLER_AXIS_RIGHTY) / 32767.0f;

        // Joysticks
        if (abs(f_LX) > deadzone)
            *pX1 = f_LX;
        if (abs(f_LY) > deadzone)
            *pY1 = f_LY;

        if (abs(f_RX) > deadzone)
            *pX2 = f_RX;
        if (abs(f_RY) > deadzone)
            *pY2 = f_RY;

        // DPad Movement
        if (SDL_GameControllerGetButton(pSDLController, SDL_CONTROLLER_BUTTON_DPAD_RIGHT))
            *pX1 = 1;
        else if (SDL_GameControllerGetButton(pSDLController, SDL_CONTROLLER_BUTTON_DPAD_LEFT))
            *pX1 = -1;
        if (SDL_GameControllerGetButton(pSDLController, SDL_CONTROLLER_BUTTON_DPAD_UP))
            *pY1 = -1;
        else if (SDL_GameControllerGetButton(pSDLController, SDL_CONTROLLER_BUTTON_DPAD_DOWN))
            *pY1 = 1;

        const auto isButtonPressed = [&](SDL_GameControllerButton button) -> bool
        {
            return SDL_GameControllerGetButton(pSDLController, button) == 1;
        };
        const auto isAxisPressed = [&](SDL_GameControllerAxis axis) -> bool
        {
            return SDL_GameControllerGetAxis(pSDLController, axis) > 32;
        };

        pButtons.Set(Flags_VKGamepad_Buttons_Pressed::eBit1_face_btn_left, isButtonPressed(SDL_CONTROLLER_BUTTON_X) );
        pButtons.Set(Flags_VKGamepad_Buttons_Pressed::eBit2_face_btn_bottom, isButtonPressed(SDL_CONTROLLER_BUTTON_A) );
        pButtons.Set(Flags_VKGamepad_Buttons_Pressed::eBit3_face_btn_right, isButtonPressed(SDL_CONTROLLER_BUTTON_B) );
        pButtons.Set(Flags_VKGamepad_Buttons_Pressed::eBit4_face_btn_top, isButtonPressed(SDL_CONTROLLER_BUTTON_Y) );
        pButtons.Set(Flags_VKGamepad_Buttons_Pressed::eBit5_left_shoulder, isButtonPressed(SDL_CONTROLLER_BUTTON_LEFTSHOULDER) );
        pButtons.Set(Flags_VKGamepad_Buttons_Pressed::eBit6_right_shoulder, isButtonPressed(SDL_CONTROLLER_BUTTON_RIGHTSHOULDER) );
        pButtons.Set(Flags_VKGamepad_Buttons_Pressed::eBit7_left_trigger, isAxisPressed(SDL_CONTROLLER_AXIS_TRIGGERLEFT) );
        pButtons.Set(Flags_VKGamepad_Buttons_Pressed::eBit8_right_trigger, isAxisPressed(SDL_CONTROLLER_AXIS_TRIGGERRIGHT) );
        pButtons.Set(Flags_VKGamepad_Buttons_Pressed::eBit9_back, isButtonPressed(SDL_CONTROLLER_BUTTON_BACK) );
        pButtons.Set(Flags_VKGamepad_Buttons_Pressed::eBit10_start, isButtonPressed(SDL_CONTROLLER_BUTTON_START) );

        static f32 vibrationAmount = 0.0f;
        s32 screenShake = std::max(abs(gScreenXOffset), abs(gScreenYOffset));

        if (screenShake > 0)
        {
            vibrationAmount = std::min(screenShake, 30) / 30.0f;
        }
        else if (EventGet(kEventScreenShake))
        {
            vibrationAmount = 1.0f;
        }

        if (pSDLController)
        {
            const u16 amount = static_cast<u16>(vibrationAmount * 0xFFFF);
            SDL_GameControllerRumble(pSDLController, amount, amount, 200);
        }

        vibrationAmount -= 0.2f;
        vibrationAmount = std::max(0.0f, vibrationAmount);
    }
}

void Input_GetJoyState_460280(f32* pX1, f32* pY1, f32* pX2, f32* pY2, BitField16<Flags_VKGamepad_Buttons_Pressed>& pButtons)
{
    Input_GetJoyState_SDL(pX1, pY1, pX2, pY2, pButtons);
}

u8 Input_GetInputEnabled_4EDDE0()
{
    return sInputEnabled_BBB9D0 != 0;
}

s8 Input_GetKeyState_4EDD20(s32 key)
{
    const s8 keyState = sInputKeyStates_BD2F60[key] & 0x80;
    sInputKeyStates_BD2F60[key] = keyState;
    return keyState;
}

void Input_EnableInput_4EDDD0()
{
    sInputEnabled_BBB9D0 = true;
}

void Input_DisableInputForPauseMenuAndDebug_4EDDC0()
{
    sInputEnabled_BBB9D0 = false;
}

struct KeyName final
{
    char_type field_0_name[10];
};

struct KeyNames final
{
    KeyName keys[32];
};

KeyNames sKeyboardDisplayKeyNames_5C9E30 = {};
KeyNames sGamepadDisplayKeyNames_5C9798 = {};

void Input_Init_Names_491870()
{
    sKeyboardDisplayKeyNames_5C9E30 = {};

    strcpy(sKeyboardDisplayKeyNames_5C9E30.keys[0].field_0_name, kAllYa);
    strcpy(sKeyboardDisplayKeyNames_5C9E30.keys[1].field_0_name, kSorry);
    strcpy(sKeyboardDisplayKeyNames_5C9E30.keys[2].field_0_name, kStopIt);
    strcpy(sKeyboardDisplayKeyNames_5C9E30.keys[3].field_0_name, kChant);

    u32 bindingMask = 16;
    for (s32 i = 4; i < 32; i++)
    {
        char_type* keyNamePtr = sKeyboardDisplayKeyNames_5C9E30.keys[i].field_0_name;
        for (s32 keyNameIdx = 0; keyNameIdx < 256; keyNameIdx++)
        {
            if (bindingMask & sKeyboardBindings_5C9930[keyNameIdx])
            {
                strcpy(keyNamePtr, sKeyNames_5C9394[keyNameIdx]);
            }
        }

        bindingMask <<= 1;
    }

    s32 eSpeak1idx = -1;
    s32 eSpeak2idx = -1;

    sGamepadDisplayKeyNames_5C9798 = {};

    for (s32 i = 0; i < gamePadBindingsSize; i++)
    {
        if (sGamePadBindings_5C98E0[i] & InputCommands::Enum::eLeftGameSpeak)
        {
            sprintf(sGamepadDisplayKeyNames_5C9798.keys[10].field_0_name, "%s+%s", sJoyButtonNames_5C9908[i], sJoyButtonNames_5C9908[3]);
            sprintf(sGamepadDisplayKeyNames_5C9798.keys[11].field_0_name, "%s+%s", sJoyButtonNames_5C9908[i], sJoyButtonNames_5C9908[0]);
            sprintf(sGamepadDisplayKeyNames_5C9798.keys[12].field_0_name, "%s+%s", sJoyButtonNames_5C9908[i], sJoyButtonNames_5C9908[1]);
            sprintf(sGamepadDisplayKeyNames_5C9798.keys[13].field_0_name, "%s+%s", sJoyButtonNames_5C9908[i], sJoyButtonNames_5C9908[2]);
            eSpeak1idx = i;
        }
        else if (sGamePadBindings_5C98E0[i] & InputCommands::Enum::eRightGameSpeak)
        {
            sprintf(sGamepadDisplayKeyNames_5C9798.keys[14].field_0_name, "%s+%s", sJoyButtonNames_5C9908[i], sJoyButtonNames_5C9908[1]);
            sprintf(sGamepadDisplayKeyNames_5C9798.keys[15].field_0_name, "%s+%s", sJoyButtonNames_5C9908[i], sJoyButtonNames_5C9908[3]);
            sprintf(sGamepadDisplayKeyNames_5C9798.keys[16].field_0_name, "%s+%s", sJoyButtonNames_5C9908[i], sJoyButtonNames_5C9908[2]);
            sprintf(sGamepadDisplayKeyNames_5C9798.keys[17].field_0_name, "%s+%s", sJoyButtonNames_5C9908[i], sJoyButtonNames_5C9908[0]);
            eSpeak2idx = i;
        }

        if (sGamePadBindings_5C98E0[i])
        {
            s32 curBinding = sGamePadBindings_5C98E0[i];
            for (s32 k = 0; k < 32; k++)
            {
                if (curBinding & 1)
                {
                    strcpy(sGamepadDisplayKeyNames_5C9798.keys[k].field_0_name, sJoyButtonNames_5C9908[i]);
                }
                curBinding >>= 1;
                if (!curBinding)
                {
                    break;
                }
            }
        }
    }

    if (eSpeak1idx != -1 && eSpeak2idx != -1)
    {
        sprintf(sGamepadDisplayKeyNames_5C9798.keys[18].field_0_name, "%s+%s", sJoyButtonNames_5C9908[eSpeak1idx], sJoyButtonNames_5C9908[eSpeak2idx]);
    }

    // NOTE: diversion from OG!
    // adding the UnPause_OrConfirm command's own gamepad button string (AE does not use it in menus, but AO does)
    strcpy(sGamepadDisplayKeyNames_5C9798.keys[20].field_0_name, sJoyButtonNames_5C9908[1]);
}

const char_type* Input_GetButtonString_492530(const char_type* idx, s32 controllerType)
{
    const char_type* ret = ""; // don't crash the game if the buttonstring cannot be looked up
    if (controllerType == 0)
    {
        ret = sKeyboardDisplayKeyNames_5C9E30.keys[*idx - 1].field_0_name;
    }
    else
    {
        if (sGamepadDisplayKeyNames_5C9798.keys[*idx - 1].field_0_name[0])
        {
            ret = sGamepadDisplayKeyNames_5C9798.keys[*idx - 1].field_0_name;
        }
        else
        {
            if (controllerType == 2)
            {
                ret = sKeyboardDisplayKeyNames_5C9E30.keys[*idx - 1].field_0_name;
            }
        }
    }
    return ret;
}

bool Input_JoyStickEnabled()
{
    return sJoystickEnabled;
}

bool Input_JoyStickAvailable()
{
    return sJoystickAvailable;
}

void Input_SetJoyStickEnabled(bool enabled)
{
    sJoystickEnabled = enabled;
}

s32 Input_Remap_492680(InputCommands::Enum inputCmd)
{
    if (!Input_GetInputEnabled_4EDDE0())
    {
        return 0;
    }

    if (sJoystickEnabled)
    {
        f32 y2 = 0.0f;
        f32 x2 = 0.0f;
        f32 y1 = 0.0f;
        f32 x1 = 0.0f;
        BitField16<Flags_VKGamepad_Buttons_Pressed> buttonsPressed;
        Input_GetJoyState_460280(&x1, &y1, &x2, &y2, buttonsPressed);

        // Mask of buttons that exist for this controller
        const s32 allButtonsMask = (1 << sJoystickNumButtons_5C2EFC) - 1;

        const s32 buttonsToRebind = allButtonsMask & buttonsPressed.Raw().all;

        buttonsPressed.Raw().all &= allButtonsMask;
        if (buttonsPressed.Raw().all)
        {
            s32 bindIdx = 0;

            // get bindIdx from the mask
            for (bindIdx = 0; bindIdx < gamePadBindingsSize; bindIdx++)
            {
                if ((1 << bindIdx) & buttonsToRebind)
                {
                    break;
                }
            }

            // don't allow binding Speak I/II to any of the right-hand side action buttons
            if (inputCmd & (InputCommands::Enum::eLeftGameSpeak | InputCommands::Enum::eRightGameSpeak) && bindIdx < 4)
            {
                return 0;
            }

            // OG Bugfix:
            // Originally only the command that the key is being bound TO was reset.
            // By also resetting the command the key was bound FROM
            // (and only if it's a non-menu command), a situation can be
            // avoided where one key can be assigned to multiple commands.
            const u32 onlyNonMenuOriginalCommands = sGamePadBindings_5C98E0[bindIdx] & ~AllMenuCommandsMask;
            Input_ResetBinding_4925A0(inputCmd | onlyNonMenuOriginalCommands, 1);
            sGamePadBindings_5C98E0[bindIdx] |= inputCmd;
            Input_Init_Names_491870();
            return 2;
        }
    }

    // Find an "empty" key that is pressed
    s32 bindIdx = 0;
    while ((!sAllowedGameKeys_5C9D30[bindIdx] && bindIdx != VK_ESCAPE) || !Input_GetKeyState_4EDD20(bindIdx))
    {
        // Out of bounds
        if (++bindIdx >= 256)
        {
            return 0;
        }
    }

    // Out of bounds
    if (bindIdx >= 256)
    {
        return 0;
    }

    // Back can only be used to clear the binding, back it self can't be rebound
    if (bindIdx == VK_BACK)
    {
        Input_ResetBinding_4925A0(inputCmd, sJoystickEnabled);
        Input_Init_Names_491870();
        return -1;
    }

    // Escape can't be rebound
    if (bindIdx == VK_ESCAPE)
    {
        return -2;
    }

    if (sJoystickEnabled || !sAllowedGameKeys_5C9D30[bindIdx])
    {
        return 0;
    }

    // OG Bugfix:
    // Originally only the command that the key is being bound TO was reset.
    // By also resetting the command the key was bound FROM
    // (and only if it's a non-menu command), a situation can be
    // avoided where one key can be assigned to multiple commands.
    const u32 onlyNonMenuOriginalCommands = sKeyboardBindings_5C9930[bindIdx] & ~AllMenuCommandsMask;
    Input_ResetBinding_4925A0(inputCmd | onlyNonMenuOriginalCommands, 0);

    sKeyboardBindings_5C9930[bindIdx] |= inputCmd;
    Input_Init_Names_491870();
    return 1;
}

void Input_ResetBinding_4925A0(s32 input_command, s32 bIsGamePad)
{
    if (bIsGamePad)
    {
        for (s32 i = 0; i < gamePadBindingsSize; i++)
        {
            sGamePadBindings_5C98E0[i] &= ~input_command;
        }
    }
    else
    {
        for (s32 i = 0; i < 256; i++)
        {
            sKeyboardBindings_5C9930[i] &= ~input_command;
        }
    }
}

InputCommands::Enum Input_LoadSettingsIni_GetInputCommand_492B80(const char_type* pActionName)
{
    if (!_strcmpi(pActionName, "run"))
    {
        return InputCommands::Enum::eRun;
    }
    if (!_strcmpi(pActionName, "sneak"))
    {
        return InputCommands::Enum::eSneak;
    }
    if (!_strcmpi(pActionName, "jump"))
    {
        return InputCommands::Enum::eHop;
    }
    if (!_strcmpi(pActionName, "action"))
    {
        return InputCommands::Enum::eDoAction;
    }
    if (!_strcmpi(pActionName, "throw"))
    {
        return InputCommands::Enum::eThrowItem;
    }
    if (!_strcmpi(pActionName, "fart"))
    {
        return InputCommands::Enum::eFartOrRoll;
    }
    if (_strcmpi(pActionName, "speak1"))
    {
        return _strcmpi(pActionName, "speak2") != 0 ? static_cast<InputCommands::Enum>(0) : InputCommands::Enum::eRightGameSpeak;
    }
    return InputCommands::Enum::eLeftGameSpeak;
}

s32 Input_GetKeyboardKeyCode_492CA0(const char_type* keyName)
{
    for (s32 i = 0; i < 256; i++)
    {
        if (sKeyNames_5C9394[i] && !strcmpi(keyName, sKeyNames_5C9394[i]))
        {
            return i;
        }
    }

    return -1;
}

s32 Input_GetGamePadCode_492CF0(const char_type* buttonName)
{
    for (s32 i = 0; i < gamePadBindingsSize; i++)
    {
        if (sJoyButtonNames_5C9908[i] && !strcmpi(buttonName, sJoyButtonNames_5C9908[i]))
        {
            return i;
        }
    }

    return -1;
}

void Input_SetKeyboardBinding_493180(const char_type* pKeyName, s32 inputCommand)
{
    s32 keyCode = Input_GetKeyboardKeyCode_492CA0(pKeyName);
    if (keyCode >= 0)
    {
        Input_ResetBinding_4925A0(inputCommand, 0);
        sKeyboardBindings_5C9930[keyCode] = static_cast<InputCommands::Enum>(sKeyboardBindings_5C9930[keyCode] | inputCommand);
    }
}

void Input_SetGamePadBinding_4931D0(const char_type* pButtonName, s32 inputCommand)
{
    s32 gamePadCode = Input_GetGamePadCode_492CF0(pButtonName);
    Input_ResetBinding_4925A0(inputCommand, 1);
    if (gamePadCode >= 0)
    {
        sGamePadBindings_5C98E0[gamePadCode] = static_cast<InputCommands::Enum>(sGamePadBindings_5C98E0[gamePadCode] | inputCommand);
    }
}

std::vector<std::string> Ini_SplitParams(std::string line)
{
    auto paramSplit = string_util::SplitString(line, '=');
    for (u32 i = 0; i < paramSplit.size(); i++)
    {
        if (paramSplit[i].size() == 0)
        {
            continue;
        }

        while (paramSplit[i].size() && paramSplit[i][0] == ' ')
        {
            paramSplit[i] = paramSplit[i].substr(1, paramSplit[i].size() - 1);
        }
        while (paramSplit[i].size() && (paramSplit[i][paramSplit[i].size() - 1] == ' ' || paramSplit[i][paramSplit[i].size() - 1] == '\r'))
        {
            paramSplit[i] = paramSplit[i].substr(0, paramSplit[i].size() - 1);
        }
    }

    return paramSplit;
}

union intOrBoolPointer
{
    intOrBoolPointer(bool* pBool)
        : boolVal(pBool)
    { }
    intOrBoolPointer(s32* pInt)
        : intVal(pInt)
    { }

    s32* intVal;
    bool* boolVal;
};

struct IniCustomSaveEntry final
{
    const char_type* name;
    intOrBoolPointer data;
    bool isBool;
};

bool canOverwriteIni = true;
bool gLatencyHack = true;

const std::vector<IniCustomSaveEntry> gCustomSaveEntries = {
//    {"keep_aspect", {&s_VGA_KeepAspectRatio}, true}, todo
//    {"filter_screen", {&s_VGA_FilterScreen}, true}, todo
#if USE_SDL2_SOUND
    {"reverb", {&gReverbEnabled}, true},
    {"audio_stereo", {&gAudioStereo}, true},
#endif
    {"overwrite_ini_by_game", {&canOverwriteIni}, true},
    {"latency_hack", {&gLatencyHack}, true},
};

enum class IniCategory
{
    eNone = -1,
    eControl,
    eKeyboard,
    eGamepad,
    eAlive
};

const char_type* iniCategories[4] = {
    "Control",
    "Keyboard",
    "Gamepad",
    "Alive"
};

void NewParseSettingsIni()
{
    FileSystem fs;
    auto abeBuffer = fs.LoadToVec("abe2.ini");

    // Save the ini data to the recording or overwrite the data we read from
    // disk with the previously saved ini file data buffer that is in the recording
    // that we are playing back.
    // This prevents rebound keys/game pad input buttons etc from de-syncing the game
    // by changing the LED text size thus changing the number of rng calls and other annoyances.
    abeBuffer = GetGameAutoPlayer().RestoreFileBuffer(abeBuffer);

    const std::string abeConfig(reinterpret_cast<const char_type*>(abeBuffer.data()), abeBuffer.size());
    std::vector<std::string> configSplit = string_util::SplitString(abeConfig, '\n');

    IniCategory currentCategory = IniCategory::eNone;

    for (std::string& o : configSplit)
    {
        if (o.size() == 0) // Skip empty strings
        {
            continue;
        }

        if (o[0] == '[') // Category start
        {
            if (o[o.size() - 1] == '\r')
            {
                o = o.substr(0, o.size() - 1);
            }

            std::string category = o.substr(1, o.size() - 2);

            LOG_INFO("Ini category: %s", category.c_str());
            if (category == iniCategories[1])
            {
                currentCategory = IniCategory::eKeyboard;
                Input_ResetBinding_4925A0(InputCommands::Enum::eRun, 0);
                Input_ResetBinding_4925A0(InputCommands::Enum::eSneak, 0);
                Input_ResetBinding_4925A0(InputCommands::Enum::eHop, 0);
                Input_ResetBinding_4925A0(InputCommands::Enum::eDoAction, 0);
                Input_ResetBinding_4925A0(InputCommands::Enum::eThrowItem, 0);
                Input_ResetBinding_4925A0(InputCommands::Enum::eFartOrRoll, 0);
            }
            else if (category == iniCategories[2])
            {
                currentCategory = IniCategory::eGamepad;
                Input_ResetBinding_4925A0(InputCommands::Enum::eRun, 1);
                Input_ResetBinding_4925A0(InputCommands::Enum::eSneak, 1);
                Input_ResetBinding_4925A0(InputCommands::Enum::eHop, 1);
                Input_ResetBinding_4925A0(InputCommands::Enum::eDoAction, 1);
                Input_ResetBinding_4925A0(InputCommands::Enum::eThrowItem, 1);
                Input_ResetBinding_4925A0(InputCommands::Enum::eFartOrRoll, 1);
                Input_ResetBinding_4925A0(InputCommands::Enum::eLeftGameSpeak, 1);
                Input_ResetBinding_4925A0(InputCommands::Enum::eRightGameSpeak, 1);
            }
            else if (category == iniCategories[3])
            {
                currentCategory = IniCategory::eAlive;
            }
            else if (category == iniCategories[0])
            {
                currentCategory = IniCategory::eControl;
            }
            else
            {
                LOG_ERROR("Wrong INI category name! %s", category.c_str());
                currentCategory = IniCategory::eNone;
            }
        }
        else
        {
            std::vector<std::string> param = Ini_SplitParams(o);

            if (param.size() == 2)
            {
                LOG_INFO("Value: %s = %s", param[0].c_str(), param[1].c_str());

                if (currentCategory == IniCategory::eControl)
                {
                    if (param[0] == "controller" && param[1] == "Gamepad")
                    {
                        sJoystickEnabled = true;
                    }
                    else if (param[0] == "controller" && param[1] == "Keyboard")
                    {
                        sJoystickEnabled = false;
                    }
                }
                else if (currentCategory == IniCategory::eKeyboard)
                {
                    InputCommands::Enum kbInputCommand = Input_LoadSettingsIni_GetInputCommand_492B80(param[0].c_str());
                    Input_SetKeyboardBinding_493180(param[1].c_str(), kbInputCommand);
                }
                else if (currentCategory == IniCategory::eGamepad)
                {
                    InputCommands::Enum gamepadInputCommand = Input_LoadSettingsIni_GetInputCommand_492B80(param[0].c_str());
                    Input_SetGamePadBinding_4931D0(param[1].c_str(), gamepadInputCommand);
                }
                else if (currentCategory == IniCategory::eAlive)
                {
                    for (const IniCustomSaveEntry& s : gCustomSaveEntries)
                    {
                        if (param[0] == s.name)
                        {
                            if (s.isBool)
                            {
                                *s.data.boolVal = param[1] == "true";
                            }
                            else // s32
                            {
                                *s.data.intVal = atoi(param[1].c_str());
                            }

                            break;
                        }
                    }
                }
            }
        }
    }
}

void Input_SaveSettingsIni_Common()
{
    if (!canOverwriteIni)
    {
        return;
    }

    s32 prevJoyState = sJoystickEnabled;

    std::stringstream output;

    // Control remap
    output << "[" << iniCategories[0] << "]"
           << "\n";

    if (sJoystickEnabled)
    {
        if (sJoystickEnabled)
        {
            output << "controller = Gamepad\n";
        }
    }
    else
    {
        output << "controller = Keyboard\n";
    }

    output << "\n";

    sJoystickEnabled = false;

    // Keyboard remap
    output << "[" << iniCategories[1] << "]"
           << "\n";

    const char_type* btnString = nullptr;

    btnString = Input_GetButtonString_492530(kRun, 0);
    if (btnString)
    {
        output << "run = " << btnString << "\n";
    }
    btnString = Input_GetButtonString_492530(kSneak, 0);
    if (btnString)
    {
        output << "sneak = " << btnString << "\n";
    }
    btnString = Input_GetButtonString_492530(kJump, 0);
    if (btnString)
    {
        output << "jump = " << btnString << "\n";
    }
    btnString = Input_GetButtonString_492530(kAction, 0);
    if (btnString)
    {
        output << "action = " << btnString << "\n";
    }
    btnString = Input_GetButtonString_492530(kThrow, 0);
    if (btnString)
    {
        output << "throw = " << btnString << "\n";
    }
    btnString = Input_GetButtonString_492530(kFart, 0);
    if (btnString)
    {
        output << "fart = " << btnString << "\n";
    }

    sJoystickEnabled = true;

    output << "\n";

    // Gamepad remap
    output << "[" << iniCategories[2] << "]"
           << "\n";
    output << "buttons = " << sJoystickNumButtons_5C2EFC << "\n";

    btnString = Input_GetButtonString_492530(kRun, 1);
    if (btnString)
    {
        output << "run = " << btnString << "\n";
    }

    btnString = Input_GetButtonString_492530(kSneak, 1);
    if (btnString)
    {
        output << "sneak = " << btnString << "\n";
    }

    btnString = Input_GetButtonString_492530(kJump, 1);
    if (btnString)
    {
        output << "jump = " << btnString << "\n";
    }

    btnString = Input_GetButtonString_492530(kAction, 1);
    if (btnString)
    {
        output << "action = " << btnString << "\n";
    }

    btnString = Input_GetButtonString_492530(kThrow, 1);
    if (btnString)
    {
        output << "throw = " << btnString << "\n";
    }

    btnString = Input_GetButtonString_492530(kFart, 1);
    if (btnString)
    {
        output << "fart = " << btnString << "\n";
    }

    btnString = Input_GetButtonString_492530(kSpeak1, 1);
    if (btnString)
    {
        output << "speak1 = " << btnString << "\n";
    }

    btnString = Input_GetButtonString_492530(kSpeak2, 1);
    if (btnString)
    {
        output << "speak2 = " << btnString << "\n";
    }

    sJoystickEnabled = prevJoyState;

    output << "\n";

    // New Renderer Options
    output << "[" << iniCategories[3] << "]"
           << "\n";

    for (const auto& s : gCustomSaveEntries)
    {
        if (s.isBool)
        {
            output << s.name << " = " << (*s.data.boolVal ? "true" : "false") << "\n";
        }
        else
        {
            output << s.name << " = " << *s.data.intVal << "\n";
        }
    }

    FileSystem fs;
    FileSystem::Path iniPath;
    iniPath.Append("abe2.ini");
    const auto iniDataStr = output.str();
    std::vector<u8> buf(iniDataStr.begin(), iniDataStr.end());
    if (!fs.Save(iniPath, buf))
    {
        ALIVE_FATAL("Saving abe2.ini failed");
    }

    Input_Init_Names_491870();
}

void Input_SaveSettingsIni_492840()
{
    Input_SaveSettingsIni_Common();
}

void Input_LoadSettingsIni_492D40()
{
    NewParseSettingsIni();
}

u8 sFlags_5C9390 = 0;
u32 sPrevTimeStamp_5C98D8 = 0;
u32 dword_5C98DC = 0;
u32 sPrevious_down_keyboard_keys_5C9F74 = 0;
u32 dword_5C9F78 = 0;

#if ORIGINAL_PS1_BEHAVIOR // OG Change - chant with any shoulder button combo
static bool IsChantingAnyShoulderButton(s32 shoulderButtonsPressedCount)
{
    return (shoulderButtonsPressedCount > 1);
}
#else
static bool IsChanting(s8 input_command_c_pressed, s8 input_command_delete_pressed)
{
    return (input_command_c_pressed && input_command_delete_pressed);
}
#endif

// Temp Hax. Todo: fix up
s32 Input_Convert_KeyboardGamePadInput_To_Internal_Format_492150()
{
    u32 timeStamp;                   // eax
    u32 currentTime;                 // eax

    if (!Input_GetInputEnabled_4EDDE0())
    {
        return 0;
    }

    s32 converted_input = 0;
    s32 pressedInputCommands = 0;

    if (sFlags_5C9390 & 1)
    {
        timeStamp = sPrevTimeStamp_5C98D8;
    }
    else
    {
        sFlags_5C9390 |= 1u;
        timeStamp = SYS_GetTicks() - 55;
        sPrevTimeStamp_5C98D8 = timeStamp;
    }

    if (!(sFlags_5C9390 & 2))
    {
        sFlags_5C9390 |= 2u;
        dword_5C98DC = timeStamp - 55;
    }

    f32 pX1 = 0.0f;
    f32 pY1 = 0.0f;
    f32 pY2 = 0.0f;
    f32 pX2 = 0.0f;

    if (Sys_IsAppActive())
    {
        for (s32 i = 0; i < 256; i++)
        {
            if (sKeyboardBindings_5C9930[i])
            {
                if (Input_IsVKPressed_4EDD40(i))
                {
                    pressedInputCommands |= sKeyboardBindings_5C9930[i];
                }
            }
        }

        if (sJoystickEnabled)
        {
            BitField16<Flags_VKGamepad_Buttons_Pressed> gamepadButtonsPressed;
            Input_GetJoyState_460280(&pX1, &pY1, &pX2, &pY2, gamepadButtonsPressed);

            if ((sGamepadCapFlags_5C2EF8 & eDisableAutoRun) == 1 && sJoystickNumButtons_5C2EFC <= 4 && fabs(pX1) >= 0.75f) // Auto sprint
            {
                pressedInputCommands |= InputCommands::Enum::eRun;
            }

            float axisDeadZoneThreshold = 0.050000001f;
            if (pX1 > axisDeadZoneThreshold)
            {
                pressedInputCommands |= InputCommands::Enum::eRight;
            }
            else if(pX1 < -axisDeadZoneThreshold)
            {
                pressedInputCommands |= InputCommands::Enum::eLeft;
            }
            if (pY1 > axisDeadZoneThreshold)
            {
                pressedInputCommands |= InputCommands::Enum::eDown;
            }
            else if(pY1 < -axisDeadZoneThreshold)
            {
                pressedInputCommands |= InputCommands::Enum::eUp;
            }

            if (!(pressedInputCommands & (InputCommands::Enum::eRight | InputCommands::Enum::eLeft | InputCommands::Enum::eDown | InputCommands::Enum::eUp)))
            {
                s32 shoulderButtonsPressedCount = 0;

                bool isGameSpeakLeftPressed = false;
                bool isGameSpeakRightPressed = false;
                for (s32 i = 0; i < gamePadBindingsSize; i++)
                {
                    if (sGamePadBindings_5C98E0[i] & InputCommands::Enum::eLeftGameSpeak)
                    {
                        if (gamepadButtonsPressed.Get(static_cast<Flags_VKGamepad_Buttons_Pressed>( 1 << i )))
                        {
                            isGameSpeakLeftPressed = true;
                            ++shoulderButtonsPressedCount;
                        }
                    }

                    if (sGamePadBindings_5C98E0[i] & InputCommands::Enum::eRightGameSpeak)
                    {
                        if (gamepadButtonsPressed.Get(static_cast<Flags_VKGamepad_Buttons_Pressed>( 1 << i )))
                        {
                            isGameSpeakRightPressed = true;
                            ++shoulderButtonsPressedCount;
                        }
                    }

                    if (sGamePadBindings_5C98E0[i] & InputCommands::Enum::eRun)
                    {
                        if (gamepadButtonsPressed.Get(static_cast<Flags_VKGamepad_Buttons_Pressed>( 1 << i )))
                        {
                            ++shoulderButtonsPressedCount;
                        }
                    }

                    if (sGamePadBindings_5C98E0[i] & InputCommands::Enum::eSneak)
                    {
                        if (gamepadButtonsPressed.Get(static_cast<Flags_VKGamepad_Buttons_Pressed>( 1 << i )))
                        {
                            ++shoulderButtonsPressedCount;
                        }
                    }
                }

    #if ORIGINAL_PS1_BEHAVIOR // OG Change - chant with any shoulder button combo
                const bool isChanting = IsChantingAnyShoulderButton(shoulderButtonsPressedCount);
    #else
                const bool isChanting = IsChanting(isGameSpeakLeftPressed, isGameSpeakRightPressed);
    #endif

                if (isChanting)
                {
                    pressedInputCommands |= InputCommands::Enum::eChant;
                }
                else if(isGameSpeakLeftPressed || isGameSpeakRightPressed)
                {
                    // gamepadButtonsPressed.Clear(Flags_VKGamepad_Buttons_Pressed::eBit1_face_btn_left);
                    // gamepadButtonsPressed.Clear(Flags_VKGamepad_Buttons_Pressed::eBit2_face_btn_bottom);
                    // gamepadButtonsPressed.Clear(Flags_VKGamepad_Buttons_Pressed::eBit3_face_btn_right);
                    // gamepadButtonsPressed.Clear(Flags_VKGamepad_Buttons_Pressed::eBit4_face_btn_top);
                }
            }

            for (int i = 0; i < gamePadBindingsSize; i++)
            {
                if (sGamePadBindings_5C98E0[i])
                {
                    if (gamepadButtonsPressed.Get(static_cast<Flags_VKGamepad_Buttons_Pressed>( 1 << i )))
                    {
                        pressedInputCommands |= sGamePadBindings_5C98E0[i];
                    }
                }
            }

            if (sJoystickNumButtons_5C2EFC <= 6)
            {
                if (!(sGamepadCapFlags_5C2EF8 & eDisableAutoRun))
                {
                    if ((pressedInputCommands ^ sPrevious_down_keyboard_keys_5C9F74) & (InputCommands::Enum::eRight | InputCommands::Enum::eLeft))
                    {
                        dword_5C9F78 = sGamepadCapFlags_5C2EF8 & eDisableAutoRun;
                        if (!(sPrevious_down_keyboard_keys_5C9F74 & (InputCommands::Enum::eRight | InputCommands::Enum::eLeft)))
                        {
                            currentTime = SYS_GetTicks();
                            if ((u32)(sPrevTimeStamp_5C98D8 - dword_5C98DC) <= 220 && currentTime - sPrevTimeStamp_5C98D8 <= 220)
                            {
                                dword_5C9F78 = 1;
                            }
                            dword_5C98DC = currentTime;
                        }
                        if (!(pressedInputCommands & (InputCommands::Enum::eRight | InputCommands::Enum::eLeft)))
                        {
                            sPrevTimeStamp_5C98D8 = SYS_GetTicks();
                        }
                    }
                }
                if (dword_5C9F78)
                {
                    pressedInputCommands |= InputCommands::Enum::eRun;
                }
            }
        }

        // If pressing up and down at same time turn off
        if ((pressedInputCommands & (InputCommands::Enum::eDown | InputCommands::Enum::eUp)) == (InputCommands::Enum::eDown | InputCommands::Enum::eUp))
        {
            pressedInputCommands &= ~(InputCommands::Enum::eDown | InputCommands::Enum::eUp);
        }

        // If pressing left and right at same time turn off
        if ((pressedInputCommands & (InputCommands::Enum::eRight | InputCommands::Enum::eLeft)) == (InputCommands::Enum::eRight | InputCommands::Enum::eLeft))
        {
            pressedInputCommands &= ~(InputCommands::Enum::eRight | InputCommands::Enum::eLeft);
        }

        converted_input = pressedInputCommands;
    }
    sPrevious_down_keyboard_keys_5C9F74 = pressedInputCommands;
    return converted_input;
}

bool Input_IsGameSpeakPressed(InputCommands::Enum gameSpeakId)
{
    auto held = Input().GetHeld();
    auto pressed = Input().GetPressed();
    bool correctSpeakBtnHeld = false;
    if (gameSpeakId >= InputCommands::Enum::eGameSpeak1 && gameSpeakId <= InputCommands::Enum::eGameSpeak4)
    {
        correctSpeakBtnHeld = held & InputCommands::Enum::eLeftGameSpeak;
    }
    else if (gameSpeakId >= InputCommands::Enum::eGameSpeak5 && gameSpeakId <= InputCommands::Enum::eGameSpeak8)
    {
        correctSpeakBtnHeld = held & InputCommands::Enum::eRightGameSpeak;
    }

    if(correctSpeakBtnHeld)
    {
        switch(gameSpeakId)
        {
            case InputCommands::Enum::eGameSpeak1:
            {
                return pressed & InputCommands::Enum::eHop;
                break;
            }
            case InputCommands::Enum::eGameSpeak2:
            {
                return pressed & InputCommands::Enum::eDoAction;
                break;
            }
            case InputCommands::Enum::eGameSpeak3:
            {
                return pressed & InputCommands::Enum::eFartOrRoll;
                break;
            }
            case InputCommands::Enum::eGameSpeak4:
            {
                return pressed & InputCommands::Enum::eThrowItem;
                break;
            }
            case InputCommands::Enum::eGameSpeak5:
            {
                return pressed & InputCommands::Enum::eFartOrRoll;
                break;
            }
            case InputCommands::Enum::eGameSpeak6:
            {
                return pressed & InputCommands::Enum::eHop;
                break;
            }
            case InputCommands::Enum::eGameSpeak7:
            {
                return pressed & InputCommands::Enum::eThrowItem;
                break;
            }
            case InputCommands::Enum::eGameSpeak8:
            {
                return pressed & InputCommands::Enum::eDoAction;
                break;
            }
            default: break;
        }
    }
    return false;
}

void Input_SetCallback_4FA910(t_InputCallback pFunc)
{
    sInputCallbackFunc_BD1870 = pFunc;
}

void Input_45FDF0(f32 x, f32 y, s32 a3, bool cap_has_r)
{
    if (sJoystickNumButtons_5C2EFC == 4 && (a3 || cap_has_r))
    {
        bAbsX_5C2EE8 = 0;
        bAbsY_5C2EA4 = 0;
        bAutoX_5C2EE4 = 0;
        bAutoY_5C2EF0 = 0;
        if (a3)
        {
            bAutoX_5C2EE4 = 1;
            sJoystickNumButtons_5C2EFC = 5;
            if (x <= -0.25f || x >= 0.25f)
            {
                bAbsX_5C2EE8 = 1;
                if (x <= 0.0f)
                {
                    bAbsX_5C2EE8 = 0;
                }
            }
            else
            {
                sJoystickNumButtons_5C2EFC = 6;
                bAutoX_5C2EE4 = 2;
            }
        }
        if (cap_has_r)
        {
            bAutoY_5C2EF0 = 1;
            ++sJoystickNumButtons_5C2EFC;
            if (y <= -0.25f || y >= 0.25f)
            {
                bAbsY_5C2EA4 = 1;
                if (y <= 0.0f)
                {
                    bAbsY_5C2EA4 = 0;
                }
            }
            else
            {
                bAutoY_5C2EF0 = 2;
                ++sJoystickNumButtons_5C2EFC;
            }
        }
    }
}

void Input_InitJoyStick_460080()
{
    // Added because sometimes joyGetDevCapsA hangs on Win10 1809.
    // Not too worried about this given all of this will be replaced with SDL2 at some point.
    TRACE_ENTRYEXIT;

    SDL_GameControllerAddMappingsFromFile("gamecontrollerdb.txt");

#if MOBILE
    gTouchController->Init();
#endif

    sJoystickAvailable = false;

    sGamepadCapFlags_5C2EF8 |= eDisableAutoRun;
 
    LOG_INFO("SDL_NumJoysticks: %d", SDL_NumJoysticks());
    for (s32 i = 0; i < SDL_NumJoysticks(); i++)
    {
        if (SDL_IsGameController(i))
        {
            pSDLController = SDL_GameControllerOpen(i); // TODO: SDL_GameControllerClose is never called on this
            if (pSDLController)
            {
                LOG_INFO("Controller name is %s", SDL_GameControllerName(pSDLController));

                sJoystickAvailable = true;

                strncpy(sGamePadStr_55E85C, SDL_GameControllerName(pSDLController), 32u);

                //TODO add binding
                break;
            }
            else
            {
                LOG_ERROR("Could not open SDL GamePad %d %s", i, SDL_GetError());
            }
        }
        else
        {
            LOG_INFO("Item %d is not a game controller", i);
        }
    }
}

void Input_Init()
{
    // This seems so stupid
    strncpy(sGamePadStr_555708, sGamePadStr_55E85C, 32u);
    strncpy(sGamePadStr_55E85C, sGamePadStr_555708, 32u);

    memset(sKeyNames_5C9394, 0, 1024);
    memset(sAllowedGameKeys_5C9D30, 0, 256);

    sAllowedGameKeys_5C9D30[VK_SHIFT] = 1;
    sAllowedGameKeys_5C9D30[VK_CONTROL] = 1;
    sAllowedGameKeys_5C9D30[VK_MENU] = 1;
    sAllowedGameKeys_5C9D30[VK_BACK] = 1;
    sAllowedGameKeys_5C9D30[VK_INSERT] = 1;
    sAllowedGameKeys_5C9D30[VK_DELETE] = 1;
    sAllowedGameKeys_5C9D30[VK_HOME] = 1;
    sAllowedGameKeys_5C9D30[VK_END] = 1;
    sAllowedGameKeys_5C9D30[VK_PRIOR] = 1;
    sAllowedGameKeys_5C9D30[VK_NEXT] = 1;
    sAllowedGameKeys_5C9D30[VK_SPACE] = 1;
    sAllowedGameKeys_5C9D30['A'] = 1;
    sAllowedGameKeys_5C9D30['B'] = 1;
    sAllowedGameKeys_5C9D30['C'] = 1;
    sAllowedGameKeys_5C9D30['D'] = 1;
    sAllowedGameKeys_5C9D30['E'] = 1;
    sAllowedGameKeys_5C9D30['F'] = 1;
    sAllowedGameKeys_5C9D30['G'] = 1;
    sAllowedGameKeys_5C9D30['H'] = 1;
    sAllowedGameKeys_5C9D30['I'] = 1;
    sAllowedGameKeys_5C9D30['J'] = 1;
    sAllowedGameKeys_5C9D30['K'] = 1;
    sAllowedGameKeys_5C9D30['L'] = 1;
    sAllowedGameKeys_5C9D30['M'] = 1;
    sAllowedGameKeys_5C9D30['N'] = 1;
    sAllowedGameKeys_5C9D30['O'] = 1;
    sAllowedGameKeys_5C9D30['P'] = 1;
    sAllowedGameKeys_5C9D30['Q'] = 1;
    sAllowedGameKeys_5C9D30['R'] = 1;
    sAllowedGameKeys_5C9D30['S'] = 1;
    sAllowedGameKeys_5C9D30['T'] = 1;
    sAllowedGameKeys_5C9D30['U'] = 1;
    sAllowedGameKeys_5C9D30['V'] = 1;
    sAllowedGameKeys_5C9D30['W'] = 1;
    sAllowedGameKeys_5C9D30['X'] = 1;
    sAllowedGameKeys_5C9D30['Y'] = 1;
    sAllowedGameKeys_5C9D30['Z'] = 1;
    sAllowedGameKeys_5C9D30['9'] = 1;
    sAllowedGameKeys_5C9D30[VK_SEPARATOR] = 1;
    sAllowedGameKeys_5C9D30[VK_DECIMAL] = 1;
    sAllowedGameKeys_5C9D30[0] = 1;

    sKeyNames_5C9394[VK_UP] = kAllYa;
    sKeyNames_5C9394[VK_DOWN] = kSorry;
    sKeyNames_5C9394[VK_LEFT] = kStopIt;
    sKeyNames_5C9394[VK_RIGHT] = kChant;
    sKeyNames_5C9394[VK_SHIFT] = "shift";
    sKeyNames_5C9394[VK_CONTROL] = "ctrl";
    sKeyNames_5C9394[VK_RETURN] = "enter";
    sKeyNames_5C9394[VK_ESCAPE] = "esc";
    sKeyNames_5C9394[VK_BACK] = "bkspc";
    sKeyNames_5C9394[VK_INSERT] = "ins";
    sKeyNames_5C9394[VK_DELETE] = "del";
    sKeyNames_5C9394[VK_HOME] = "home";
    sKeyNames_5C9394[VK_END] = "end";
    sKeyNames_5C9394[VK_PRIOR] = "pgup";
    sKeyNames_5C9394[VK_NEXT] = "pgdn";
    sKeyNames_5C9394[VK_SPACE] = "space";
    sKeyNames_5C9394[VK_MENU] = "alt";
    sKeyNames_5C9394[VK_TAB] = "tab";
    sKeyNames_5C9394['A'] = "A";
    sKeyNames_5C9394['B'] = "B";
    sKeyNames_5C9394['C'] = "C";
    sKeyNames_5C9394['D'] = "D";
    sKeyNames_5C9394['E'] = "E";
    sKeyNames_5C9394['F'] = "F";
    sKeyNames_5C9394['G'] = "G";
    sKeyNames_5C9394['H'] = "H";
    sKeyNames_5C9394['I'] = "I";
    sKeyNames_5C9394['J'] = "J";
    sKeyNames_5C9394['K'] = "K";
    sKeyNames_5C9394['L'] = "L";
    sKeyNames_5C9394['M'] = "M";
    sKeyNames_5C9394['N'] = "N";
    sKeyNames_5C9394['O'] = "O";
    sKeyNames_5C9394['P'] = "P";
    sKeyNames_5C9394['Q'] = "Q";
    sKeyNames_5C9394['R'] = "R";
    sKeyNames_5C9394['S'] = "S";
    sKeyNames_5C9394['T'] = "T";
    sKeyNames_5C9394['U'] = "U";
    sKeyNames_5C9394['V'] = "V";
    sKeyNames_5C9394['W'] = "W";
    sKeyNames_5C9394['X'] = "X";
    sKeyNames_5C9394['Y'] = "Y";
    sKeyNames_5C9394['Z'] = "Z";
    sKeyNames_5C9394['0'] = "0";
    sKeyNames_5C9394['1'] = "1";
    sKeyNames_5C9394['2'] = "2";
    sKeyNames_5C9394['3'] = "3";
    sKeyNames_5C9394['4'] = "4";
    sKeyNames_5C9394['5'] = "5";
    sKeyNames_5C9394['6'] = "6";
    sKeyNames_5C9394['7'] = "7";
    sKeyNames_5C9394['8'] = "8";
    sKeyNames_5C9394['9'] = "9";
    sKeyNames_5C9394[VK_SEPARATOR] = ",";
    sKeyNames_5C9394[VK_DECIMAL] = ".";
    sKeyNames_5C9394[0] = "\\";
    sKeyNames_5C9394[VK_NUMPAD0] = "0";
    sKeyNames_5C9394[VK_NUMPAD1] = "1";
    sKeyNames_5C9394[VK_NUMPAD2] = "2";
    sKeyNames_5C9394[VK_NUMPAD3] = "3";
    sKeyNames_5C9394[VK_NUMPAD4] = "4";
    sKeyNames_5C9394[VK_NUMPAD5] = "5";
    sKeyNames_5C9394[VK_NUMPAD6] = "6";
    sKeyNames_5C9394[VK_NUMPAD7] = "7";
    sKeyNames_5C9394[VK_NUMPAD8] = "8";
    sKeyNames_5C9394[VK_NUMPAD9] = "9";

//TODO Mlg hax
    sJoyButtonNames_5C9908[0] = "X";
    sJoyButtonNames_5C9908[1] = "A";
    sJoyButtonNames_5C9908[2] = "B";
    sJoyButtonNames_5C9908[3] = "Y";
    sJoyButtonNames_5C9908[4] = "LB";
    sJoyButtonNames_5C9908[5] = "RB";
    sJoyButtonNames_5C9908[6] = "LT";
    sJoyButtonNames_5C9908[7] = "RT";
    sJoyButtonNames_5C9908[8] = "BACK";
    sJoyButtonNames_5C9908[9] = "START";


    Input_InitJoyStick_460080();
    memset(sKeyboardBindings_5C9930, 0, sizeof(*sKeyboardBindings_5C9930) * 256);

    for (const auto* kb = sDefaultKeyboardBindings_55EAD8; kb->key; kb++)
    {
        sKeyboardBindings_5C9930[kb->key] = kb->command;
    }

    for (int i = 0; i < gamePadBindingsSize; i++)
    {
        const auto kb = sDefaultGamepadBindings_55EA2C[i];
        sGamePadBindings_5C98E0[kb.key] = kb.command;
    }

    Input_LoadSettingsIni_492D40();
    Input_Init_Names_491870();
    Input_SetCallback_4FA910(Input_Convert_KeyboardGamePadInput_To_Internal_Format_492150);
}

u32 Input_GetLastPressedKey_492610()
{
    if (!Sys_IsAnyKeyDown())
    {
        return 0;
    }

    const u32 lastVKeyPressed = sLastPressedKey;

    sIsAKeyDown = false;
    sLastPressedKey = 0;

    return lastVKeyPressed;
}

void Input_Reset_492660()
{
    Input_EnableInput_4EDDD0();
    Input_InitKeyStateArray_4EDD60();
}

u32 Input_IsChanting()
{
    return (Input().mPads[0].mRawInput & InputCommands::Enum::eChant) == InputCommands::Enum::eChant;
}

// Zeros the input key state array.
void Input_InitKeyStateArray_4EDD60()
{
    memset(sInputKeyStates_BD2F60, 0, 256u);
}

void Input_SetKeyState(s32 key, s8 bIsDown)
{
    if (bIsDown)
    {
        sInputKeyStates_BD2F60[key] = 0x81;
    }
    else
    {
        sInputKeyStates_BD2F60[key] &= 1u;
    }
}

// Returns true if a key was just pressed down.
bool Input_IsVKPressed_4EDD40(s32 key)
{
    const auto keyState = sInputKeyStates_BD2F60[key];

    if (!keyState)
    {
        return false;
    }

    sInputKeyStates_BD2F60[key] = keyState & 0x80;

    return true;
}

s32 Input_Read_Pad(s32 padNum)
{
    if (!sReadPadEnable_BD1874 || (padNum != 0) != (Input_GetKeyState_4EDD20(VK_F2) != 0))
    {
        return 0;
    }

    if (sInputCallbackFunc_BD1870)
    {
        sLastPad_Input_BD1878 = sInputCallbackFunc_BD1870(); // usually pointer to Input_Convert_KeyboardGamePadInput_To_Internal_Format_492150
    }

#if MOBILE
    gTouchController->Update();
#endif

    return sLastPad_Input_BD1878;
}


InputObject& Input()
{
    return sInputObject;
}

void Input_Pads_Reset_4FA960()
{
    sLastPad_Input_BD1878 = 0;
    sReadPadEnable_BD1874 = true;
}

s32 InputObject::Is_Demo_Playing_45F220()
{
    return mbDemoPlaying & 1;
}

void InputObject::UnsetDemoPlaying_45F240()
{
    mbDemoPlaying &= ~1;
}

void InputObject::SetDemoResource_45F1E0(u32** pDemoRes)
{
    mDemoCommandIndex = 2;
    mpDemoRes = pDemoRes;
    mbDemoPlaying |= 1u;
    mCommandDuration = 0;
}
#include <assert.h>

// BC break of Update_45F040
void InputObject::Update(BaseGameAutoPlayer& autoPlayer)
{
    const u8 directionTable_545A4C[20] = {
        0,
        64,  // up
        192, // down
        0,
        128, // left
        96,
        160,
        128,
        0, // right
        32,
        224,
        0,
        0,
        64,
        192,
        0,
        0,
        0,
        0,
        0};

    mPads[0].mPreviousInput = mPads[0].mRawInput;
    mPads[0].mRawInput = autoPlayer.GetInput(0);

    if (Is_Demo_Playing_45F220())
    {
        // Stop if any button on any pad is pressed
        if (mPads[sCurrentControllerIndex].mRawInput)
        {
            bLongerTimeoutToNextDemo = 0;
            UnsetDemoPlaying_45F240();
            return;
        }

        if (sGnFrame >= mCommandDuration)
        {
            const u32 command = (*mpDemoRes)[mDemoCommandIndex++];
            mCommand = command >> 16;
            mCommandDuration = sGnFrame + (command & 0xFFFF);

            // End demo/quit command
            if (command & 0x8000)
            {
                UnsetDemoPlaying_45F240();
            }
        }

        // Will do nothing if we hit the end command..
        if (Is_Demo_Playing_45F220())
        {
            mPads[0].mRawInput = PsxButtonsToKeyboardInput_45EE40(mCommand);
        }
    }

    mPads[0].mReleased = mPads[0].mPreviousInput & ~mPads[0].mRawInput;
    mPads[0].mPressed = mPads[0].mRawInput & ~mPads[0].mPreviousInput;
    mPads[0].mDir = directionTable_545A4C[mPads[0].mRawInput & 0xF];

    mPads[1].mPreviousInput = mPads[1].mRawInput;
    mPads[1].mRawInput = autoPlayer.GetInput(1);
    mPads[1].mReleased = mPads[1].mPreviousInput & ~mPads[1].mRawInput;
    mPads[1].mPressed = mPads[1].mRawInput & ~mPads[1].mPreviousInput;
    mPads[1].mDir = directionTable_545A4C[mPads[1].mRawInput & 0xF];
}

u32 InputObject::PsxButtonsToKeyboardInput_45EE40(u32 cmd)
{
    u32 shoulderButtonsPressedCount = 0;

    if (cmd & PsxButtonBits::eL2)
    {
        ++shoulderButtonsPressedCount;
    }

    if (cmd & PsxButtonBits::eR2)
    {
        ++shoulderButtonsPressedCount;
    }

    if (cmd & PsxButtonBits::eL1)
    {
        ++shoulderButtonsPressedCount;
    }

    if (cmd & PsxButtonBits::eR1)
    {
        ++shoulderButtonsPressedCount;
    }

    if (shoulderButtonsPressedCount > 1) // Any 2 shoulder button combo = chanting
    {
        return InputCommands::Enum::eChant;
    }

    u32 rawInput = 0;
    if (cmd & PsxButtonBits::eDPadUp)
    {
        rawInput |= InputCommands::Enum::eUp;
    }

    if (cmd & PsxButtonBits::eDPadRight)
    {
        rawInput |= InputCommands::Enum::eRight;
    }

    if (cmd & PsxButtonBits::eDPadDown)
    {
        rawInput |= InputCommands::Enum::eDown;
    }

    if (cmd & PsxButtonBits::eDPadLeft)
    {
        rawInput |= InputCommands::Enum::eLeft;
    }

    if (cmd & PsxButtonBits::eR1)
    {
        rawInput |= InputCommands::Enum::eRun;
    }

    if (cmd & PsxButtonBits::eR2)
    {
        rawInput |= InputCommands::Enum::eSneak;
    }

    if (cmd & PsxButtonBits::eL1)
    {
        rawInput |= InputCommands::Enum::eLeftGameSpeak;

        if (cmd & PsxButtonBits::eTriangle)
        {
            rawInput |= InputCommands::Enum::eGameSpeak1;
        }

        if (cmd & PsxButtonBits::eCircle)
        {
            rawInput |= InputCommands::Enum::eGameSpeak4;
        }

        if (cmd & PsxButtonBits::eCross)
        {
            rawInput |= InputCommands::Enum::eGameSpeak3;
        }

        if (cmd & PsxButtonBits::eSquare)
        {
            rawInput |= InputCommands::Enum::eGameSpeak2;
        }
    }
    else if (cmd & PsxButtonBits::eL2)
    {
        rawInput |= InputCommands::Enum::eRightGameSpeak;

        if (cmd & PsxButtonBits::eTriangle)
        {
            rawInput |= InputCommands::Enum::eGameSpeak6;
        }

        if (cmd & PsxButtonBits::eCircle)
        {
            rawInput |= InputCommands::Enum::eGameSpeak7;
        }

        if (cmd & PsxButtonBits::eCross)
        {
            rawInput |= InputCommands::Enum::eGameSpeak5;
        }

        if (cmd & PsxButtonBits::eSquare)
        {
            rawInput |= InputCommands::Enum::eGameSpeak8;
        }
    }
    else // No shoulder buttons
    {
        if (cmd & PsxButtonBits::eTriangle)
        {
            rawInput |= InputCommands::Enum::eHop;
        }

        if (cmd & PsxButtonBits::eCircle)
        {
            rawInput |= InputCommands::Enum::eThrowItem;
        }

        if (cmd & PsxButtonBits::eCross)
        {
            rawInput |= InputCommands::Enum::eFartOrRoll;
        }

        if (cmd & PsxButtonBits::eSquare)
        {
            rawInput |= InputCommands::Enum::eDoAction;
        }
    }

    return rawInput;
}

// apparently we don't convert movement and some other inputs due to the
// OG function return size limit
s8 InputObject::KeyboardInputToPsxButtons_45EF70(s32 cmd)
{
    s8 result = 0;
    if (cmd & 1)
    {
        result = 0;
    }

    if (cmd & InputCommands::Enum::eChant)
    {
        result |= PsxButtonBits::eL1 + PsxButtonBits::eL2;
    }

    if (cmd & InputCommands::Enum::eHop)
    {
        result |= PsxButtonBits::eTriangle;
    }

    if (cmd & InputCommands::Enum::eThrowItem)
    {
        result |= PsxButtonBits::eCircle;
    }

    if (cmd & InputCommands::Enum::eFartOrRoll)
    {
        result |= PsxButtonBits::eCross;
    }

    if (cmd & InputCommands::Enum::eDoAction)
    {
        result |= PsxButtonBits::eSquare;
    }

    if (cmd & InputCommands::Enum::eGameSpeak1)
    {
        result |= PsxButtonBits::eL1 + PsxButtonBits::eTriangle;
    }

    if (cmd & InputCommands::Enum::eGameSpeak2)
    {
        result |= PsxButtonBits::eL1 + PsxButtonBits::eSquare;
    }

    if (cmd & InputCommands::Enum::eGameSpeak3)
    {
        result |= PsxButtonBits::eL1 + PsxButtonBits::eCross;
    }

    if (cmd & InputCommands::Enum::eGameSpeak4)
    {
        result |= PsxButtonBits::eL1 + PsxButtonBits::eCircle;
    }

    if (cmd & InputCommands::Enum::eGameSpeak5)
    {
        result |= PsxButtonBits::eL2 + PsxButtonBits::eCross;
    }

    if (cmd & InputCommands::Enum::eGameSpeak6)
    {
        result |= PsxButtonBits::eL2 + PsxButtonBits::eTriangle;
    }

    if (cmd & InputCommands::Enum::eGameSpeak7)
    {
        result |= PsxButtonBits::eL2 + PsxButtonBits::eCircle;
    }

    if (cmd & InputCommands::Enum::eGameSpeak8)
    {
        result |= PsxButtonBits::eL2 + PsxButtonBits::eSquare;
    }

    return result;
}

void InputObject::ShutDown_45F020()
{
#if MOBILE
    gTouchController->Close();
    gTouchController = nullptr;
#endif
}

bool InputObject::IsAnyHeld(u32 command)
{
    return (mPads[sCurrentControllerIndex].mRawInput & command) != 0;
}

bool InputObject::IsAnyPressed(u32 command)
{
    return (mPads[sCurrentControllerIndex].mPressed & command) != 0;
}

bool InputObject::IsAnyReleased(u32 keys)
{
    return (mPads[sCurrentControllerIndex].mReleased & keys) != 0;
}

u32 InputObject::GetHeld()
{
    return mPads[sCurrentControllerIndex].mRawInput;
}
u32 InputObject::GetPressed()
{
    return mPads[sCurrentControllerIndex].mPressed;
}
u32 InputObject::GetReleased()
{
    return mPads[sCurrentControllerIndex].mReleased;
}
