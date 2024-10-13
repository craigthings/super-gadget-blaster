#include "StrikerVRState.h"

const FKey FStrikerVRKey::StrikerVR_Trigger("StrikerVR_Trigger");
const FKey FStrikerVRKey::StrikerVR_FrontTop("StrikerVR_FrontTop");
const FKey FStrikerVRKey::StrikerVR_FrontBottom("StrikerVR_FrontBottom");
const FKey FStrikerVRKey::StrikerVR_TriggerRight("StrikerVR_TriggerRight");
const FKey FStrikerVRKey::StrikerVR_TriggerLeft("StrikerVR_TriggerLeft");
const FKey FStrikerVRKey::StrikerVR_CircleRight("StrikerVR_CircleRight");
const FKey FStrikerVRKey::StrikerVR_CircleLeft("StrikerVR_CircleLeft");

const FStrikerVRKeyNames::Type FStrikerVRKeyNames::StrikerVR_Trigger("StrikerVR_Trigger");
const FStrikerVRKeyNames::Type FStrikerVRKeyNames::StrikerVR_FrontTop("StrikerVR_FrontTop");
const FStrikerVRKeyNames::Type FStrikerVRKeyNames::StrikerVR_FrontBottom("StrikerVR_FrontBottom");
const FStrikerVRKeyNames::Type FStrikerVRKeyNames::StrikerVR_TriggerRight("StrikerVR_TriggerRight");
const FStrikerVRKeyNames::Type FStrikerVRKeyNames::StrikerVR_TriggerLeft("StrikerVR_TriggerLeft");
const FStrikerVRKeyNames::Type FStrikerVRKeyNames::StrikerVR_CircleRight("StrikerVR_CircleRight");
const FStrikerVRKeyNames::Type FStrikerVRKeyNames::StrikerVR_CircleLeft("StrikerVR_CircleLeft");

FStrikerVRDeviceButtons::FStrikerVRDeviceButtons()
{
	SideLeft = false;
	SideRight = false;
	MenuTop = false;
	MenuBottom = false;
	TouchpadLeft = false;
	TouchpadRight = false;
	TriggerActive = false;
	TriggerMode = false;
	TriggerAxis = 0;
}

FStrikerVRDeviceState::FStrikerVRDeviceState()
{
	Guid = "";
	Connected = false;

	DeviceType = 0;
	CoverPins = 0;

	IsReady = false;
	RSSI = 0;

	BatteryLevel = 0;
	BatteryChargeState = 0;

	PosOffsetX = 0;
	PosOffsetY = 0;
	PosOffsetZ = 0;

	RotOffsetX = 0;
	RotOffsetY = 0;
	RotOffsetZ = 0;
}


FStrikerVRData::FStrikerVRData()
{
	// TODO: Better way to do this?
	Devices.Reserve(16);
	for (int idx = 0; idx < 16; idx++) 
		Devices.AddDefaulted();
}

FStrikerVRRawData::FStrikerVRRawData()
{	
	Slide = 0;

	Reload = 0;

	TriggerGripLeft = false;
	TriggerGripRight = false;
	TriggerTouch = false;

	ForwardBarGrip = 0;
	UnderTouchpadGrip = 0;
	FrontHandGrip = 0;
}

FStrikerVRAccell::FStrikerVRAccell()
{
	AccX = 0;
	AccY = 0;
	AccZ = 0;
}

FStrikerVRTouchPad::FStrikerVRTouchPad()
{
	TouchpadLeftX = 0;
	TouchpadLeftY = 0;
	TouchpadLeftZ = 0;

	TouchpadRightX = 0;
	TouchpadRightY = 0;
	TouchpadRightZ = 0;
}

FStrikerVRCaculatedSensors::FStrikerVRCaculatedSensors()
{
	SlideTouched = false;
	SlideTouchAmount = 0;
	SlidePosition = 0;

	ReloadTouched = false;
	ReloadTouchAmount = 0;

	ForwardBarGripTouched = false;
	ForwardBarGripTouchAmount = 0;
	ForwardBarGripPosition = 0;

	UnderTouchpadGripTouched = false;
	UnderTouchpadGripTouchAmount = 0;
	UnderTouchpadGripPosition = 0;

	FrontHandGripTouched = false;
	FrontHandGripTouchAmount = 0;
	FrontHandGripFaceTouched = false;
	FrontHandGripFaceTouchAmount = 0;

	TriggerGripTouched = false;
	TriggerGripTouchAmount = 0;
}


FStrikerGestureData::FStrikerGestureData()
{
	GestureId = "";
	GestureSpeed = 0.0f;
	GestureProgress = 0.0f;
}