/**
 ** Supermodel
 ** A Sega Model 3 Arcade Emulator.
 ** Copyright 2011 Bart Trzynadlowski, Nik Henson
 **
 ** This file is part of Supermodel.
 **
 ** Supermodel is free software: you can redistribute it and/or modify it under
 ** the terms of the GNU General Public License as published by the Free 
 ** Software Foundation, either version 3 of the License, or (at your option)
 ** any later version.
 **
 ** Supermodel is distributed in the hope that it will be useful, but WITHOUT
 ** ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 ** FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 ** more details.
 **
 ** You should have received a copy of the GNU General Public License along
 ** with Supermodel.  If not, see <http://www.gnu.org/licenses/>.
 **/
 
/*
 * DirectInputSystem.h
 * 
 * Header file for DirectInput, XInput, and Raw Input input system.
 */

#ifndef INCLUDED_DIRECTINPUTSYSTEM_H
#define INCLUDED_DIRECTINPUTSYSTEM_H

#include "Types.h"
#include "Inputs/Input.h"
#include "Inputs/InputSource.h"
#include "Inputs/InputSystem.h"

#include <SDL.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#ifndef _XBOX_UWP
#include <dinput.h>
#endif

typedef struct DIJOYSTATE2 {
	LONG    lX;                     /* x-axis position              */
	LONG    lY;                     /* y-axis position              */
	LONG    lZ;                     /* z-axis position              */
	LONG    lRx;                    /* x-axis rotation              */
	LONG    lRy;                    /* y-axis rotation              */
	LONG    lRz;                    /* z-axis rotation              */
	LONG    rglSlider[2];           /* extra axes positions         */
	DWORD   rgdwPOV[4];             /* POV directions               */
	BYTE    rgbButtons[128];        /* 128 buttons                  */
	LONG    lVX;                    /* x-axis velocity              */
	LONG    lVY;                    /* y-axis velocity              */
	LONG    lVZ;                    /* z-axis velocity              */
	LONG    lVRx;                   /* x-axis angular velocity      */
	LONG    lVRy;                   /* y-axis angular velocity      */
	LONG    lVRz;                   /* z-axis angular velocity      */
	LONG    rglVSlider[2];          /* extra axes velocities        */
	LONG    lAX;                    /* x-axis acceleration          */
	LONG    lAY;                    /* y-axis acceleration          */
	LONG    lAZ;                    /* z-axis acceleration          */
	LONG    lARx;                   /* x-axis angular acceleration  */
	LONG    lARy;                   /* y-axis angular acceleration  */
	LONG    lARz;                   /* z-axis angular acceleration  */
	LONG    rglASlider[2];          /* extra axes accelerations     */
	LONG    lFX;                    /* x-axis force                 */
	LONG    lFY;                    /* y-axis force                 */
	LONG    lFZ;                    /* z-axis force                 */
	LONG    lFRx;                   /* x-axis torque                */
	LONG    lFRy;                   /* y-axis torque                */
	LONG    lFRz;                   /* z-axis torque                */
	LONG    rglFSlider[2];          /* extra axes forces            */
} DIJOYSTATE2, * LPDIJOYSTATE2;

#include <XInput.h>
#include <functional>

#include <vector>

#define NUM_DI_KEYS (sizeof(s_keyMap) / sizeof(DIKeyMapStruct))

#define DI_EFFECTS_MAX DI_FFNOMINALMAX
#define DI_EFFECTS_SCALE (DI_EFFECTS_MAX / 100)

#define XI_VIBRATE_MAX 65535
#define XI_VIBRATE_SCALE (XI_VIBRATE_MAX / 100)

struct DIKeyMapStruct
{
	const char *keyName;
	int diKey;
};

struct RawMseState
{
	LONG x;
	LONG y;
	LONG z;
	LONG wheelDelta;
	SHORT wheelDir;
	USHORT buttons;
};

struct DIMseState
{
	LONG x;
	LONG y;
	LONG z;
	SHORT wheelDir;
	BYTE buttons[5];
};

struct DIJoyInfo
{
	// DirectInput details
	GUID guid;
#ifndef _XBOX_UWP
	int dInputNum;
	LPDIRECTINPUTEFFECT dInputEffects[NUM_JOY_AXES][NUM_FF_EFFECTS];
#endif
	// XInput details
	bool isXInput;  // True if joystick is XInput controller
	int xInputNum;  // XInput controller number

