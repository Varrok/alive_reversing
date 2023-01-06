#pragma once

#include "../relive_lib/Function.hpp"
#include <optional>

class BaseGameAutoPlayer;

namespace AO {

struct PSX_Pad final
{
    u32 mRawInput;
    u8 mDir;
    s8 field_3;
    u32 mPreviousInput;
    u32 mPressed;
    u32 mReleased;
    s8 mPreviousDir;
    s8 field_B;
};
ALIVE_ASSERT_SIZEOF(PSX_Pad, 0xC);

enum InputCommands : u32
{
    eRightGameSpeak = 1u << 0,    // 0x1
    eSneak = 1u << 1,             // 0x2
    eLeftGameSpeak = 1u << 2,     // 0x4
    eRun = 1u << 3,               // 0x8
    eHop = 1u << 4,               // 0x10
    eThrowItem = 1u << 5,         // 0x20
    eCrouchOrRoll = 1u << 6,      // 0x40
    eDoAction = 1u << 7,          // 0x80
    eCheatMode = 1u << 8,         // 0x100
    eUnPause_OrConfirm = 1u << 9, // 0x200 - not present/unused in OG!!
    ePause = 1u << 10,            // 0x400 - not present/unused in OG!!
    eBack = 1u << 11,             // 0x800
    eUp = 1u << 12,               // 0x1000
    eRight = 1u << 13,            // 0x2000
    eDown = 1u << 14,             // 0x4000
    eLeft = 1u << 15,             // 0x8000
    eGameSpeak1 = 1u << 16,
    eGameSpeak2 = 1u << 17,
    eGameSpeak3 = 1u << 18,
    eGameSpeak4 = 1u << 19,
    eGameSpeak5 = 1u << 20,
    eGameSpeak6 = 1u << 21,
    eGameSpeak7 = 1u << 22,
    eGameSpeak8 = 1u << 23,
};

extern const InputCommands sInputKey_Right;
extern const InputCommands sInputKey_Left;
extern const InputCommands sInputKey_Up;
extern const InputCommands sInputKey_Down;
extern const InputCommands sInputKey_Hop;
extern const InputCommands sInputKey_DoAction;
extern const InputCommands sInputKey_Run;
extern const InputCommands sInputKey_Sneak;
extern const InputCommands sInputKey_FartRoll;
extern const InputCommands sInputKey_ThrowItem;

const InputCommands sInputKey_Chant = static_cast<InputCommands>(eRightGameSpeak | eLeftGameSpeak);

#define kAO_Esc "\x06"

#define kAO_Speak1 "\x08"

#define kAO_Speak2 "\x09"

#define kAO_Up "\x10"
#define kAO_Down "\x11"
#define kAO_Left "\x12"
#define kAO_Right "\x13"
#define kAO_Or "\x14"
#define kAO_DirectionalButtons "\x15"
#define kAO_ConfirmContinue "\x16"

#define kAO_Run "\x0a"
#define kAO_Sneak "\x0b"
#define kAO_Jump_Or_Hello "\x0c"
#define kAO_Action "\x0d"
#define kAO_Throw "\x0e"
#define kAO_Crouch "\x0f"


bool Input_IsChanting();
void Input_InitKeyStateArray();
void Input_SetKeyState(s32 key, s8 bIsDown);



void Input_GetCurrentKeyStates();

s8 Input_IsVKPressed(s32 key);

void Input_Init();

void Input_DisableInput();

const char_type* Input_GetButtonString(InputCommands inputCommand, bool forceKeyboardLookupIfGamepadFails = true);
const char_type* Input_GetButtonString_44F1C0(InputCommands inputCommand);

s32 Input_Remap(InputCommands inputCmd);

s32 Input_SaveSettingsIni();

s8 Input_GetLastPressedKey();

void Input_Reset();

bool Input_JoyStickEnabled();
void Input_SetJoyStickEnabled(bool enabled);
bool Input_JoyStickAvailable();


class InputObject final
{
public:
    void InitPad(u32 padCount);

    void Update(BaseGameAutoPlayer& gameAutoPlayer);

    static void Shutdown();

    void SetDemoRes(u32** ppDemoRes);

    s32 IsDemoPlaying();

    PSX_Pad mPads[2] = {};
    u32** mpDemoRes = nullptr;
    u32 mDemoCommandIndex = 0;
    u16 mbDemoPlaying = 0;
    s16 field_22 = 0;
    u32 mCommand = 0;
    s32 mCommandDuration = 0;
    s32 field_2C = 0;

    // These use the active pad
    bool IsAnyHeld(u32 command) const;
    bool IsAnyPressed(u32 command) const;
    bool IsAnyReleased(u32 command) const;
    u8 Dir() const;

    enum class PadIndex
    {
        First,
        Second,
        Active,
    };

    PadIndex CurrentController() const;

    void SetCurrentController(PadIndex padIdx);

    bool JoyStickEnabled() const;

    // Check a specific pad
    bool IsAnyHeld(PadIndex padIx, u32 command) const;
    bool IsAnyPressed(PadIndex padIx, u32 command) const;
    bool IsAnyReleased(PadIndex padIx, u32 command) const;
    bool IsAllPressed(PadIndex padIx, u32 commands) const;

    u32 GetHeld(PadIndex padIx) const;

    // These use the active pad
    u32 GetHeld() const;
    u32 GetPressed() const;
    u32 GetReleased() const;

    u32 GetPressed(PadIndex padIx) const;

    u32 Input_Read_Pad(u32 padIdx);
};
ALIVE_ASSERT_SIZEOF(InputObject, 0x30);

InputObject& Input();

bool Input_IsGameSpeakPressed(InputCommands gameSpeakId);
std::optional<InputCommands> Input_IsAnyGameSpeakPressed();
} // namespace AO
