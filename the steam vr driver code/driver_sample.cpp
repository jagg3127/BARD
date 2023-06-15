//============ Copyright (c) Valve Corporation, All rights reserved. ============
//===================== Half-Life: Alyx novr mod / driver =======================

#include "C:/VRFILES/openvr_driver.h"
//#include "driverlog.h"

#include <vector>
#include <thread>
#include <chrono>

#include <windows.h>
#include <atlbase.h>
#include "IniReader\IniReader.h"

using namespace vr;
using namespace std::chrono;

#define HMD_DLL_EXPORT extern "C" __declspec( dllexport )
#define HMD_DLL_IMPORT extern "C" __declspec( dllimport )
#define GRIP_BTN	   0x0001
#define THUMB_BTN	   0x0002
#define A_BTN          0x0004
#define B_BTN    	   0x0008
#define MENU_BTN	   0x0010
#define SYS_BTN		   0x0020
#define StepPos        0.01; //0.0033;
#define StepRot        0.4;

inline HmdQuaternion_t HmdQuaternion_Init( double w, double x, double y, double z )
{
	HmdQuaternion_t quat;
	quat.w = w;
	quat.x = x;
	quat.y = y;
	quat.z = z;
	return quat;
}

inline void HmdMatrix_SetIdentity( HmdMatrix34_t *pMatrix )
{
	pMatrix->m[0][0] = 1.f;
	pMatrix->m[0][1] = 0.f;
	pMatrix->m[0][2] = 0.f;
	pMatrix->m[0][3] = 0.f;
	pMatrix->m[1][0] = 0.f;
	pMatrix->m[1][1] = 1.f;
	pMatrix->m[1][2] = 0.f;
	pMatrix->m[1][3] = 0.f;
	pMatrix->m[2][0] = 0.f;
	pMatrix->m[2][1] = 0.f;
	pMatrix->m[2][2] = 1.f;
	pMatrix->m[2][3] = 0.f;
}

int KeyNameToKeyCode(std::string KeyName) {
	std::transform(KeyName.begin(), KeyName.end(), KeyName.begin(), ::toupper);

	if (KeyName == "NONE") return 0;

	else if (KeyName == "MOUSE-LEFT-BTN") return VK_LBUTTON;
	else if (KeyName == "MOUSE-RIGHT-BTN") return VK_RBUTTON;
	else if (KeyName == "MOUSE-MIDDLE-BTN") return VK_MBUTTON;
	else if (KeyName == "MOUSE-SIDE1-BTN") return VK_XBUTTON1;
	else if (KeyName == "MOUSE-SIDE2-BTN") return VK_XBUTTON2;

	else if (KeyName == "ESCAPE") return VK_ESCAPE;
	else if (KeyName == "F1")  return VK_F1;
	else if (KeyName == "F2")  return VK_F2;
	else if (KeyName == "F3")  return VK_F3;
	else if (KeyName == "F4")  return VK_F4;
	else if (KeyName == "F5")  return VK_F5;
	else if (KeyName == "F6")  return VK_F6;
	else if (KeyName == "F7")  return VK_F7;
	else if (KeyName == "F8")  return VK_F8;
	else if (KeyName == "F9")  return VK_F9;
	else if (KeyName == "F10") return VK_F10;
	else if (KeyName == "F11") return VK_F11;
	else if (KeyName == "F12") return VK_F12;
	else if (KeyName == "F13") return VK_F13;
	else if (KeyName == "F14") return VK_F14;
	else if (KeyName == "F15") return VK_F15;
	else if (KeyName == "F16") return VK_F16;
	else if (KeyName == "F17") return VK_F17;
	else if (KeyName == "F18") return VK_F18;
	else if (KeyName == "F19") return VK_F19;
	else if (KeyName == "F20") return VK_F20;
	else if (KeyName == "F21") return VK_F21;
	else if (KeyName == "F22") return VK_F22;
	else if (KeyName == "F23") return VK_F23;
	else if (KeyName == "F24") return VK_F24;

	else if (KeyName == "~") return 192;
	else if (KeyName == "1") return '1';
	else if (KeyName == "2") return '2';
	else if (KeyName == "3") return '3';
	else if (KeyName == "4") return '4';
	else if (KeyName == "5") return '5';
	else if (KeyName == "6") return '6';
	else if (KeyName == "7") return '7';
	else if (KeyName == "8") return '8';
	else if (KeyName == "9") return '9';
	else if (KeyName == "0") return '0';
	else if (KeyName == "-") return 189;
	else if (KeyName == "=") return 187;

	else if (KeyName == "TAB") return VK_TAB;
	else if (KeyName == "CAPS-LOCK") return VK_CAPITAL;
	else if (KeyName == "SHIFT") return VK_SHIFT;
	else if (KeyName == "CTRL") return VK_CONTROL;
	else if (KeyName == "RCTRL") return VK_RCONTROL;
	else if (KeyName == "WIN") return VK_LWIN;
	else if (KeyName == "ALT") return VK_MENU;
	else if (KeyName == "SPACE") return VK_SPACE;
	else if (KeyName == "ENTER") return VK_RETURN;
	else if (KeyName == "BACKSPACE") return VK_BACK;

	else if (KeyName == "Q") return 'Q';
	else if (KeyName == "W") return 'W';
	else if (KeyName == "E") return 'E';
	else if (KeyName == "R") return 'R';
	else if (KeyName == "T") return 'T';
	else if (KeyName == "Y") return 'Y';
	else if (KeyName == "U") return 'U';
	else if (KeyName == "I") return 'I';
	else if (KeyName == "O") return 'O';
	else if (KeyName == "P") return 'P';
	else if (KeyName == "[") return 219;
	else if (KeyName == "]") return 221;
	else if (KeyName == "A") return 'A';
	else if (KeyName == "S") return 'S';
	else if (KeyName == "D") return 'D';
	else if (KeyName == "F") return 'F';
	else if (KeyName == "G") return 'G';
	else if (KeyName == "H") return 'H';
	else if (KeyName == "J") return 'J';
	else if (KeyName == "K") return 'K';
	else if (KeyName == "L") return 'L';
	else if (KeyName == ":") return 186;
	else if (KeyName == "APOSTROPHE") return 222;
	else if (KeyName == "\\") return 220;
	else if (KeyName == "Z") return 'Z';
	else if (KeyName == "X") return 'X';
	else if (KeyName == "C") return 'C';
	else if (KeyName == "V") return 'V';
	else if (KeyName == "B") return 'B';
	else if (KeyName == "N") return 'N';
	else if (KeyName == "M") return 'M';
	else if (KeyName == "<") return 188;
	else if (KeyName == ">") return 190;
	else if (KeyName == "?") return 191;

	else if (KeyName == "PRINTSCREEN") return VK_SNAPSHOT;
	else if (KeyName == "SCROLL-LOCK") return VK_SCROLL;
	else if (KeyName == "PAUSE") return VK_PAUSE;
	else if (KeyName == "INSERT") return VK_INSERT;
	else if (KeyName == "HOME") return VK_HOME;
	else if (KeyName == "PAGE-UP") return VK_NEXT;
	else if (KeyName == "DELETE") return VK_DELETE;
	else if (KeyName == "END") return VK_END;
	else if (KeyName == "PAGE-DOWN") return VK_PRIOR;

	else if (KeyName == "UP") return VK_UP;
	else if (KeyName == "DOWN") return VK_DOWN;
	else if (KeyName == "LEFT") return VK_LEFT;
	else if (KeyName == "RIGHT") return VK_RIGHT;

	else if (KeyName == "NUM-LOCK") return VK_NUMLOCK;
	else if (KeyName == "NUMPAD0") return VK_NUMPAD0;
	else if (KeyName == "NUMPAD1") return VK_NUMPAD1;
	else if (KeyName == "NUMPAD2") return VK_NUMPAD2;
	else if (KeyName == "NUMPAD3") return VK_NUMPAD3;
	else if (KeyName == "NUMPAD4") return VK_NUMPAD4;
	else if (KeyName == "NUMPAD5") return VK_NUMPAD5;
	else if (KeyName == "NUMPAD6") return VK_NUMPAD6;
	else if (KeyName == "NUMPAD7") return VK_NUMPAD7;
	else if (KeyName == "NUMPAD8") return VK_NUMPAD8;
	else if (KeyName == "NUMPAD9") return VK_NUMPAD9;

	else if (KeyName == "NUMPAD-DIVIDE") return VK_DIVIDE;
	else if (KeyName == "NUMPAD-MULTIPLY") return VK_MULTIPLY;
	else if (KeyName == "NUMPAD-MINUS") return VK_SUBTRACT;
	else if (KeyName == "NUMPAD-PLUS") return VK_ADD;
	else if (KeyName == "NUMPAD-DEL") return VK_DECIMAL;

	else return 0;
}

