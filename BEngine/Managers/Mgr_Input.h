//*********************************************************************************\\
// 
// Originally provided by Mark Featherstone (2019), Extended and modified by 
// Benjamin Kimberley (2019-2024).
// Comprised of two major classes that provide management for KBM inputs and 
// gamepad inputs, as well several other related functionality options for 
// game functions.
//
//*********************************************************************************\\

#pragma once
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <Xinput.h>
#include <string>
#include <sstream>
#include <bitset>
#include <WinUser.h>
#include "D3D.h"

#include "Utils/Utils_D3D.h"		//D3D Types

//
//Keyboard Virtual Keycodes
//

#define VK_NUMPAD_ENTER 0xF0		//these three are very naughty
#define VK_LALT			0xF1		//they are actually some oem
#define VK_RALT			0xF2		//keys ATTN/FINISH/COPY - but lets pinch them :)

#define VK_0	0x30
#define VK_1	0x31
#define VK_2	0x32
#define VK_3	0x33
#define VK_4	0x34
#define VK_5	0x35
#define VK_6	0x36
#define VK_7	0x37
#define VK_8	0x38
#define VK_9	0x39
//these codes match ascii
#define VK_A	0x41
#define VK_B	0x42
#define VK_C	0x43
#define VK_D	0x44
#define VK_E	0x45
#define VK_F	0x46
#define VK_G	0x47
#define VK_H	0x48
#define VK_I	0x49
#define VK_J	0x4a
#define VK_K	0x4b
#define VK_L	0x4c
#define VK_M	0x4d
#define VK_N	0x4e
#define VK_O	0x4f
#define VK_P	0x50
#define VK_Q	0x51
#define VK_R	0x52
#define VK_S	0x53
#define VK_T	0x54
#define VK_U	0x55
#define VK_V	0x56
#define VK_W	0x57
#define VK_X	0x58
#define VK_Y	0x59
#define VK_Z	0x5a

//
//Gamepad Virtual Keycodes
//

#define VK_PAD_A                        0x5800
#define VK_PAD_B                        0x5801
#define VK_PAD_X                        0x5802
#define VK_PAD_Y                        0x5803
#define VK_PAD_RSHOULDER                0x5804
#define VK_PAD_LSHOULDER                0x5805
#define VK_PAD_LTRIGGER                 0x5806
#define VK_PAD_RTRIGGER                 0x5807

#define VK_PAD_DPAD_UP                  0x5810
#define VK_PAD_DPAD_DOWN                0x5811
#define VK_PAD_DPAD_LEFT                0x5812
#define VK_PAD_DPAD_RIGHT               0x5813
#define VK_PAD_START                    0x5814
#define VK_PAD_BACK                     0x5815
#define VK_PAD_LTHUMB_PRESS             0x5816
#define VK_PAD_RTHUMB_PRESS             0x5817

#define VK_PAD_LTHUMB_UP                0x5820
#define VK_PAD_LTHUMB_DOWN              0x5821
#define VK_PAD_LTHUMB_RIGHT             0x5822
#define VK_PAD_LTHUMB_LEFT              0x5823
#define VK_PAD_LTHUMB_UPLEFT            0x5824
#define VK_PAD_LTHUMB_UPRIGHT           0x5825
#define VK_PAD_LTHUMB_DOWNRIGHT         0x5826
#define VK_PAD_LTHUMB_DOWNLEFT          0x5827

#define VK_PAD_RTHUMB_UP                0x5830
#define VK_PAD_RTHUMB_DOWN              0x5831
#define VK_PAD_RTHUMB_RIGHT             0x5832
#define VK_PAD_RTHUMB_LEFT              0x5833
#define VK_PAD_RTHUMB_UPLEFT            0x5834
#define VK_PAD_RTHUMB_UPRIGHT           0x5835
#define VK_PAD_RTHUMB_DOWNRIGHT         0x5836
#define VK_PAD_RTHUMB_DOWNLEFT          0x5837

#define XINPUT_KEYSTROKE_KEYDOWN        0x0001
#define XINPUT_KEYSTROKE_KEYUP          0x0002
#define XINPUT_KEYSTROKE_REPEAT         0x0004

//
//Other
//

