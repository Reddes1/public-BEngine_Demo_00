#include "Mgr_Input.h"

#include "Game.h"       //For native resolution access

#include "Utils/MathHelper.h"
#include "Utils/Utils_D3D.h"
#include "Utils/Utils_Debug.h"
#include "Utils/Utils_D3D_Debug.h"

using namespace DirectX::SimpleMath;

bool Mgr_InputKBM::IsKeyPressed(unsigned short key)
{
    assert(key < KEYBUFF_SIZE);
    return m_KeyDownEventBuffer[key] != 0;
}

bool Mgr_InputKBM::IsKeyPressedNoRepeat(unsigned short key)
{
    assert(key < KEYBUFF_SIZE);
    return m_KeyDownEventBuffer[key] && !m_PrevKeyDownEventBuffer[key];
}

bool Mgr_InputKBM::IsKeyReleased(unsigned short key)
{
    assert(key < KEYBUFF_SIZE);
    return m_KeyUpEventBuffer[key] != 0;
}

bool Mgr_InputKBM::IsKeyHeld(unsigned short key)
{
    assert(key < KEYBUFF_SIZE);
    return m_KeyDownEventBuffer[key] && m_PrevKeyDownEventBuffer[key];
}

bool Mgr_InputKBM::IsKeyHeld(unsigned short key, float condition)
{
    assert(key < KEYBUFF_SIZE);
    return (m_KeyDownEventBuffer[key] && m_PrevKeyDownEventBuffer[key]) && m_KeyHeldDownTimers[key] > condition;
}

void Mgr_InputKBM::ResetKeyTimer(unsigned short key)
{
    assert(key < KEYBUFF_SIZE);
    m_KeyHeldDownTimers[key] = 0.f;
}

bool Mgr_InputKBM::IsMouseButtonPressed(ButtonTypes button)
{
    assert(button != ButtonTypes::COUNT);
    return m_MouseDownEventBuffer[(unsigned)button];
}

bool Mgr_InputKBM::IsMouseButtonPressedNoRepeat(ButtonTypes button)
{
    assert(button != ButtonTypes::COUNT);
    return m_MouseDownEventBuffer[(unsigned)button] && !m_PrevMouseDownEventBuffer[(unsigned)button];
}

bool Mgr_InputKBM::IsMouseButtonReleased(ButtonTypes button)
{
    assert(button != ButtonTypes::COUNT);
    return m_MouseReleasedEventBuffer[(unsigned)button];
}

bool Mgr_InputKBM::IsMouseButtonHeld(ButtonTypes button)
{
    assert(button != ButtonTypes::COUNT);
    return m_MouseDownEventBuffer[(unsigned)button] && m_PrevMouseDownEventBuffer[(unsigned)button];
}

bool Mgr_InputKBM::IsMouseButtonHeld(ButtonTypes button, float condition)
{
    assert(button != ButtonTypes::COUNT);
    return (m_MouseDownEventBuffer[(unsigned)button] && m_PrevMouseDownEventBuffer[(unsigned)button]) && m_MouseHeldDownTimers[(unsigned)button] > condition;
}

void Mgr_InputKBM::ResetButtonTimer(ButtonTypes button)
{
    assert(button != ButtonTypes::COUNT);
    m_MouseHeldDownTimers[(unsigned)button] = 0.f;
}