// keys for use with the settings API
static const char * const k_pch_Sample_Section = "hlalyx"; static const char * const k_pch_Sample_SerialNumber_String = "serialNumber"; static const char * const k_pch_Sample_ModelNumber_String = "modelNumber"; static const char * const k_pch_Sample_WindowX_Int32 = "windowX"; static const char * const k_pch_Sample_WindowY_Int32 = "windowY"; static const char * const k_pch_Sample_WindowWidth_Int32 = "windowWidth"; static const char * const k_pch_Sample_WindowHeight_Int32 = "windowHeight"; static const char * const k_pch_Sample_RenderWidth_Int32 = "renderWidth"; static const char * const k_pch_Sample_RenderHeight_Int32 = "renderHeight"; static const char * const k_pch_Sample_SecondsFromVsyncToPhotons_Float = "secondsFromVsyncToPhotons"; static const char * const k_pch_Sample_DisplayFrequency_Float = "displayFrequency"; static const char * const k_pch_Sample_ZoomWidth_Float = "ZoomWidth"; static const char * const k_pch_Sample_ZoomHeight_Float = "ZoomHeight"; static const char * const k_pch_Sample_FOV_Float = "FOV"; static const char * const k_pch_Sample_Stereo_Bool = "Stereo"; static const char * const k_pch_Sample_DebugMode_Bool = "DebugMode";

//Velocity
double FirstCtrlLastPos[3] = { 0, 0, 0 }, SecondCtrlLastPos[3] = { 0, 0, 0 }; milliseconds deltaTime;

//Half Life Alyx
typedef struct _HMDData
{
	double	X;
	double	Y;
	double	Z;
	double	Yaw;
	double	Pitch;
	double	Roll;
} THMD, *PHMD; typedef struct _Controller
{
	double	X;
	double	Y;
	double	Z;
	double	Yaw;
	double	Pitch;
	double	Roll;
	unsigned short	Buttons;
	float	Trigger;
	float	AxisX;
	float	AxisY;
} TController, *PController;
THMD           MyHMD; 
THMD           mouseHMD;                              
TController    MyCtrl; 
TController    MyCtrl2;
bool           HMDConnected = false, ctrlsConnected = false;

double DegToRad(double f) {
	return f * (3.14159265358979323846 / 180);
}

//Key bindings
float IX_MOVEMENT_SPEED;
float IX_HAND_SPEED; //0.8 
float IX_CROUCH_DIST; //0.8 

int   IX_crouch;

//HMD
int IX_HMD_xp; //right
int IX_HMD_yp; //foward
int IX_HMD_zp; //up

int IX_HMD_xn; //left
int IX_HMD_yn; //backwards
int IX_HMD_zn; //down


//LEFT CONTROL
int IX_CTL_left_xp; //right
int IX_CTL_left_yp; //foward
int IX_CTL_left_zp; //up 

int IX_CTL_left_xn; //left
int IX_CTL_left_yn; //backwards
int IX_CTL_left_zn; //down

int IX_CTL_left_f;
int IX_CTL_left_b;
int IX_CTL_left_l;
int IX_CTL_left_r;

int IX_CTL_left_trigger;
int IX_CTL_left_btn_thumb;
int IX_CTL_left_btn_menu;

int IX_CTL_left_reset_r;
int IX_CTL_left_reset_xyz;

//RIGHT CONTROL
int IX_CTL_right_xp; //right
int IX_CTL_right_yp; //foward
int IX_CTL_right_zp; //up

int IX_CTL_right_xn; //left
int IX_CTL_right_yn; //backwards
int IX_CTL_right_zn; //down

int IX_CTL_right_f;
int IX_CTL_right_b;
int IX_CTL_right_l;
int IX_CTL_right_r;

int IX_CTL_right_trigger;
int IX_CTL_right_btn_thumb;
int IX_CTL_right_btn_menu;

int IX_CTL_right_reset_r;
int IX_CTL_right_reset_xyz;

//end key bindings


double HMDPosX = 0; //left/right
double HMDPosY = 0; //foward/back
double HMDPosZ = 0; //up/down

double FirstCtrlPos[3] = { 0, 0, 0 }, SecondCtrlPos[3] = { 0, 0, 0 };
double CtrlsRollRight = 0, CtrlsPitchRight = 0;
double CtrlsRollLeft  = 0, CtrlsPitchLeft  = 0;
float  CrouchOffsetZ = 0;

bool firstCP = true;
int m_HalfWidth = 1920 / 2;
int m_HalfHeight = 1080 / 2;
float mouseSensetivePitch = 0.04;
float mouseSensetiveYaw = 0.05;
float mouseYaw = 0, mousePitch = 0;

HWND SteamVRHeadsetWindow = 0;
HWND HalfLifeAlyxWindow = 0;
bool HeadsetWindowFocused = false;
bool HalfLifeAlyxFocused = false;
bool VRMode = false;
float VRAimingAngleOffset;

float oldPitch = 0, deltaPitch = 0;
float oldRoll = 0, deltaRoll = 0;
double CtrlPosOffset[3] = { 0, 0, 0 };

//For fix left controller
double FirstCtrlLastXYZ[3] = { 0, 0, 0 };
double FirstCtrlLastYPR[3] = { 0, 0, 0 };

double SecondCtrlYPROffset[3] = { 0, 0, 0 };
double SecondCtrlLastYPR[3] = { 0, 0, 0 };


float LeftHandYaw = 0, LeftHandPitch = 0, LeftAngleDist = 0, LeftRadDist = 0, LeftYOffset = 0;
float RightAngleDist = 0; //�������, ������� ���������� ����
float RightHandYaw = 0, RightHandPitch = 0, RightRadDist = 0;

float HandRadDist = 0;

//Motion
float MotionLeftRadDist = 0, MotionLeftAngleDist = 0;


int GranadeTick = 0;
bool UseGranade = false;

bool Aiming = false;
bool CoverMouth = false;
bool PutHat = false;
bool LaserMode = false;
float LaserPos = 0, LaserYaw = 0;

bool UsedEnergyBall = false;

#define OffsetPitchByDefault -45
float ZPosSensetive = 0.009;

double OffsetYPR(float f, float f2)
{
	f -= f2;
	if (f < -180)
		f += 360;
	else if (f > 180)
		f -= 360;

	return f;
}

void MouseToYawPitch()
{
	POINT mousePos;

	if (firstCP) {
		SetCursorPos(m_HalfWidth, m_HalfHeight);
		firstCP = false;
	}

	GetCursorPos(&mousePos);

	mouseYaw += (mousePos.x - m_HalfWidth) * mouseSensetiveYaw * -1;
	if (mouseYaw < -180)
		mouseYaw += 360;
	else if (mouseYaw > 180)
		mouseYaw -= 360;

	mouseHMD.Yaw = mouseYaw;

	mousePitch += (mousePos.y - m_HalfHeight) * mouseSensetivePitch * -1;
	if (mousePitch > 90)
		mousePitch = 90;
	else if (mousePitch < -90)
		mousePitch = -90;
	mouseHMD.Pitch = mousePitch;

	SetCursorPos(m_HalfWidth, m_HalfHeight);
}

bool key(int keyname) {
	return (GetAsyncKeyState(keyname) & 0x8000) != 0;
}

void GetHMDData(__out THMD *HMD)
{
	
	if (key(IX_HMD_xp)) {
		HMDPosX += IX_MOVEMENT_SPEED;
	}
	if (key(IX_HMD_yp)) {
		HMDPosY += IX_MOVEMENT_SPEED;
	}


	if (key(IX_HMD_xn)) {
		HMDPosX -= IX_MOVEMENT_SPEED;
	}
	if (key(IX_HMD_yn)) {
		HMDPosY -= IX_MOVEMENT_SPEED;
	}


	HMD->X = HMDPosX;
	HMD->Y = HMDPosY;
	HMD->Z = HMDPosZ - CrouchOffsetZ;

	HMD->Yaw   = mouseHMD.Yaw * -1;
	HMD->Pitch = mouseHMD.Pitch * -1;
	HMD->Roll  = 0;  
}