#define DEFAULT_HELD_CONDITION_TIME 0.1f

class Mgr_InputKBM
{
public:

	//////////////////////////////
	/// Enums, Types & Statics ///
	//////////////////////////////

	enum class ButtonTypes : unsigned
	{
		LBUTTON = 0,
		RBUTTON = 1,
		MBUTTON = 2,
		BUTTON4 = 3,
		BUTTON5 = 4,
		COUNT
	};

	////////////////////
	/// Constructors ///
	////////////////////

	Mgr_InputKBM() { ClearBuffers(); }
	~Mgr_InputKBM() { }	

	//////////////////
	/// Operations ///
	//////////////////

	//
	//Init + Core
	//

	/*
		Initialises input systems, alongside optional control flags for mouse.
		Requires knowing native resolution for accurate relative mouse position functionality.
		Run only ONCE at start of program.
	*/
	void RunOnceInit(HWND hwnd, bool showMouse = true, bool confineMouse = false);
	/*
		Processes rawinput messages and updates buffers accordingly.
		Plug this into message handler/pump function.
	*/
	void MessageEvent(HRAWINPUT rawInput);

	//Pre-frame update to update buffers with this frames key and mouse inputs
	void PreFrameProcess(float dt);
	//Call after all frame operations are done
	void PostFrameProcess();

	//
	//Keyboard
	//

	//Check if specific key is pressed this frame
	bool IsKeyPressed(unsigned short key);
	//Check if key is pressed, returning true if not pressed in previous frame
	bool IsKeyPressedNoRepeat(unsigned short key);
	//Check if key was released this frame
	bool IsKeyReleased(unsigned short key);
	//Check if key has been held (current and previous frame)
	bool IsKeyHeld(unsigned short key);
	//Check if key has been held, checking against a time condition for validity
	bool IsKeyHeld(unsigned short key, float condition);

	//Forcibly restarts the held timer at the given index
	void ResetKeyTimer(unsigned short key);


	//
	//Mouse
	//

	//Check if mouse button pressed this frame
	bool IsMouseButtonPressed(ButtonTypes button);
	//Check if mouse button pressed this frame, return false if down last frame
	bool IsMouseButtonPressedNoRepeat(ButtonTypes button);
	//Check if mouse button was released this frame
	bool IsMouseButtonReleased(ButtonTypes button);
	//Check if key has been held (current and previous frame)
	bool IsMouseButtonHeld(ButtonTypes button);
	//Check if key has been held, checking against a time condition for validity
	bool IsMouseButtonHeld(ButtonTypes button, float condition);


	//Forcibly restarts the held timer at the given index
	void ResetButtonTimer(ButtonTypes button);

	//
	//Debug + Utils
	//

	//Constructs a string providing debug info about the current state of the keyboard
	void Debug_KeyInfo(std::wstring& msg);
	//Constructs a string providing debug info about the current state of the mouse
	void Debug_MouseInfo(std::wstring& msg);
	//Constructs string providing current mouse position and adjust mouse position
	void Debug_MousePosition(std::string& msg);

	//Optional resetting of mouse position if mouse lost or after events (e.g window<->fullscreen)
	void OnLost();		//Sets to old mouse clip
	void OnReset();		//Sets to current mouse clip

	//Zeroes all buffers and mouse position diff
	void ClearBuffers();


	/////////////////
	/// Accessors ///
	/////////////////

	//
	//Keyboard
	//

	//Returns current timer value at index
	float GetKeyHeldTimer(unsigned short key);

	//
	//Mouse
	//

	//Returns current timer value at index
	float GetButtonHeldTimer(ButtonTypes button);

	//Returns collision values for mouse (posX, posY, width, height)
	Vec3 GetMouseCollider();
	//Returns the raw postion of the mosue relative to the window (unadjusted for changed window size)
	Vec2 GetMouseRawPosition() { return m_MousePosition; }
	//Returns an adjusted mouse position, adjusting for the change in window size relative to the native resolution. Gets adjustment from Game.
	Vec2 GetMouseRelativePosition();
	//A slightly more efficent version of the above that uses a passed modifier instead of seeking the value from Game class.
	Vec2 GetMouseRelativePosition(const Vec2& adj);
	//Returns the difference in movement between frames
	Vec2 GetMouseMovementDiff() { return m_MouseMoveDiff; }