void Mgr_InputKBM::ClearBuffers()
{
    //Clear raw buffer
    ZeroMemory(m_RawInputBuffer, sizeof(m_RawInputBuffer));
    
    //Clear KB buffers
    ZeroMemory(m_KeyDownEventBuffer, sizeof(m_KeyDownEventBuffer));
    ZeroMemory(m_KeyUpEventBuffer, sizeof(m_KeyUpEventBuffer));
    ZeroMemory(m_PrevKeyDownEventBuffer, sizeof(m_PrevKeyDownEventBuffer));
    
    //Clear mouse buffers
    ZeroMemory(m_MouseDownEventBuffer, sizeof(m_MouseDownEventBuffer));
    ZeroMemory(m_MouseHeldEventBuffer, sizeof(m_MouseHeldEventBuffer));
    ZeroMemory(m_MouseReleasedEventBuffer, sizeof(m_MouseReleasedEventBuffer));
    ZeroMemory(m_PrevMouseDownEventBuffer, sizeof(m_PrevMouseDownEventBuffer));

    //Clear timers
    ZeroMemory(m_KeyHeldDownTimers, sizeof(m_KeyHeldDownTimers));
    ZeroMemory(m_MouseHeldDownTimers, sizeof(m_MouseHeldDownTimers));

    //Clear position data
    m_MousePosition = Vec2(0.f, 0.f);
    m_MouseMoveDiff = Vec2(0.f, 0.f);
}

void Mgr_InputKBM::ProcessKeys(RAWINPUT* raw)
{
    //Get raw data flags and virtual key code
    unsigned short flags = raw->data.keyboard.Flags;
    unsigned short vkey = raw->data.keyboard.VKey;
    //Additionally get the scan code (not fully used yet)
    unsigned short scanCode = raw->data.keyboard.MakeCode;

    //Discard any invalid inputs
    if (vkey >= 255)
        return;

    if (vkey == VK_SHIFT)
    {
        // correct left-hand / right-hand SHIFT (VK_LSHIFT / VK_RSHIFT)
        vkey = MapVirtualKey(scanCode, MAPVK_VSC_TO_VK_EX);
    }
    else if (vkey == VK_NUMLOCK)
    {
        // correct PAUSE/BREAK and NUM LOCK silliness, and set the extended bit
        scanCode = (MapVirtualKey(vkey, MAPVK_VK_TO_VSC) | 0x100);
    }

    // e0 and e1 are escape sequences used for certain special keys, such as PRINT and PAUSE/BREAK.
    // see http://www.win.tue.nl/~aeb/linux/kbd/scancodes-1.html
    const bool isE0 = ((flags & RI_KEY_E0) != 0);
    const bool isE1 = ((flags & RI_KEY_E1) != 0);

    if (isE1)
    {
        // for escaped sequences, turn the virtual key into the correct scan code using MapVirtualKey.
        // however, MapVirtualKey is unable to map VK_PAUSE (this is a known bug), hence we map that by hand.
        if (vkey == VK_PAUSE)
            scanCode = 0x45;
        else
            scanCode = MapVirtualKey(vkey, MAPVK_VK_TO_VSC);
    }

    //Determine if the vkey is one of the modifier/special keys
    switch (vkey)
    {
        // right-hand CONTROL and ALT have their e0 bit set
    case VK_CONTROL:
        if (isE0)
            vkey = VK_RCONTROL; //right control
        else
            vkey = VK_LCONTROL;  //left control
        break;

    case VK_MENU:
        if (isE0)
            vkey = VK_RALT;  //right alt
        else
            vkey = VK_LALT;	//left alt
        break;

        // NUMPAD ENTER has its e0 bit set
    case VK_RETURN:
        if (isE0)
            vkey = VK_NUMPAD_ENTER; //numpad enter
        break;
    }

    //Was it a key down event?
    if (!(flags & RI_KEY_BREAK))
    {
        //Store key down event
        m_KeyDownEventBuffer[vkey] = true;
    }
    //Was infact a key up event
    else
    {
        //Update key down and up buffers appropriately
        m_KeyDownEventBuffer[vkey] = false;
        m_KeyUpEventBuffer[vkey] = true;
    }
}

void Mgr_InputKBM::ProcessMouse(RAWINPUT* raw)
{
    //Get flag data from raw input
    unsigned short flags = raw->data.mouse.usButtonFlags;


    //
    //Mouse
    //

    //Keep the last known mouse position briefly and update it to a new position
    Vec2 last(m_MousePosition);
    GetMousePosAbsolute(m_MousePosition);
    //Calculate the difference in position between frames
    m_MouseMoveDiff = m_MousePosition - last;
}