	// XInput force feedback state
	WORD xiConstForceLeft;
	WORD xiConstForceRight;
	WORD xiVibrateBoth;
};

// RawInput API
typedef /*WINUSERAPI*/ INT (WINAPI *GetRawInputDeviceListPtr)(OUT PRAWINPUTDEVICELIST pRawInputDeviceList, IN OUT PUINT puiNumDevices, IN UINT cbSize);
typedef /*WINUSERAPI*/ INT (WINAPI *GetRawInputDeviceInfoPtr)(IN HANDLE hDevice, IN UINT uiCommand, OUT LPVOID pData, IN OUT PUINT pcbSize);
typedef /*WINUSERAPI*/ bool (WINAPI *RegisterRawInputDevicesPtr)(IN PCRAWINPUTDEVICE pRawInputDevices, IN UINT uiNumDevices, IN UINT cbSize);
typedef /*WINUSERAPI*/ INT (WINAPI *GetRawInputDataPtr)(IN HRAWINPUT hRawInput, IN UINT uiCommand, OUT LPVOID pData, IN OUT PUINT pcbSize, IN UINT cbSizeHeader);

// XInput API
typedef /*WINUSERAPI*/ DWORD (WINAPI *XInputGetCapabilitiesPtr)(IN DWORD dwUserIndex, IN DWORD dwFlags, OUT PXINPUT_CAPABILITIES pCapabilities);
typedef /*WINUSERAPI*/ DWORD (WINAPI *XInputGetStatePtr)(IN DWORD dwUserIndex, OUT PXINPUT_STATE pState);
typedef /*WINUSERAPI*/ DWORD (WINAPI *XInputSetStatePtr)(IN DWORD dwUserIndex, IN PXINPUT_VIBRATION pVibration);

/*
 * Input system that uses combination of DirectInput, XInput and RawInput APIs.
 */
class CDirectInputSystem : public CInputSystem
{
private:
  const Util::Config::Node &m_config;

	// Lookup table to map key names to DirectInput keycodes and Virtual keycodes
	static DIKeyMapStruct s_keyMap[];

	static const char *ConstructName(bool useRawInput, bool useXInput);

	bool m_useRawInput;
	bool m_useXInput;
	bool m_enableFFeedback;
	
	HWND m_hwnd;
	DWORD m_screenW;
	DWORD m_screenH;

	bool m_initializedCOM;
	bool m_activated;
	SDL_Window *m_window = nullptr;
	
	// Function pointers for RawInput API
	GetRawInputDeviceListPtr m_getRIDevListPtr;
	GetRawInputDeviceInfoPtr m_getRIDevInfoPtr;
	RegisterRawInputDevicesPtr m_regRIDevsPtr;
	GetRawInputDataPtr m_getRIDataPtr;
	
	// Keyboard, mouse and joystick details
	std::vector<KeyDetails> m_keyDetails;
	std::vector<MouseDetails> m_mseDetails;
	std::vector<JoyDetails> m_joyDetails;
	
	// RawInput keyboard and mice handles and states
	std::vector<HANDLE> m_rawKeyboards;
	std::vector<bool*> m_rawKeyStates;
	std::vector<HANDLE> m_rawMice;
	RawMseState m_combRawMseState;
	std::vector<RawMseState> m_rawMseStates;

	// Function pointers for XInput API
	XInputGetCapabilitiesPtr m_xiGetCapabilitiesPtr;
	XInputGetStatePtr m_xiGetStatePtr;
	XInputSetStatePtr m_xiSetStatePtr;

	// DirectInput pointers and details
#ifndef _XBOX_UWP
	LPDIRECTINPUT8 m_di8;
	LPDIRECTINPUTDEVICE8 m_di8Keyboard;
	LPDIRECTINPUTDEVICE8 m_di8Mouse;	
	std::vector<LPDIRECTINPUTDEVICE8> m_di8Joysticks;
#endif
	
	// DirectInput keyboard and mouse states
	BYTE m_diKeyState[256];
	DIMseState m_diMseState;
	
	// DirectInput joystick infos and states
	std::vector<DIJoyInfo> m_diJoyInfos;
	std::vector<DIJOYSTATE2> m_diJoyStates;

#ifndef _XBOX_UWP
	bool GetRegString(HKEY regKey, const char *regPath, std::string &str);