	//Set visability status of the mouse
	void SetMouseVisibility(bool showMouse);
	//Set whether the cursor should be locked to the window or not
	void SetMouseWindowConfinement(bool confineMouse);
	//Set cursor visability and confine state in one call
	void SetMouseStatus(bool showMouse, bool confineMouse);
	//Forceably moves the cursor to set position
	void SetMousePosition(int posX, int posY);
	//Set collision radius for the mouse
	void SetMouseColliderRadius(float collider) { m_CollisionRadius = collider; }

	//
	//Other
	//

	//Update current known window size
	void SetWindowSize(float x, float y) {}

	/*
		Enables optional feature that tracks the duration of time that a key or mouse button has
		been held down for. Several useful applications but can be an unneeded function so disabled
		by default.
	*/
	void SetKeyTimersFlag(bool enable) { m_EnableKeyHeldDownTimers = enable; }
	void SetMouseTimersFlag(bool enable) { m_EnableMouseHeldDownTimers = enable; }

private:

	//////////////////////////////
	/// Enums, Types & Statics ///
	//////////////////////////////

	//Define buffers sizes, specific flags etc
	enum
	{
		RAWBUFF_SIZE = 512,
		KEYBUFF_SIZE = 255,
		KMASK_IS_DOWN = 1,
		MAX_BUTTONS = ButtonTypes::COUNT
	};

	//////////////////
	/// Operations ///
	//////////////////

	void ProcessKeys(RAWINPUT* raw);
	void ProcessMouse(RAWINPUT* raw);
	void GetMousePosAbsolute(Vec2& pos);

	////////////
	/// Data ///
	////////////

	//
	//Buffers
	//

	//Raw input buffer used during when handling the message event
	BYTE m_RawInputBuffer[RAWBUFF_SIZE];

	/*
		Multiple buffers to track different key event types:
		Key Down: When any registered keyboard key is pressed down.
		Key Up: When said key is released (toggling down buffer back)
		Last Down: Comparative buffer for checking if a key is being held down or not.
	*/
	bool m_KeyDownEventBuffer[KEYBUFF_SIZE];
	bool m_KeyUpEventBuffer[KEYBUFF_SIZE];
	bool m_PrevKeyDownEventBuffer[KEYBUFF_SIZE];

	/*
		Like keyboard events, we track mouse events the same way, with same
		tracking logic for different events.
	*/
	bool m_MouseDownEventBuffer[MAX_BUTTONS];
	bool m_MouseHeldEventBuffer[MAX_BUTTONS];
	bool m_MouseReleasedEventBuffer[MAX_BUTTONS];
	bool m_PrevMouseDownEventBuffer[MAX_BUTTONS];

	//Input timers may not be required, so are optionally enabled/disabled (saving on compute time)
	bool m_EnableKeyHeldDownTimers = false;
	bool m_EnableMouseHeldDownTimers = false;
	/*
		Knowing how long a key has been down can be valuable information for gameplay purposes,
		so we track them here. Timers are cleared in the event of a key release event, and updated
		in post-frame update.
	*/
	float m_KeyHeldDownTimers[KEYBUFF_SIZE];
	float m_MouseHeldDownTimers[MAX_BUTTONS];

	//
	//Mouse/Cursor
	//

	//Tracking value the current mouse position relative to game window.
	Vec2 m_MousePosition;
	//Tracks the difference in movement made per-frame
	Vec2 m_MouseMoveDiff;

	/*
		If needing to restrict the mouse to the window/limited space, track the current clipping range
		(e.g. rect equal to the size of the window), the previous clip (like the entire desktop size),
		and a controlling flag to indicate that the mouse should be confined to the current window or not.
	*/
	RECT m_CurrentMouseClip;
	RECT m_OldMouseClip;
	bool m_ConfineMouse = false;

	//If using a collider method with the mouse, this can be used to define a radius
	float m_CollisionRadius = 2.f;

	//
	//Other
	//

	//Main window handle (for mouse purposes)
	HWND m_WindowHandle;
};