void Mgr_InputKBM::GetMousePosAbsolute(Vec2& pos)
{
    POINT mpos;
    if (GetCursorPos(&mpos))
    {
        if (ScreenToClient(m_WindowHandle, &mpos))
        {
            pos = Vec2(static_cast<float>(mpos.x), static_cast<float>(mpos.y));
        }
    }
}

void Mgr_InputKBM::MessageEvent(HRAWINPUT rawInput)
{
    UINT dwSize = 0;

    UINT res = GetRawInputData(rawInput, RID_INPUT, NULL, &dwSize, sizeof(RAWINPUTHEADER));
    if (res != 0)
        return;//bad input so ignore it

    if (dwSize >= RAWBUFF_SIZE)
    {
        DBOUT("Buffer too small. Is " << RAWBUFF_SIZE << " wants " << dwSize);
        assert(false);
    }

    if (GetRawInputData(rawInput, RID_INPUT, m_RawInputBuffer, &dwSize, sizeof(RAWINPUTHEADER)) != dwSize)
    {
        DBOUT("GetRawInputData failed");
        assert(false);
    }

    RAWINPUT* raw = (RAWINPUT*)m_RawInputBuffer;

    if (raw->header.dwType == RIM_TYPEKEYBOARD)
    {
        ProcessKeys(raw);
    }
    else if (raw->header.dwType == RIM_TYPEMOUSE)
    {
        ProcessMouse(raw);
    }
}

void Mgr_InputKBM::Debug_KeyInfo(std::wstring& msg)
{
    msg = L"";
    for (int i = 0; i < KEYBUFF_SIZE; ++i)
    {
        if (m_KeyDownEventBuffer[i] == 0)
            continue;
        //numeric pad and arrow keys conflict, ise0 is set for arrow keys
        //here we just assume the codes refer to arrow keys
        switch (i)
        {
        case VK_RIGHT:
            msg += L"RIGHT ARROW ";
            break;
        case VK_LEFT:
            msg += L"LEFT ARROW ";
            break;
        case VK_UP:
            msg += L"UP ARROW ";
            break;
        case VK_DOWN:
            msg += L"DOWN ARROW ";
            break;
        default:
            wchar_t key[256];
            if (GetKeyNameTextW(m_KeyDownEventBuffer[i] << 13, key, sizeof(key)))
            {
                msg += key;
                msg += L" ";
            }
        }
    }
}

void Mgr_InputKBM::Debug_MouseInfo(std::wstring& msg)
{
    msg = L"";
    static float oldX = 0, oldY = 0;
    if (!MathHelper::Equals(m_MouseMoveDiff.x, 0))
        oldX = m_MouseMoveDiff.x;
    if (!MathHelper::Equals(m_MouseMoveDiff.y, 0))
        oldY = m_MouseMoveDiff.y;
    std::wstringstream str;
    str.precision(0);
    str << L"x=" << m_MousePosition.x << L" y=" << m_MousePosition.y;
    str << L" xinc=" << oldX << L" yinc=" << oldY;
    if (m_MouseDownEventBuffer[(unsigned)ButtonTypes::LBUTTON])
        str << L" LButton";
    if (m_MouseDownEventBuffer[(unsigned)ButtonTypes::MBUTTON])
        str << L" MButton";
    if (m_MouseDownEventBuffer[(unsigned)ButtonTypes::RBUTTON])
        str << L" RButton";

    msg = str.str();
}

void Mgr_InputKBM::Debug_MousePosition(std::string& msg)
{
    msg += "Raw Mouse Pos: ( " + std::to_string(m_MousePosition.x) + ", " + std::to_string(m_MousePosition.y) + " )\n";
    Vec2 adjPos = GetMouseRelativePosition();
    msg += "Relative Mouse Pos: ( " + std::to_string(adjPos.x) + ", " + std::to_string(adjPos.y) + " )";
}