	bool GetRegDeviceName(const char *rawDevName, char *name);

	void OpenKeyboardsAndMice();

	void ActivateKeyboardsAndMice();

	void PollKeyboardsAndMice();

	void CloseKeyboardsAndMice();

	void ResetMice();
#endif

	void ProcessRawInput(HRAWINPUT hInput);

	void OpenJoysticks();

	void ActivateJoysticks();

	void PollJoysticks();

	void CloseJoysticks();
#ifndef _XBOX_UWP
	HRESULT CreateJoystickEffect(LPDIRECTINPUTDEVICE8 di8Joystick, int axisNum, ForceFeedbackCmd ffCmd, LPDIRECTINPUTEFFECT *di8Effect);
#endif
	void LoadXInputDLL();

protected:
	/*
	 * Initializes the DirectInput input system.
	 */
	bool InitializeSystem();
#ifndef _XBOX_UWP
	int GetKeyIndex(const char *keyName);

	const char *GetKeyName(int keyIndex);

	bool IsKeyPressed(int joyNum, int keyIndex);

	int GetMouseAxisValue(int mseNum, int axisNum);

	int GetMouseWheelDir(int mseNum);

	bool IsMouseButPressed(int mseNum, int butNum);
#else
	int GetKeyIndex(const char* keyName) { return -1; }

	const char* GetKeyName(int keyIndex) { return NULL; }

	bool IsKeyPressed(int joyNum, int keyIndex) { return false; }

	int GetMouseAxisValue(int mseNum, int axisNum) { return 0; }

	int GetMouseWheelDir(int mseNum) { return -1; }

	bool IsMouseButPressed(int mseNum, int butNum) { return false; }
#endif
	int GetJoyAxisValue(int joyNum, int axisNum);

	bool IsJoyPOVInDir(int joyNum, int povNum, int povDir);

	bool IsJoyButPressed(int joyNum, int butNum);
	
	bool ProcessForceFeedbackCmd(int joyNum, int axisNum, ForceFeedbackCmd ffCmd);
#ifndef _XBOX_UWP
	bool ConfigMouseCentered();

	CInputSource *CreateAnyMouseSource(EMousePart msePart);
#endif

public:
	/*
	 * Constructs a DirectInput/XInput/RawInput input system.  
	 * If useRawInput is true then RawInput is used for keyboard and mice movements (allowing multiple devices, eg for dual lightguns in gun
	 * games such as Lost World).  If false then DirectInput is used instead (which doesn't allow multiple devices).  In both cases,
	 * DirectInput/XInput is used for reading joysticks.
	 * If useXInput is true then XInput is used for reading XBox 360 game controllers (and/or XInput compatible joysticks) and DirectInput is used
	 * for all other types of joystick.  If false, then DirectInput is used for reading all joysticks (including XBox 360 ones).
	 * The advantage of using XInput for XBox 360 game controllers is that it allows the left and right triggers to be used simultaneously
	 * (ie to brake and accelerate at the same time in order to power slide the car in Daytona USA 2).  Under DirectInput the triggers get mapped
	 * to the same shared axis and so cannot be distinguished when pressed together.
	 * If enableFFeedback is true then force feedback is enabled (for those joysticks which are force feedback capable).
	 */
	CDirectInputSystem(const Util::Config::Node &config, SDL_Window *window, bool useRawInput, bool useXInput);

	~CDirectInputSystem();
#ifndef _XBOX_UWP
	int GetNumKeyboards();	

	int GetNumMice();
#else
	int GetNumKeyboards() { return 0; }

	int GetNumMice() { return 0; } 
#endif
	int GetNumJoysticks();
#ifndef _XBOX_UWP
	const KeyDetails *GetKeyDetails(int kbdNum);

	const MouseDetails *GetMouseDetails(int mseNum);
#else
	const KeyDetails* GetKeyDetails(int kbdNum) { return NULL; }

	const MouseDetails* GetMouseDetails(int mseNum) { return NULL; }
#endif
	const JoyDetails *GetJoyDetails(int joyNum);

	bool Poll();
#ifndef _XBOX_UWP
	void GrabMouse();

	void UngrabMouse();

	void SetMouseVisibility(bool visible);
#else
	void SetMouseVisibility(bool visible) {}
#endif
};

#endif	// INCLUDED_DIRECTINPUTSYSTEM_H