void GetControllersData(__out TController *FirstController, __out TController *SecondController)
{
	//Controllers default state 
	FirstController->AxisX     = 0;
	FirstController->AxisY     = 0;
	FirstController->Buttons   = 0;
	FirstController->Trigger   = 0;

	SecondController->AxisX    = 0;
	SecondController->AxisY    = 0;
	SecondController->Buttons  = 0;
	SecondController->Trigger  = 0;


	//Default
	LeftHandYaw = 0;
	LeftHandPitch = 0;
	LeftAngleDist = 0;
	LeftRadDist = 0;

	RightHandYaw = 0;
	RightHandPitch = 0;
	RightAngleDist = 0;
	RightRadDist = 0;



	

	//Left hand setup
	if (key(IX_CTL_left_reset_xyz)) {
		FirstCtrlPos[0] = 0;
		FirstCtrlPos[1] = 0;
		FirstCtrlPos[2] = 0;
	} else {
		if (key(IX_CTL_left_xp)) {
			FirstCtrlPos[0] += IX_HAND_SPEED;
		}
		if (key(IX_CTL_left_yp)) {
			FirstCtrlPos[1] += IX_HAND_SPEED;
		}
		if (key(IX_CTL_left_zp)) {
			FirstCtrlPos[2] += IX_HAND_SPEED;
		}

		if (key(IX_CTL_left_xn)) {
			FirstCtrlPos[0] -= IX_HAND_SPEED;
		}
		if (key(IX_CTL_left_yn)) {
			FirstCtrlPos[1] -= IX_HAND_SPEED;
		}
		if (key(IX_CTL_left_zn)) {
			FirstCtrlPos[2] -= IX_HAND_SPEED;
		}

		if (key(IX_CTL_left_btn_menu)) {
			FirstController->Buttons |= MENU_BTN;
		}

		if (key(IX_CTL_left_btn_thumb)) {
			FirstController->Buttons |= THUMB_BTN;
		}

		if (key(IX_CTL_left_trigger)) {
			FirstController->Trigger = 1;
		}
		else {
			FirstController->Trigger = 0;
		}
	}
	

	//Right hand setup
	if (key(IX_CTL_right_reset_xyz)) {
		SecondCtrlPos[0] = 0;
		SecondCtrlPos[1] = 0;
		SecondCtrlPos[2] = 0;
	} else {
		if (key(IX_CTL_right_xp)) {
			SecondCtrlLastPos[0] += IX_HAND_SPEED;
		}
		if (key(IX_CTL_right_yp)) {
			SecondCtrlLastPos[1] += IX_HAND_SPEED;
		}
		if (key(IX_CTL_right_zp)) {
			SecondCtrlLastPos[2] += IX_HAND_SPEED;
		}

		if (key(IX_CTL_right_xn)) {
			SecondCtrlLastPos[0] -= IX_HAND_SPEED;
		}
		if (key(IX_CTL_right_yn)) {
			SecondCtrlLastPos[1] -= IX_HAND_SPEED;
		}
		if (key(IX_CTL_right_zn)) {
			SecondCtrlLastPos[2] -= IX_HAND_SPEED;
		}

		if (key(IX_CTL_right_btn_menu)) {
			SecondController->Buttons |= MENU_BTN;
		}

		if (key(IX_CTL_right_btn_thumb)) {
			SecondController->Buttons |= THUMB_BTN;
		}

		if (key(IX_CTL_right_trigger)) {
			SecondController->Trigger = 1;
		}
		else {
			SecondController->Trigger = 0;
		}
	}

	//Crouch
	if (key(IX_crouch)) {
		HMDPosZ = 0;
		CrouchOffsetZ = IX_CROUCH_DIST;
	} else {
		CrouchOffsetZ = 0;
	}	


	FirstController->X = FirstCtrlPos[0];
	FirstController->Y = FirstCtrlPos[1];
	FirstController->Z = FirstCtrlPos[2] - 0.15;

	SecondController->X = SecondCtrlPos[0];
	SecondController->Y = SecondCtrlPos[1];
	SecondController->Z = SecondCtrlPos[2] - 0.15;

	//Rotation controllers
	FirstController->Roll  = CtrlsRollLeft;
	FirstController->Yaw   = OffsetYPR(mouseHMD.Yaw, LeftHandYaw);
	FirstController->Pitch = OffsetYPR(OffsetYPR(OffsetYPR(mouseHMD.Pitch, OffsetPitchByDefault), LeftHandPitch), CtrlsPitchLeft);



	SecondController->Roll  = CtrlsRollRight;
	SecondController->Yaw   = OffsetYPR(mouseHMD.Yaw, RightHandYaw);
	SecondController->Pitch = OffsetYPR(OffsetYPR(OffsetYPR(mouseHMD.Pitch, OffsetPitchByDefault), RightHandPitch), CtrlsPitchRight);

	



	/*//New version of SteamVR driver with correct axes 
	FirstController->Yaw    = FirstController->Yaw    * -1;
	FirstController->Pitch  = FirstController->Pitch  * -1;

	SecondController->Yaw   = SecondController->Yaw   * -1;
	SecondController->Pitch = SecondController->Pitch * -1;*/
}