float Mgr_InputKBM::GetKeyHeldTimer(unsigned short key)
{
    assert(key < KEYBUFF_SIZE);
    return m_KeyHeldDownTimers[key];
}

float Mgr_InputKBM::GetButtonHeldTimer(ButtonTypes button)
{
    assert(button != ButtonTypes::COUNT);
    return m_MouseHeldDownTimers[(unsigned)button];
}

Vec3 Mgr_InputKBM::GetMouseCollider()
{
    return Vec3(m_MousePosition.x, m_MousePosition.y, m_CollisionRadius);
}

Vec2 Mgr_InputKBM::GetMouseRelativePosition()
{
    Game* game = Game::GetGame();
    Vec2 adj = game->GetWinAdjustMult();
    return Vec2(m_MousePosition.x * adj.x, m_MousePosition.y * adj.y);
}

Vec2 Mgr_InputKBM::GetMouseRelativePosition(const Vec2& adj)
{
    return Vec2(m_MousePosition.x * adj.x, m_MousePosition.y * adj.y);
}

void Mgr_InputKBM::RunOnceInit(HWND hwnd, bool showMouse, bool confineMouse)
{
    //Get window handle
    m_WindowHandle = hwnd;

    RAWINPUTDEVICE Rid[2];

    //these ids trigger mouse input
    Rid[0].usUsagePage = 0x01;
    Rid[0].usUsage = 0x02;
    Rid[0].dwFlags = RIDEV_INPUTSINK;// RIDEV_NOLEGACY;   // adds HID mouse and also ignores legacy mouse messages
    Rid[0].hwndTarget = hwnd;

    //these ids trigger keyboard input
    Rid[1].usUsagePage = 0x01;
    Rid[1].usUsage = 0x06;
    Rid[1].dwFlags = RIDEV_INPUTSINK;// RIDEV_NOLEGACY;   // adds HID keyboard and also ignores legacy keyboard messages
    Rid[1].hwndTarget = hwnd;

    // RIDEV_NOLEGACY will stop normal message pump WM_CHAR type messages
    // sometimes we might want that, for now we'll leave it, then we can
    // still use the normal message pump for things like player name entry

    if (RegisterRawInputDevices(Rid, 2, sizeof(Rid[0])) == FALSE) {
        MessageBox(0, "Cannot get keyboard and mouse input", 0, 0);
        assert(false);
    }

    //Store the current clip
    GetClipCursor(&m_OldMouseClip);
    SetMouseStatus(showMouse, confineMouse);
    GetMousePosAbsolute(m_MousePosition);
}

void Mgr_InputKBM::SetMouseStatus(bool showMouse, bool confineMouse)
{
    ShowCursor(showMouse);
    SetMouseWindowConfinement(confineMouse);
}

void Mgr_InputKBM::SetMousePosition(int posX, int posY)
{
    m_MousePosition = { (float)posX, (float)posY };
    SetCursorPos(posX, posY);
}

void Mgr_InputKBM::SetMouseVisibility(bool showMouse)
{
    ShowCursor(showMouse);
}

void Mgr_InputKBM::SetMouseWindowConfinement(bool confineMouse)
{
    //Setup new confinement area if true
    if (confineMouse)
    {
        GetWindowRect(m_WindowHandle, &m_CurrentMouseClip);
        ClipCursor(&m_CurrentMouseClip);
    }
    //As the current clip may now be invalid, restore to original clip generated at start of program
    else
    {
        m_CurrentMouseClip = m_OldMouseClip;
        ClipCursor(&m_CurrentMouseClip);
    }
}


void Mgr_InputKBM::OnLost()
{
    ClearBuffers();
    ClipCursor(&m_OldMouseClip);
}

void Mgr_InputKBM::OnReset()
{
    ClearBuffers();
    ClipCursor(&m_CurrentMouseClip);
}