class Mgr_InputGamepad
{
public:

	//////////////////////////////
	/// Enums, Types & Statics ///
	//////////////////////////////

	//Maximium range of movement for controller sticks
	static const int STICK_RANGE = 32767;
	static const int STICK_INPUT_COUNT = 32;

	//Gamepad Data, one used per pad
	struct State
	{
		//
		//General
		//

		//Controllers current button state
		XINPUT_STATE m_State;
		//What port/ID this pad relates to
		int m_PortID = -1;
		
		//
		//Triggers
		//

		//Amount of pressure applied on each trigger (0-1 range)
		float m_LeftTrigger = 0.f;
		float m_RightTrigger = 0.f;
		//Like stick deadzone, define the range at which inputs are registered (NYI)
		float m_LeftTriggerDeadzone = 0.f;
		float m_RightTriggerDeadzone = 0.f;

		//If pad is detected already, allows for granular control over input capabilities
		bool m_EnableInputs = true;
		//Alignment padding (or future flags)
		bool m_Padding_02 = false;
		bool m_Padding_01 = false;
		bool m_Padding_00 = false;

		//
		//L Stick 
		//
	
		//Stick direction (-1 to 1 range)
		float m_LeftStickX = 0;
		float m_LeftStickY = 0;
		//Amount of stick movement required to produce a response (0-1 range)
		float m_LeftStickDeadzoneX = 0.9f;
		float m_LeftStickDeadzoneY = 0.9f;

		//
		//R Stick
		//
		
		//Stick direction
		float m_RightStickX = 0;
		float m_RightStickY = 0;
		//Amount of stick movement required to produce a response
		float m_RightStickDeadzoneX = 0.1f;
		float m_RightStickDeadzoneY = 0.1f;
	};

	enum class StickIDs : unsigned
	{
		LEFT_STICK,
		RIGHT_STICK
	};

	////////////////////
	/// Constructors ///
	////////////////////

	Mgr_InputGamepad() { }
	~Mgr_InputGamepad() { }

	//////////////////
	/// Operations ///
	//////////////////

	//Main update window
	void Update();

	//Post frame operations
	void PostFrameProcess(float dt);

	//
	//Input Checks
	//

	//Check to see if button was pressed
	bool IsButtonPressed(int portID, unsigned short buttonID);
	//Check to see if button was pressed, but only once
	bool IsButtonPressedNoRepeat(unsigned portID, unsigned short buttonID);
	//Based on previous state, check to see if button was released this frame
	bool IsButtonReleased(int portID, unsigned short buttonID);
	//Based on previous state, check to see if button is held down.
	bool IsButtonHeld(int portID, unsigned short buttonID);
	

	/////////////////
	/// Accessors ///
	/////////////////

	/*
		Adjusts the range of inputs to ignore from target stick.
		Range clamps to between 0-1 range.
	*/
	void SetStickDeadzone(int portID, StickIDs stickID, float x, float y);

	//Enables optional timer track for held down buttons
	void SetHeldTimerFlag(bool enable) { m_EnableHeldTimers = enable; }

	//Gets all state + additional information about a pad
	const State& GetState(int portID);
	//Check to see if pad is plugged in
	bool GetConnectedStatus(int portID);

	//Get a list of pad activity - motion of sticks and triggers
	void GetAnalogueNames(int portID, std::wstring& msg);
	//Get a list of pad activity - button presses
	void GetDigitalNames(int portID, std::wstring& msg);

private:

	//////////////////
	/// Operations ///
	//////////////////

	////////////
	/// Data ///
	////////////

	/*
		Similarly to KBM manager, two sets of buffers/states are kept here to track the current and previous state
		of the gamepad, alongside timers to track how long inputs have been engaged for.
	*/
	State m_States[XUSER_MAX_COUNT];
	State m_PrevStates[XUSER_MAX_COUNT];

	//Input timers may not be required, so are optionally enabled/disabled (saving on compute time)
	bool m_EnableHeldTimers = false;
	float m_InputHeldTimers[XUSER_MAX_COUNT][STICK_INPUT_COUNT];
};

//Extra typedef to trim down on char bloat
typedef Mgr_InputKBM::ButtonTypes MouseButtons;