inline vr::HmdQuaternion_t EulerAngleToQuaternion(double Yaw, double Pitch, double Roll)
{
	vr::HmdQuaternion_t q;
	// Abbreviations for the various angular functions
	double cy = cos(Yaw * 0.5);
	double sy = sin(Yaw * 0.5);
	double cp = cos(Pitch * 0.5);
	double sp = sin(Pitch * 0.5);
	double cr = cos(Roll * 0.5);
	double sr = sin(Roll * 0.5);

	q.w = cr * cp * cy + sr * sp * sy;
	q.x = sr * cp * cy - cr * sp * sy;
	q.y = cr * sp * cy + sr * cp * sy;
	q.z = cr * cp * sy - sr * sp * cy;

	return q;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
class CSampleDeviceDriver : public vr::ITrackedDeviceServerDriver, public vr::IVRDisplayComponent
{
public:
	CSampleDeviceDriver(  )
	{
		HMDIndex_t = vr::k_unTrackedDeviceIndexInvalid;
		m_ulPropertyContainer = vr::k_ulInvalidPropertyContainer;

		//DriverLog( "Using settings values\n" );
		m_flIPD = vr::VRSettings()->GetFloat( k_pch_SteamVR_Section, k_pch_SteamVR_IPD_Float );

		char buf[1024];
		vr::VRSettings()->GetString( k_pch_Sample_Section, k_pch_Sample_SerialNumber_String, buf, sizeof( buf ) );
		m_sSerialNumber = buf;

		vr::VRSettings()->GetString( k_pch_Sample_Section, k_pch_Sample_ModelNumber_String, buf, sizeof( buf ) );
		m_sModelNumber = buf;

		m_nWindowX = vr::VRSettings()->GetInt32( k_pch_Sample_Section, k_pch_Sample_WindowX_Int32 );
		m_nWindowY = vr::VRSettings()->GetInt32( k_pch_Sample_Section, k_pch_Sample_WindowY_Int32 );
		m_nWindowWidth = vr::VRSettings()->GetInt32( k_pch_Sample_Section, k_pch_Sample_WindowWidth_Int32 );
		m_nWindowHeight = vr::VRSettings()->GetInt32( k_pch_Sample_Section, k_pch_Sample_WindowHeight_Int32 );

		if (m_nWindowWidth == 0)
			m_nWindowWidth = GetSystemMetrics(SM_CXSCREEN);
		if (m_nWindowHeight == 0)
			m_nWindowHeight = GetSystemMetrics(SM_CYSCREEN);

		m_nRenderWidth = vr::VRSettings()->GetInt32( k_pch_Sample_Section, k_pch_Sample_RenderWidth_Int32 );
		m_nRenderHeight = vr::VRSettings()->GetInt32( k_pch_Sample_Section, k_pch_Sample_RenderHeight_Int32 );

		m_flSecondsFromVsyncToPhotons = vr::VRSettings()->GetFloat( k_pch_Sample_Section, k_pch_Sample_SecondsFromVsyncToPhotons_Float );
		m_flDisplayFrequency = vr::VRSettings()->GetFloat( k_pch_Sample_Section, k_pch_Sample_DisplayFrequency_Float );

		m_fZoomWidth = vr::VRSettings()->GetFloat(k_pch_Sample_Section, k_pch_Sample_ZoomWidth_Float);
		m_fZoomHeight = vr::VRSettings()->GetFloat(k_pch_Sample_Section, k_pch_Sample_ZoomHeight_Float);
		m_fFOV = (vr::VRSettings()->GetFloat(k_pch_Sample_Section, k_pch_Sample_FOV_Float) * 3.14159265358979323846 / 180); //radians
		m_bStereoMode = vr::VRSettings()->GetBool(k_pch_Sample_Section, k_pch_Sample_Stereo_Bool);
		m_bDebugMode = vr::VRSettings()->GetBool(k_pch_Sample_Section, k_pch_Sample_DebugMode_Bool);

		/*DriverLog( "driver_null: Serial Number: %s\n", m_sSerialNumber.c_str() );
		DriverLog( "driver_null: Model Number: %s\n", m_sModelNumber.c_str() );
		DriverLog( "driver_null: Window: %d %d %d %d\n", m_nWindowX, m_nWindowY, m_nWindowWidth, m_nWindowHeight );
		DriverLog( "driver_null: Render Target: %d %d\n", m_nRenderWidth, m_nRenderHeight );
		DriverLog( "driver_null: Seconds from Vsync to Photons: %f\n", m_flSecondsFromVsyncToPhotons );
		DriverLog( "driver_null: Display Frequency: %f\n", m_flDisplayFrequency );
		DriverLog( "driver_null: IPD: %f\n", m_flIPD );*/

	}

	virtual ~CSampleDeviceDriver()
	{
	}


	virtual EVRInitError Activate( vr::TrackedDeviceIndex_t unObjectId ) 
	{
		HMDIndex_t = unObjectId;
		m_ulPropertyContainer = vr::VRProperties()->TrackedDeviceToPropertyContainer( HMDIndex_t );

		vr::VRProperties()->SetStringProperty( m_ulPropertyContainer, Prop_ModelNumber_String, m_sModelNumber.c_str() );
		vr::VRProperties()->SetStringProperty( m_ulPropertyContainer, Prop_RenderModelName_String, m_sModelNumber.c_str() );
		vr::VRProperties()->SetFloatProperty( m_ulPropertyContainer, Prop_UserIpdMeters_Float, m_flIPD );
		vr::VRProperties()->SetFloatProperty( m_ulPropertyContainer, Prop_UserHeadToEyeDepthMeters_Float, 0.f );
		vr::VRProperties()->SetFloatProperty( m_ulPropertyContainer, Prop_DisplayFrequency_Float, m_flDisplayFrequency );
		vr::VRProperties()->SetFloatProperty( m_ulPropertyContainer, Prop_SecondsFromVsyncToPhotons_Float, m_flSecondsFromVsyncToPhotons );

		// return a constant that's not 0 (invalid) or 1 (reserved for Oculus)
		vr::VRProperties()->SetUint64Property( m_ulPropertyContainer, Prop_CurrentUniverseId_Uint64, 2 );

		// avoid "not fullscreen" warnings from vrmonitor
		vr::VRProperties()->SetBoolProperty( m_ulPropertyContainer, Prop_IsOnDesktop_Bool, false );

		//Debug mode activate Windowed Mode (borderless fullscreen), lock to 30 FPS 
		vr::VRProperties()->SetBoolProperty(m_ulPropertyContainer, Prop_DisplayDebugMode_Bool, m_bDebugMode);

		// Icons can be configured in code or automatically configured by an external file "drivername\resources\driver.vrresources".
		// Icon properties NOT configured in code (post Activate) are then auto-configured by the optional presence of a driver's "drivername\resources\driver.vrresources".
		// In this manner a driver can configure their icons in a flexible data driven fashion by using an external file.
		//
		// The structure of the driver.vrresources file allows a driver to specialize their icons based on their HW.
		// Keys matching the value in "Prop_ModelNumber_String" are considered first, since the driver may have model specific icons.
		// An absence of a matching "Prop_ModelNumber_String" then considers the ETrackedDeviceClass ("HMD", "Controller", "GenericTracker", "TrackingReference")
		// since the driver may have specialized icons based on those device class names.
		//
		// An absence of either then falls back to the "system.vrresources" where generic device class icons are then supplied.
		//
		// Please refer to "bin\drivers\sample\resources\driver.vrresources" which contains this sample configuration.
		//
		// "Alias" is a reserved key and specifies chaining to another json block.
		//
		// In this sample configuration file (overly complex FOR EXAMPLE PURPOSES ONLY)....
		//
		// "Model-v2.0" chains through the alias to "Model-v1.0" which chains through the alias to "Model-v Defaults".
		//
		// Keys NOT found in "Model-v2.0" would then chase through the "Alias" to be resolved in "Model-v1.0" and either resolve their or continue through the alias.
		// Thus "Prop_NamedIconPathDeviceAlertLow_String" in each model's block represent a specialization specific for that "model".
		// Keys in "Model-v Defaults" are an example of mapping to the same states, and here all map to "Prop_NamedIconPathDeviceOff_String".
		//
		/*bool bSetupIconUsingExternalResourceFile = true;
		if ( !bSetupIconUsingExternalResourceFile )
		{
			// Setup properties directly in code.
			// Path values are of the form {drivername}\icons\some_icon_filename.png
			vr::VRProperties()->SetStringProperty( m_ulPropertyContainer, vr::Prop_NamedIconPathDeviceOff_String, "{null}/icons/headset_sample_status_off.png" );
			vr::VRProperties()->SetStringProperty( m_ulPropertyContainer, vr::Prop_NamedIconPathDeviceSearching_String, "{null}/icons/headset_sample_status_searching.gif" );
			vr::VRProperties()->SetStringProperty( m_ulPropertyContainer, vr::Prop_NamedIconPathDeviceSearchingAlert_String, "{null}/icons/headset_sample_status_searching_alert.gif" );
			vr::VRProperties()->SetStringProperty( m_ulPropertyContainer, vr::Prop_NamedIconPathDeviceReady_String, "{null}/icons/headset_sample_status_ready.png" );
			vr::VRProperties()->SetStringProperty( m_ulPropertyContainer, vr::Prop_NamedIconPathDeviceReadyAlert_String, "{null}/icons/headset_sample_status_ready_alert.png" );
			vr::VRProperties()->SetStringProperty( m_ulPropertyContainer, vr::Prop_NamedIconPathDeviceNotReady_String, "{null}/icons/headset_sample_status_error.png" );
			vr::VRProperties()->SetStringProperty( m_ulPropertyContainer, vr::Prop_NamedIconPathDeviceStandby_String, "{null}/icons/headset_sample_status_standby.png" );
			vr::VRProperties()->SetStringProperty( m_ulPropertyContainer, vr::Prop_NamedIconPathDeviceAlertLow_String, "{null}/icons/headset_sample_status_ready_low.png" );
		}*/

		return VRInitError_None;
	}

	virtual void Deactivate() 
	{
		HMDIndex_t = vr::k_unTrackedDeviceIndexInvalid;
	}

	virtual void EnterStandby()
	{
	}

	void *GetComponent( const char *pchComponentNameAndVersion )
	{
		if ( !_stricmp( pchComponentNameAndVersion, vr::IVRDisplayComponent_Version ) )
		{
			return (vr::IVRDisplayComponent*)this;
		}

		// override this to add a component to a driver
		return NULL;
	}

	virtual void PowerOff() 
	{
	}

	/** debug request from a client */
	virtual void DebugRequest( const char *pchRequest, char *pchResponseBuffer, uint32_t unResponseBufferSize ) 
	{
		if( unResponseBufferSize >= 1 )
			pchResponseBuffer[0] = 0;
	}

	virtual void GetWindowBounds( int32_t *pnX, int32_t *pnY, uint32_t *pnWidth, uint32_t *pnHeight ) 
	{
		*pnX = m_nWindowX;
		*pnY = m_nWindowY;
		*pnWidth = m_nWindowWidth;
		*pnHeight = m_nWindowHeight;
	}

	virtual bool IsDisplayOnDesktop() 
	{
		return true;
	}

	virtual bool IsDisplayRealDisplay() 
	{
		return true;
	}

	virtual void GetRecommendedRenderTargetSize( uint32_t *pnWidth, uint32_t *pnHeight ) 
	{
		*pnWidth = m_nRenderWidth;
		*pnHeight = m_nRenderHeight;
	}

	virtual void GetEyeOutputViewport( EVREye eEye, uint32_t *pnX, uint32_t *pnY, uint32_t *pnWidth, uint32_t *pnHeight ) 
	{
		/**pnY = 0;
				*pnWidth = m_nWindowWidth / 2;
				*pnHeight = m_nWindowHeight;

				if ( eEye == Eye_Left )
				{
					*pnX = 0;
				}
				else
				{
					*pnX = m_nWindowWidth / 2;
				}*/

		if (m_bStereoMode) {

			*pnY = 0;
			*pnWidth = m_nWindowWidth / 2;
			*pnHeight = m_nWindowHeight;

			if (eEye == Eye_Left)
			{
				*pnX = 0;
			}
			else
			{
				*pnX = (m_nWindowWidth / 2);
			}
		}
		else { //Mono mode
			pnY = 0;
			*pnWidth = m_nRenderWidth;
			*pnHeight = m_nRenderHeight;

			if (eEye == Eye_Left)
			{
				*pnX = (m_nWindowWidth - m_nRenderWidth) / 2;
			}
			else
			{
				*pnX = m_nWindowWidth;
			}


		}
	}

	virtual void GetProjectionRaw( EVREye eEye, float *pfLeft, float *pfRight, float *pfTop, float *pfBottom ) 
	{
		if (m_bStereoMode) {
			
			*pfLeft = -m_fFOV;
			*pfRight = m_fFOV;
			*pfTop = -m_fFOV;
			*pfBottom = m_fFOV;
		}
		else { //Mono
			*pfLeft = (float)m_nRenderWidth / m_nRenderHeight * -1;
			*pfRight = (float)m_nRenderWidth / m_nRenderHeight;
			*pfTop = -1.0;
			*pfBottom = 1.0;
		}
	}

	virtual DistortionCoordinates_t ComputeDistortion( EVREye eEye, float fU, float fV ) 
	{
		DistortionCoordinates_t coordinates;

		//distortion for lens from https://github.com/HelenXR/openvr_survivor/blob/master/src/head_mount_display_device.cc
		float hX;
		float hY;
		double rr;
		double r2;
		double theta;

		rr = sqrt((fU - 0.5f)*(fU - 0.5f) + (fV - 0.5f)*(fV - 0.5f));
		r2 = rr * (1 + 0 * (rr*rr) + 0 * (rr*rr*rr*rr));
		theta = atan2(fU - 0.5f, fV - 0.5f);
		hX = sin(theta)*r2*m_fZoomWidth;
		hY = cos(theta)*r2*m_fZoomHeight;

		coordinates.rfBlue[0] = hX + 0.5f;
		coordinates.rfBlue[1] = hY + 0.5f;
		coordinates.rfGreen[0] = hX + 0.5f;
		coordinates.rfGreen[1] = hY + 0.5f;
		coordinates.rfRed[0] = hX + 0.5f;
		coordinates.rfRed[1] = hY + 0.5f;

		return coordinates;
	}

	virtual DriverPose_t GetPose() 
	{
		DriverPose_t pose = { 0 };

		if (HMDConnected) {
			pose.poseIsValid = true;
			pose.result = TrackingResult_Running_OK;
			pose.deviceIsConnected = true;
		}
		else
		{
			pose.poseIsValid = false;
			pose.result = TrackingResult_Uninitialized;
			pose.deviceIsConnected = false;
		}

		pose.qWorldFromDriverRotation = HmdQuaternion_Init(1, 0, 0, 0);
		pose.qDriverFromHeadRotation = HmdQuaternion_Init(1, 0, 0, 0);

		if (HMDConnected) {

			//TrueOpenVR data
			GetHMDData(&MyHMD);

			//Set head tracking rotation
			pose.qRotation = EulerAngleToQuaternion(DegToRad(MyHMD.Roll), DegToRad(-MyHMD.Yaw), DegToRad(-MyHMD.Pitch));

			//Set head position tracking
			pose.vecPosition[0] = MyHMD.X;
			pose.vecPosition[1] = MyHMD.Z;
			pose.vecPosition[2] = MyHMD.Y;
		}

		return pose;
	}
	

	void RunFrame()
	{
		// In a real driver, this should happen from some pose tracking thread.
		// The RunFrame interval is unspecified and can be very irregular if some other
		// driver blocks it for some periodic task.
		if ( HMDIndex_t != vr::k_unTrackedDeviceIndexInvalid )
		{
			vr::VRServerDriverHost()->TrackedDevicePoseUpdated( HMDIndex_t , GetPose(), sizeof( DriverPose_t ) );
		}
	}

	std::string GetSerialNumber() const { return m_sSerialNumber; }

private:
	vr::TrackedDeviceIndex_t HMDIndex_t;
	vr::PropertyContainerHandle_t m_ulPropertyContainer;

	std::string m_sSerialNumber;
	std::string m_sModelNumber;

	int32_t m_nWindowX;
	int32_t m_nWindowY;
	int32_t m_nWindowWidth;
	int32_t m_nWindowHeight;
	int32_t m_nRenderWidth;
	int32_t m_nRenderHeight;
	float m_flSecondsFromVsyncToPhotons;
	float m_flDisplayFrequency;
	float m_flIPD;
	float m_fZoomWidth;
	float m_fZoomHeight;
	float m_fFOV;
	bool m_bStereoMode;
	bool m_bDebugMode;
};

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------

class CSampleControllerDriver : public vr::ITrackedDeviceServerDriver
{
	int32_t ControllerIndex;
public:
	CSampleControllerDriver()
	{
		Ctrl1Index_t = vr::k_unTrackedDeviceIndexInvalid;
		Ctrl2Index_t = vr::k_unTrackedDeviceIndexInvalid;
		m_ulPropertyContainer = vr::k_ulInvalidPropertyContainer;
	}

	virtual void SetControllerIndex(int32_t CtrlIndex)
	{
		ControllerIndex = CtrlIndex;
	}

	virtual ~CSampleControllerDriver()
	{
	}

	virtual EVRInitError Activate( vr::TrackedDeviceIndex_t unObjectId )
	{

		switch (ControllerIndex)
		{
		case 1:
			Ctrl1Index_t = unObjectId;
			m_ulPropertyContainer = vr::VRProperties()->TrackedDeviceToPropertyContainer(Ctrl1Index_t);
			vr::VRProperties()->SetStringProperty(m_ulPropertyContainer, Prop_SerialNumber_String, "CTRL1Serial");
			vr::VRProperties()->SetInt32Property(m_ulPropertyContainer, Prop_ControllerRoleHint_Int32, TrackedControllerRole_LeftHand);
			break;
		case 2:
			Ctrl2Index_t = unObjectId;
			m_ulPropertyContainer = vr::VRProperties()->TrackedDeviceToPropertyContainer(Ctrl2Index_t);
			vr::VRProperties()->SetStringProperty(m_ulPropertyContainer, Prop_SerialNumber_String, "CTRL2Serial");
			vr::VRProperties()->SetInt32Property(m_ulPropertyContainer, Prop_ControllerRoleHint_Int32, TrackedControllerRole_RightHand);
			break;
		}

		vr::VRProperties()->SetStringProperty(m_ulPropertyContainer, vr::Prop_ControllerType_String, "vive_controller");

		vr::VRProperties()->SetStringProperty(m_ulPropertyContainer, vr::Prop_ModelNumber_String, "ViveMV");
		vr::VRProperties()->SetStringProperty(m_ulPropertyContainer, vr::Prop_ManufacturerName_String, "HTC");
		vr::VRProperties()->SetStringProperty(m_ulPropertyContainer, vr::Prop_RenderModelName_String, "vr_controller_vive_1_5");

		vr::VRProperties()->SetStringProperty(m_ulPropertyContainer, Prop_TrackingSystemName_String, "VR Controller");

		uint64_t supportedButtons = 0xA4;
		vr::VRProperties()->SetUint64Property(m_ulPropertyContainer, vr::Prop_SupportedButtons_Uint64, supportedButtons);

		// avoid "not fullscreen" warnings from vrmonitor
		vr::VRProperties()->SetBoolProperty(m_ulPropertyContainer, Prop_IsOnDesktop_Bool, false);

		vr::VRProperties()->SetStringProperty(m_ulPropertyContainer, Prop_InputProfilePath_String, "{htc}/input/controller_profile.json");

		/*bool bSetupIconUsingExternalResourceFile = false;
		if (!bSetupIconUsingExternalResourceFile)
		{
			// Setup properties directly in code.
			// Path values are of the form {drivername}\icons\some_icon_filename.png
			vr::VRProperties()->SetStringProperty(m_ulPropertyContainer, vr::Prop_NamedIconPathDeviceOff_String, "{hlalyx}/icons/barebones_status_off.png");
			vr::VRProperties()->SetStringProperty(m_ulPropertyContainer, vr::Prop_NamedIconPathDeviceReady_String, "{hlalyx}/icons/barebones_status_ready.png");
			vr::VRProperties()->SetStringProperty(m_ulPropertyContainer, vr::Prop_NamedIconPathDeviceReadyAlert_String, "{hlalyx}/icons/barebones_status_ready_alert.png");
			vr::VRProperties()->SetStringProperty(m_ulPropertyContainer, vr::Prop_NamedIconPathDeviceNotReady_String, "{hlalyx}/icons/barebones_status_error.png");
			vr::VRProperties()->SetStringProperty(m_ulPropertyContainer, vr::Prop_NamedIconPathDeviceStandby_String, "{hlalyx}/icons/barebones_status_standby.png");
			vr::VRProperties()->SetStringProperty(m_ulPropertyContainer, vr::Prop_NamedIconPathDeviceAlertLow_String, "{hlalyx}/icons/barebones_status_ready_low.png");
		}*/

		vr::VRDriverInput()->CreateScalarComponent( m_ulPropertyContainer, "/input/trackpad/x", &HAnalog[0], vr::VRScalarType_Absolute, vr::VRScalarUnits_NormalizedTwoSided);
		vr::VRDriverInput()->CreateScalarComponent(m_ulPropertyContainer, "/input/trackpad/y", &HAnalog[1], vr::VRScalarType_Absolute, vr::VRScalarUnits_NormalizedTwoSided);
		vr::VRDriverInput()->CreateScalarComponent(m_ulPropertyContainer, "/input/trigger/value", &HAnalog[2], vr::EVRScalarType::VRScalarType_Absolute, vr::EVRScalarUnits::VRScalarUnits_NormalizedOneSided);

		//  Buttons handles
		vr::VRDriverInput()->CreateBooleanComponent(m_ulPropertyContainer, "/input/application_menu/click", &HButtons[0]);
		vr::VRDriverInput()->CreateBooleanComponent(m_ulPropertyContainer, "/input/system/click", &HButtons[2]);

		vr::VRDriverInput()->CreateBooleanComponent(m_ulPropertyContainer, "/input/grip/click", &HButtons[1]);
		vr::VRDriverInput()->CreateBooleanComponent(m_ulPropertyContainer, "/input/trigger/click", &HButtons[5]);

		vr::VRDriverInput()->CreateBooleanComponent(m_ulPropertyContainer, "/input/trackpad/click", &HButtons[3]);
		vr::VRDriverInput()->CreateBooleanComponent(m_ulPropertyContainer, "/input/trackpad/touch", &HButtons[4]);

		// create our haptic component
		vr::VRDriverInput()->CreateHapticComponent(m_ulPropertyContainer, "/output/haptic", &m_compHaptic);

		return VRInitError_None;
	}

	virtual void Deactivate()
	{
		switch (ControllerIndex)
		{
		case 1:
			Ctrl1Index_t = vr::k_unTrackedDeviceIndexInvalid;
			break;
		case 2:
			Ctrl2Index_t = vr::k_unTrackedDeviceIndexInvalid;
			break;
		}
	}


	virtual void EnterStandby()
	{
	}

	void *GetComponent( const char *pchComponentNameAndVersion )
	{
		// override this to add a component to a driver
		return NULL;
	}

	virtual void PowerOff()
	{
	}

	/** debug request from a client */
	virtual void DebugRequest( const char *pchRequest, char *pchResponseBuffer, uint32_t unResponseBufferSize )
	{
		if ( unResponseBufferSize >= 1 )
			pchResponseBuffer[0] = 0;
	}

	virtual DriverPose_t GetPose()
	{
		DriverPose_t pose = { 0 };
		//pose.poseIsValid = false;
		pose.poseIsValid = true;
		//pose.result = TrackingResult_Calibrating_OutOfRange;
		pose.result = TrackingResult_Running_OK;
		pose.deviceIsConnected = true;

		pose.qWorldFromDriverRotation = HmdQuaternion_Init( 1, 0, 0, 0 );
		pose.qDriverFromHeadRotation = HmdQuaternion_Init( 1, 0, 0, 0 );

		//Controllers positions and rotations
		if (ControllerIndex == 1) {

			pose.vecPosition[0] = MyCtrl.X;
			pose.vecPosition[1] = MyCtrl.Z;
			pose.vecPosition[2] = MyCtrl.Y;

			//Velocity, right?
			pose.vecVelocity[0] = (pose.vecPosition[0] - FirstCtrlLastPos[0]) * 1000 / max((int)deltaTime.count(), 1) / 3; // div 3 - ghosting fix, there are right ways to remove ghosting?
			pose.vecVelocity[1] = (pose.vecPosition[1] - FirstCtrlLastPos[1]) * 1000 / max((int)deltaTime.count(), 1) / 3;
			pose.vecVelocity[2] = (pose.vecPosition[2] - FirstCtrlLastPos[2]) * 1000 / max((int)deltaTime.count(), 1) / 3;
			FirstCtrlLastPos[0] = pose.vecPosition[0];
			FirstCtrlLastPos[1] = pose.vecPosition[1];
			FirstCtrlLastPos[2] = pose.vecPosition[2];

			//Rotation first controller
			pose.qRotation = EulerAngleToQuaternion(DegToRad(MyCtrl.Roll), DegToRad(-MyCtrl.Yaw), DegToRad(-MyCtrl.Pitch));

		} else { 
			//Controller2
			pose.vecPosition[0] = MyCtrl2.X;
			pose.vecPosition[1] = MyCtrl2.Z;
			pose.vecPosition[2] = MyCtrl2.Y;

			//Velocity, right?
			pose.vecVelocity[0] = (pose.vecPosition[0] - SecondCtrlLastPos[0]) * 1000 / max((int)deltaTime.count(), 1) / 3; // div 3 - ghosting fix, there are right ways to remove ghosting?
			pose.vecVelocity[1] = (pose.vecPosition[1] - SecondCtrlLastPos[1]) * 1000 / max((int)deltaTime.count(), 1) / 3;
			pose.vecVelocity[2] = (pose.vecPosition[2] - SecondCtrlLastPos[2]) * 1000 / max((int)deltaTime.count(), 1) / 3;
			SecondCtrlLastPos[0] = pose.vecPosition[0];
			SecondCtrlLastPos[1] = pose.vecPosition[1];
			SecondCtrlLastPos[2] = pose.vecPosition[2];

			//Rotation first controller
			pose.qRotation = EulerAngleToQuaternion(DegToRad(MyCtrl2.Roll), DegToRad(-MyCtrl2.Yaw), DegToRad(-MyCtrl2.Pitch));

		}

		return pose;
	}

	void RunFrame()
	{
		switch (ControllerIndex)
		{
		case 1:
			if (Ctrl1Index_t != vr::k_unTrackedDeviceIndexInvalid)
			{
				vr::VRServerDriverHost()->TrackedDevicePoseUpdated(Ctrl1Index_t, GetPose(), sizeof(DriverPose_t));
			}
			break;
		case 2:
			if (Ctrl2Index_t != vr::k_unTrackedDeviceIndexInvalid)
			{
				vr::VRServerDriverHost()->TrackedDevicePoseUpdated(Ctrl2Index_t, GetPose(), sizeof(DriverPose_t));
			}
			break;
		}

		// Your driver would read whatever hardware state is associated with its input components and pass that
		// in to UpdateBooleanComponent. This could happen in RunFrame or on a thread of your own that's reading USB
		// state. There's no need to update input state unless it changes, but it doesn't do any harm to do so.
		
		if (ControllerIndex == 1) {

			vr::VRDriverInput()->UpdateBooleanComponent(HButtons[0], (MyCtrl.Buttons & MENU_BTN) != 0, 0); //Application Menu
			vr::VRDriverInput()->UpdateBooleanComponent(HButtons[1], (MyCtrl.Buttons & GRIP_BTN) != 0, 0); //Grip
			vr::VRDriverInput()->UpdateBooleanComponent(HButtons[2], (MyCtrl.Buttons & SYS_BTN) != 0, 0); //System
			vr::VRDriverInput()->UpdateBooleanComponent(HButtons[3], (MyCtrl.Buttons & THUMB_BTN) != 0, 0); //Trackpad

			vr::VRDriverInput()->UpdateScalarComponent(HAnalog[0], MyCtrl.AxisX, 0); //Trackpad x
			vr::VRDriverInput()->UpdateScalarComponent(HAnalog[1], MyCtrl.AxisY, 0); //Trackpad y

			if (MyCtrl.AxisX != 0 || MyCtrl.AxisY != 0) {
				vr::VRDriverInput()->UpdateBooleanComponent(HButtons[4], 1, 0); //Trackpad touch
			}
			else {
				vr::VRDriverInput()->UpdateBooleanComponent(HButtons[4], 0, 0); //Trackpad touch
			}

			vr::VRDriverInput()->UpdateScalarComponent(HAnalog[2], MyCtrl.Trigger, 0); //Trigger
		} else {
			//Controller2
			vr::VRDriverInput()->UpdateBooleanComponent(HButtons[0], (MyCtrl2.Buttons & MENU_BTN) != 0, 0); //Application Menu
			vr::VRDriverInput()->UpdateBooleanComponent(HButtons[1], (MyCtrl2.Buttons & GRIP_BTN) != 0, 0); //Grip
			vr::VRDriverInput()->UpdateBooleanComponent(HButtons[2], (MyCtrl2.Buttons & SYS_BTN) != 0, 0); //System
			vr::VRDriverInput()->UpdateBooleanComponent(HButtons[3], (MyCtrl2.Buttons & THUMB_BTN) != 0, 0); //Trackpad

			vr::VRDriverInput()->UpdateScalarComponent(HAnalog[0], MyCtrl2.AxisX, 0); //Trackpad x
			vr::VRDriverInput()->UpdateScalarComponent(HAnalog[1], MyCtrl2.AxisY, 0); //Trackpad y

			if (MyCtrl2.AxisX != 0 || MyCtrl2.AxisY != 0) {
				vr::VRDriverInput()->UpdateBooleanComponent(HButtons[4], 1, 0); //Trackpad touch
			}
			else {
				vr::VRDriverInput()->UpdateBooleanComponent(HButtons[4], 0, 0); //Trackpad touch
			}

			vr::VRDriverInput()->UpdateScalarComponent(HAnalog[2], MyCtrl2.Trigger, 0); //Trigger
		}

	}

	void ProcessEvent( const vr::VREvent_t & vrEvent )
	{
		switch ( vrEvent.eventType )
		{
		case vr::VREvent_Input_HapticVibration:
		{
			if ( vrEvent.data.hapticVibration.componentHandle == m_compHaptic )
			{
				//SetControllerData(ControllerIndex, 100); 
				// This is where you would send a signal to your hardware to trigger actual haptic feedback
			}
		}
		break;
		}
	}

	std::string GetSerialNumber() const { 
		
		switch (ControllerIndex)
		{
		case 1:
			return "CTRL1Serial";
			break;
		case 2:
			return "CTRL2Serial";
			break;
		}
	}

private:
	vr::TrackedDeviceIndex_t Ctrl1Index_t;
	vr::TrackedDeviceIndex_t Ctrl2Index_t;
	vr::PropertyContainerHandle_t m_ulPropertyContainer;

	//vr::VRInputComponentHandle_t m_compA;
	//vr::VRInputComponentHandle_t m_compB;
	//vr::VRInputComponentHandle_t m_compC;
	vr::VRInputComponentHandle_t m_compHaptic;

	vr::VRInputComponentHandle_t HButtons[6], HAnalog[3];
	//std::string m_sSerialNumber;
	//std::string m_sModelNumber;
};

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
class CServerDriver_Sample: public IServerTrackedDeviceProvider
{
public:
	virtual EVRInitError Init( vr::IVRDriverContext *pDriverContext ) ;
	virtual void Cleanup() ;
	virtual const char * const *GetInterfaceVersions() { return vr::k_InterfaceVersions; }
	virtual void RunFrame() ;
	virtual bool ShouldBlockStandbyMode()  { return false; }
	virtual void EnterStandby()  {}
	virtual void LeaveStandby()  {}

private:
	CSampleDeviceDriver *m_pNullHmdLatest = nullptr;
	CSampleControllerDriver *m_pController = nullptr;
	CSampleControllerDriver *m_pController2 = nullptr;
};

CServerDriver_Sample g_serverDriverNull;


EVRInitError CServerDriver_Sample::Init( vr::IVRDriverContext *pDriverContext )
{
	VR_INIT_SERVER_DRIVER_CONTEXT( pDriverContext );

	CRegKey key;
	TCHAR steamPath[MAX_PATH];
	LONG status = key.Open(HKEY_CURRENT_USER, _T("Software\\Valve\\Steam"));
	if (status == ERROR_SUCCESS)
	{
		ULONG regSize = sizeof(steamPath);
		status = key.QueryStringValue(_T("SteamPath"), steamPath, &regSize);
	}
	key.Close();

	TCHAR configPath[MAX_PATH] = { 0 };
	_tcscat_s(configPath, sizeof(configPath), steamPath);
	_tcscat_s(configPath, sizeof(configPath), _T("/steamapps/common/SteamVR/drivers/hlalyx/HalfLifeAlyx.ini"));


	CIniReader IniFile((char *)configPath);
	mouseSensetiveYaw    = IniFile.ReadFloat("Main", "AxisX", 0.050);
	mouseSensetivePitch  = IniFile.ReadFloat("Main", "AxisY", 0.040);
	ZPosSensetive        = IniFile.ReadFloat("Main", "AxisZ", 0.009);

	IX_MOVEMENT_SPEED    = IniFile.ReadFloat("Main", "WALK_SPEED",  0.8);
	IX_HAND_SPEED        = IniFile.ReadFloat("Main", "HAND_SPEED",  0.8);
    IX_CROUCH_DIST       = IniFile.ReadFloat("Main", "CROUCH_DIST", 0.8);

	IX_crouch = KeyNameToKeyCode(IniFile.ReadString("Keys", "CROUCH", "SHIFT"));

	//HMD
	IX_HMD_xp = KeyNameToKeyCode(IniFile.ReadString("Keys", "MOVE_RIGHT",     "SHIFT")); //right
	IX_HMD_yp = KeyNameToKeyCode(IniFile.ReadString("Keys", "MOVE_BACKWARDS", "SHIFT")); //foward
	IX_HMD_zp = KeyNameToKeyCode(IniFile.ReadString("Keys", "MOVE_UP",        "SHIFT")); //up

	IX_HMD_xn = KeyNameToKeyCode(IniFile.ReadString("Keys", "MOVE_LEFT",      "SHIFT")); //left
	IX_HMD_yn = KeyNameToKeyCode(IniFile.ReadString("Keys", "MOVE_FOWARD",    "SHIFT"));//backwards
	IX_HMD_zn = KeyNameToKeyCode(IniFile.ReadString("Keys", "MOVE_DOWN",      "SHIFT"));//down


	//LEFT CONTROL
	IX_CTL_left_xp        = KeyNameToKeyCode(IniFile.ReadString("Keys", "LCTL_RIGHT",       "SHIFT")); //right
	IX_CTL_left_yp        = KeyNameToKeyCode(IniFile.ReadString("Keys", "LCTL_FOWARD",      "SHIFT")); //foward
	IX_CTL_left_zp        = KeyNameToKeyCode(IniFile.ReadString("Keys", "LCTL_UP",          "SHIFT")); //up 

	IX_CTL_left_xn        = KeyNameToKeyCode(IniFile.ReadString("Keys", "LCTL_LEFT",        "SHIFT")); //left
	IX_CTL_left_yn        = KeyNameToKeyCode(IniFile.ReadString("Keys", "LCTL_BACKWARDS",   "SHIFT")); //backwards
	IX_CTL_left_zn        = KeyNameToKeyCode(IniFile.ReadString("Keys", "LCTL_DOWN",        "SHIFT")); //down

	IX_CTL_left_f         = KeyNameToKeyCode(IniFile.ReadString("Keys", "LRCTL_FOWARD",     "SHIFT"));
	IX_CTL_left_b         = KeyNameToKeyCode(IniFile.ReadString("Keys", "LRCTL_BACKWARDS",  "SHIFT"));
	IX_CTL_left_l         = KeyNameToKeyCode(IniFile.ReadString("Keys", "LRCTL_LEFT",       "SHIFT"));
	IX_CTL_left_r         = KeyNameToKeyCode(IniFile.ReadString("Keys", "LRCTL_RIGHT",      "SHIFT"));

	IX_CTL_left_btn_menu  = KeyNameToKeyCode(IniFile.ReadString("Keys", "LCTL_MENU",        "SHIFT"));
	IX_CTL_left_btn_thumb = KeyNameToKeyCode(IniFile.ReadString("Keys", "LCTL_THUMB",       "SHIFT"));
	IX_CTL_left_trigger   = KeyNameToKeyCode(IniFile.ReadString("Keys", "LCTL_TRIGGER",     "SHIFT"));

	IX_CTL_left_reset_r   = KeyNameToKeyCode(IniFile.ReadString("Keys", "LRCTL_RESET",      "SHIFT"));
	IX_CTL_left_reset_xyz = KeyNameToKeyCode(IniFile.ReadString("Keys",  "LCTL_RESET",      "SHIFT"));



	//RIGHT CONTROL
	IX_CTL_right_xp        = KeyNameToKeyCode(IniFile.ReadString("Keys", "RCTL_RIGHT",      "SHIFT")); //right
	IX_CTL_right_yp        = KeyNameToKeyCode(IniFile.ReadString("Keys", "RCTL_FOWARD",     "SHIFT")); //foward
	IX_CTL_right_zp        = KeyNameToKeyCode(IniFile.ReadString("Keys", "RCTL_UP",         "SHIFT")); //up 

	IX_CTL_right_xn        = KeyNameToKeyCode(IniFile.ReadString("Keys", "RCTL_LEFT",       "SHIFT")); //left
	IX_CTL_right_yn        = KeyNameToKeyCode(IniFile.ReadString("Keys", "RCTL_BACKWARDS",  "SHIFT")); //backwards
	IX_CTL_right_zn        = KeyNameToKeyCode(IniFile.ReadString("Keys", "RCTL_DOWN",       "SHIFT")); //down

	IX_CTL_right_f         = KeyNameToKeyCode(IniFile.ReadString("Keys", "RRCTL_FOWARD",    "SHIFT"));
	IX_CTL_right_b         = KeyNameToKeyCode(IniFile.ReadString("Keys", "RRCTL_BACKWARDS", "SHIFT"));
	IX_CTL_right_l         = KeyNameToKeyCode(IniFile.ReadString("Keys", "RRCTL_LEFT",      "SHIFT"));
	IX_CTL_right_r         = KeyNameToKeyCode(IniFile.ReadString("Keys", "RRCTL_RIGHT",     "SHIFT"));

	IX_CTL_right_btn_menu  = KeyNameToKeyCode(IniFile.ReadString("Keys", "RCTL_MENU",       "SHIFT"));
	IX_CTL_right_btn_thumb = KeyNameToKeyCode(IniFile.ReadString("Keys", "RCTL_THUMB",      "SHIFT"));
	IX_CTL_right_trigger    = KeyNameToKeyCode(IniFile.ReadString("Keys", "RCTL_TRIGGER",    "SHIFT"));

	IX_CTL_right_reset_r   = KeyNameToKeyCode(IniFile.ReadString("Keys", "RRCTL_RESET",     "SHIFT"));
	IX_CTL_right_reset_xyz = KeyNameToKeyCode(IniFile.ReadString("Keys",  "RCTL_RESET",     "SHIFT"));



	m_HalfWidth  = GetSystemMetrics(SM_CXSCREEN) / 2;
	m_HalfHeight = GetSystemMetrics(SM_CYSCREEN) / 2;


	VRMode         = IniFile.ReadBoolean("Main", "VRMode", false);
	HMDConnected   = IniFile.ReadBoolean("Main", "HMD", true);
	ctrlsConnected = true;

	if (HMDConnected) 
	{
		m_pNullHmdLatest = new CSampleDeviceDriver();
		vr::VRServerDriverHost()->TrackedDeviceAdded(m_pNullHmdLatest->GetSerialNumber().c_str(), vr::TrackedDeviceClass_HMD, m_pNullHmdLatest);
	}

	if (ctrlsConnected) {
		m_pController = new CSampleControllerDriver();
		m_pController->SetControllerIndex(1);
		vr::VRServerDriverHost()->TrackedDeviceAdded(m_pController->GetSerialNumber().c_str(), vr::TrackedDeviceClass_Controller, m_pController);


		m_pController2 = new CSampleControllerDriver();
		m_pController2->SetControllerIndex(2);
		vr::VRServerDriverHost()->TrackedDeviceAdded(m_pController2->GetSerialNumber().c_str(), vr::TrackedDeviceClass_Controller, m_pController2);
	}

	return VRInitError_None;
}

void CServerDriver_Sample::Cleanup() 
{

}


void CServerDriver_Sample::RunFrame()
{
	//Velocity
	static milliseconds lastMillis = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
	deltaTime = duration_cast<milliseconds>(system_clock::now().time_since_epoch()) - lastMillis;
	lastMillis = duration_cast<milliseconds>(system_clock::now().time_since_epoch());

	//HL Alyx
	SteamVRHeadsetWindow = FindWindow(NULL, "Headset Window");
	HalfLifeAlyxWindow = FindWindow(NULL, "Half-Life: Alyx");

	if ((GetAsyncKeyState(VK_F1) & 0x8000) != 0 || (GetAsyncKeyState(VK_F7) & 0x8000) != 0)
		SetForegroundWindow(SteamVRHeadsetWindow);

	//if ((GetAsyncKeyState(VK_F7) & 0x8000) != 0)
	HeadsetWindowFocused = SteamVRHeadsetWindow != 0 && SteamVRHeadsetWindow == GetForegroundWindow() && IsWindowVisible(SteamVRHeadsetWindow);
	HalfLifeAlyxFocused = HalfLifeAlyxWindow != 0 && HalfLifeAlyxWindow == GetForegroundWindow() && IsWindowVisible(HalfLifeAlyxWindow);

	if (HeadsetWindowFocused || HalfLifeAlyxFocused)
		MouseToYawPitch();
	////////

	if ( m_pNullHmdLatest )
	{
		m_pNullHmdLatest->RunFrame();
	}

	if (ctrlsConnected) {
		
		GetControllersData(&MyCtrl, &MyCtrl2);

		if (m_pController)
		{
			m_pController->RunFrame();
		}
		if (m_pController2)
		{
			m_pController2->RunFrame();
		}

		vr::VREvent_t vrEvent;
		while ( vr::VRServerDriverHost()->PollNextEvent( &vrEvent, sizeof( vrEvent ) ) )
		{
			if ( m_pController )
			{
				m_pController->ProcessEvent(vrEvent);
			}
			if (m_pController2)
			{
				m_pController2->ProcessEvent(vrEvent);
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
HMD_DLL_EXPORT void *HmdDriverFactory( const char *pInterfaceName, int *pReturnCode )
{
	if( 0 == strcmp( IServerTrackedDeviceProvider_Version, pInterfaceName ) )
	{
		return &g_serverDriverNull;
	}

	if( pReturnCode )
		*pReturnCode = VRInitError_Init_InterfaceNotFound;

	return NULL;
}