void Mgr_InputKBM::PreFrameProcess(float dt)
{
    //If mouse timing enabled
    if (m_EnableMouseHeldDownTimers)
    {
        //Mouse button indexes are not contiguous, so create a temp array organising them for loop purposes
        SHORT keys[] = { VK_LBUTTON, VK_RBUTTON, VK_MBUTTON, VK_XBUTTON1, VK_XBUTTON2 };

        //Check for mouse updates
        for (unsigned i(0); i < MAX_BUTTONS; ++i)
        {
            //Get the current state of the mouse button
            SHORT state = GetAsyncKeyState(keys[i]);

            //Is button currently down?
            m_MouseDownEventBuffer[i] = state;
            //Is button held down?
            m_MouseHeldEventBuffer[i] = state & 0x8000;
            //Is button released (based on current update and previous frame)
            m_MouseReleasedEventBuffer[i] = (!m_MouseDownEventBuffer[i] && m_PrevMouseDownEventBuffer[i]);

            //Update timers
            if (m_MouseHeldEventBuffer[i])
                m_MouseHeldDownTimers[i] += dt;
            else
                m_MouseHeldDownTimers[i] = 0.f;
        }
    }
    //If key timing enabled
    if (m_EnableKeyHeldDownTimers)
    {
        for (unsigned i(0); i < KEYBUFF_SIZE; ++i)
        {
            if (m_KeyDownEventBuffer[i] && m_PrevKeyDownEventBuffer[i])
                m_KeyHeldDownTimers[i] += dt;
            else
                m_KeyHeldDownTimers[i] = 0.f;
        }
    }
}
void Mgr_InputKBM::PostFrameProcess()
{
    //Clear mouse diffs
    m_MouseMoveDiff.x = 0;
    m_MouseMoveDiff.y = 0;

    //Update the last previous frame buffer
    memcpy(m_PrevKeyDownEventBuffer, m_KeyDownEventBuffer, sizeof(m_KeyDownEventBuffer));
    //Store current mouse down buffer for comparisons next frame
    memcpy(m_PrevMouseDownEventBuffer, m_MouseDownEventBuffer, sizeof(m_MouseDownEventBuffer));

    //Clear the up buffers (will no longer be valid past this frame)
    ZeroMemory(m_KeyUpEventBuffer, sizeof(m_KeyUpEventBuffer));
}


void Mgr_InputGamepad::Update()
{
    for (short i(0); i < XUSER_MAX_COUNT; ++i)
    {
        //Get a reference of the pads state information
        State& s = m_States[i];
        //Capture the last state
        m_PrevStates[i] = s;
        //Clear current state data
        ZeroMemory(&s.m_State, sizeof(XINPUT_STATE));

        //Set pad to inactive code (will be reassigned if update successful)
        //Allows for dynamic enabling/disabling of pads during gameplay loop
        s.m_PortID = -1;

        //Attempt to get updated state for gp at index
        if (XInputGetState(i, &s.m_State) == ERROR_SUCCESS)
        {
            //Generate value representing the range of movement for the left stick on x/y coords
            float normLX = fmaxf(-1, (float)s.m_State.Gamepad.sThumbLX / STICK_RANGE);
            float normLY = fmaxf(-1, (float)s.m_State.Gamepad.sThumbLY / STICK_RANGE);

            //Determine if the movement range is within the deadzone, 
            s.m_LeftStickX = (abs(normLX) < s.m_LeftStickDeadzoneX ? 0 : (abs(normLX) - s.m_LeftStickDeadzoneX) * (normLX / abs(normLX)));
            s.m_LeftStickY = (abs(normLY) < s.m_LeftStickDeadzoneY ? 0 : (abs(normLY) - s.m_LeftStickDeadzoneY) * (normLY / abs(normLY)));

            //Adjust for deadzone settings
            if (s.m_LeftStickDeadzoneX > 0)
                s.m_LeftStickX *= 1 / (1 - s.m_LeftStickDeadzoneX);
            if (s.m_LeftStickDeadzoneY > 0)
                s.m_LeftStickY *= 1 / (1 - s.m_LeftStickDeadzoneY);

            //Right Stick
            float normRX = fmaxf(-1, (float)s.m_State.Gamepad.sThumbRX / STICK_RANGE);
            float normRY = fmaxf(-1, (float)s.m_State.Gamepad.sThumbRY / STICK_RANGE);

            s.m_RightStickX = (abs(normRX) < s.m_RightStickDeadzoneX ? 0 : (abs(normRX) - s.m_RightStickDeadzoneX) * (normRX / abs(normRX)));
            s.m_RightStickY = (abs(normRY) < s.m_RightStickDeadzoneY ? 0 : (abs(normRY) - s.m_RightStickDeadzoneY) * (normRY / abs(normRY)));


            //Adjust for deadzone settings
            if (s.m_RightStickDeadzoneX > 0)
                s.m_RightStickX *= 1 / (1 - s.m_RightStickDeadzoneX);
            if (s.m_RightStickDeadzoneY > 0)
                s.m_RightStickY *= 1 / (1 - s.m_RightStickDeadzoneY);

            //Get trigger pressure states
            s.m_LeftTrigger = (float)s.m_State.Gamepad.bLeftTrigger / 255;
            s.m_RightTrigger = (float)s.m_State.Gamepad.bRightTrigger / 255;

            //Update port
            s.m_PortID = i;
        }
    }
}

void Mgr_InputGamepad::PostFrameProcess(float dt)
{
    if (m_EnableHeldTimers)
    {
        //For each pad
        for (unsigned i(0); i < XUSER_MAX_COUNT; ++i)
        {
            //Loop through contiguous input IDs
            for (unsigned j(VK_PAD_A); j < VK_PAD_RTHUMB_DOWNLEFT; ++j)
            {
                //If port is active
                if (IsButtonHeld(i, j))
                    m_InputHeldTimers[i][j - VK_PAD_A] += dt;
                else
                    m_InputHeldTimers[i][j - VK_PAD_A] = 0.f;
            }

            //TO-DO: Keystroke Inputs
        }
    }
}

bool Mgr_InputGamepad::IsButtonPressed(int portID, unsigned short buttonID)
{
    assert(portID >= 0 && portID < XUSER_MAX_COUNT);
    if (portID == -1)
        return false;

    //Input this frame (regardless of anything else)
    return(m_States[portID].m_State.Gamepad.wButtons & buttonID) != 0;
}

bool Mgr_InputGamepad::IsButtonPressedNoRepeat(unsigned portID, unsigned short buttonID)
{
    assert(portID >= 0 && portID < XUSER_MAX_COUNT);
    if (portID == -1)
        return false;

    //Input only on this frame
    return(m_States[portID].m_State.Gamepad.wButtons & buttonID) != 0 && (m_PrevStates[portID].m_State.Gamepad.wButtons & buttonID) == 0;
}

bool Mgr_InputGamepad::IsButtonHeld(int portID, unsigned short buttonID)
{
    assert(portID >= 0 && portID < XUSER_MAX_COUNT);
    if (portID == -1)
        return false;

    //Inputs on both frames
    return(m_States[portID].m_State.Gamepad.wButtons & buttonID) != 0 && (m_PrevStates[portID].m_State.Gamepad.wButtons & buttonID) != 0;
}

bool Mgr_InputGamepad::IsButtonReleased(int portID, unsigned short buttonID)
{
    assert(portID >= 0 && portID < XUSER_MAX_COUNT);
    if (portID == -1)
        return false;

    //No input this frame, but there was last frame
    return (m_States[portID].m_State.Gamepad.wButtons & buttonID) == 0 && (m_PrevStates[portID].m_State.Gamepad.wButtons & buttonID) != 0;
}

const Mgr_InputGamepad::State& Mgr_InputGamepad::GetState(int portID)
{
    assert(portID >= 0 && portID < XUSER_MAX_COUNT);

    return m_States[portID];
}

bool Mgr_InputGamepad::GetConnectedStatus(int portID)
{
    //Check that pad ID is valid
    assert(portID >= 0 && portID < XUSER_MAX_COUNT);
    if (portID == -1)
        return false;

    return m_States[portID].m_PortID != -1;
}

void Mgr_InputGamepad::GetAnalogueNames(int portID, std::wstring& msg)
{
    assert(portID >= 0 && portID < XUSER_MAX_COUNT);
    State& s = m_States[portID];
    msg = L"";
    if (s.m_PortID == -1)
        return;

    std::wstringstream str;
    str.precision(2);
    str << L"Left stick(x=" << s.m_LeftStickX << L",y=" << s.m_LeftStickY << L")";
    str << L" Right stick(x=" << s.m_RightStickX << L",y=" << s.m_RightStickY << L")";
    str << L" Trigger Left=" << s.m_LeftTrigger << L", right=" << s.m_RightTrigger;
    msg = str.str();

}

void Mgr_InputGamepad::GetDigitalNames(int portID, std::wstring& msg)
{
    assert(portID >= 0 && portID < XUSER_MAX_COUNT);
    State& s = m_States[portID];
    msg = L"";
    if (s.m_PortID == -1) 
        return;

    std::wstringstream str;
    if (IsButtonPressed(portID, XINPUT_GAMEPAD_DPAD_UP))
        str << L"DPAD_UP ";
    if (IsButtonPressed(portID, XINPUT_GAMEPAD_DPAD_DOWN))
        str << L"DPAD DOWN ";
    if (IsButtonPressed(portID, XINPUT_GAMEPAD_DPAD_LEFT))
        str << L"DPAD LEFT ";
    if (IsButtonPressed(portID, XINPUT_GAMEPAD_DPAD_RIGHT))
        str << L"DPAD RIGHT ";
    if (IsButtonPressed(portID, XINPUT_GAMEPAD_START))
        str << L"START ";
    if (IsButtonPressed(portID, XINPUT_GAMEPAD_BACK))
        str << L"BACK ";
    if (IsButtonPressed(portID, XINPUT_GAMEPAD_LEFT_THUMB))
        str << L"LEFT THUMB ";
    if (IsButtonPressed(portID, XINPUT_GAMEPAD_RIGHT_THUMB))
        str << L"RIGHT THUMB ";
    if (IsButtonPressed(portID, XINPUT_GAMEPAD_LEFT_SHOULDER))
        str << L"LEFT SHOULDER ";
    if (IsButtonPressed(portID, XINPUT_GAMEPAD_RIGHT_SHOULDER))
        str << L"RIGHT SHOULDER ";
    if (IsButtonPressed(portID, XINPUT_GAMEPAD_A))
        str << L"A ";
    if (IsButtonPressed(portID, XINPUT_GAMEPAD_B))
        str << L"B ";
    if (IsButtonPressed(portID, XINPUT_GAMEPAD_X))
        str << L"X ";
    if (IsButtonPressed(portID, XINPUT_GAMEPAD_Y))
        str << L"Y ";
    msg = str.str();
}

void Mgr_InputGamepad::SetStickDeadzone(int portID, StickIDs stickID, float x, float y)
{
    //Check that pad ID is valid
    assert(portID >= 0 && portID < XUSER_MAX_COUNT);
    if (portID == -1)
        return;

    switch (stickID)
    {
    case StickIDs::LEFT_STICK:
        m_States[portID].m_LeftStickDeadzoneX = MathHelper::Clamp(x, 0.f, 1.f);;
        m_States[portID].m_LeftStickDeadzoneY = MathHelper::Clamp(y, 0.f, 1.f);;
        break;

    case StickIDs::RIGHT_STICK:
        m_States[portID].m_RightStickDeadzoneX = MathHelper::Clamp(x, 0.f, 1.f);;
        m_States[portID].m_RightStickDeadzoneY = MathHelper::Clamp(y, 0.f, 1.f);;
        break;
    }

}


