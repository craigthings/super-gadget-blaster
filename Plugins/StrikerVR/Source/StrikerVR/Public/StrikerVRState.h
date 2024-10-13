#pragma once

#include "CoreMinimal.h"
#include "InputCoreTypes.h"
#include "Containers/StaticArray.h"

#include "StrikerVRState.generated.h"

USTRUCT(BlueprintType)
struct FStrikerVRCaculatedSensors
{
	GENERATED_BODY()

	FStrikerVRCaculatedSensors();


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool SlideTouched;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float SlideTouchAmount;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float SlidePosition;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool ReloadTouched;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float ReloadTouchAmount;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool ForwardBarGripTouched;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float ForwardBarGripTouchAmount;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float ForwardBarGripPosition;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool UnderTouchpadGripTouched;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float UnderTouchpadGripTouchAmount;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float UnderTouchpadGripPosition;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool FrontHandGripTouched;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float FrontHandGripTouchAmount;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool FrontHandGripFaceTouched;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float FrontHandGripFaceTouchAmount;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool TriggerGripTouched;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float TriggerGripTouchAmount;
};

USTRUCT(BlueprintType)
struct FStrikerVRTouchPad
{
	GENERATED_BODY()

	FStrikerVRTouchPad();


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float TouchpadLeftX;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float TouchpadLeftY;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float TouchpadLeftZ;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float TouchpadRightX;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float TouchpadRightY;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float TouchpadRightZ;
};

USTRUCT(BlueprintType)
struct FStrikerVRAccell
{
	GENERATED_BODY()

	FStrikerVRAccell();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float AccX;	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float AccY;	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float AccZ;
};


USTRUCT(BlueprintType)
struct FStrikerVRRawData
{
	GENERATED_BODY()

	FStrikerVRRawData();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 Slide; //TODO: Should be uint16?
		
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	uint8 Reload;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool TriggerGripLeft;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool TriggerGripRight;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool TriggerTouch;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 ForwardBarGrip; // FIXME: Should be U16

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	uint8 UnderTouchpadGrip;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	uint8 FrontHandGrip;
};

USTRUCT(BlueprintType)
struct FStrikerVRDeviceButtons
{
	GENERATED_BODY()

	FStrikerVRDeviceButtons();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool SideLeft;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool SideRight;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool MenuTop;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool MenuBottom;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool TouchpadLeft;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool TouchpadRight;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool TriggerActive;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool TriggerMode;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	uint8 TriggerAxis;
};

USTRUCT(BlueprintType)
struct FStrikerVRDeviceState
{
	GENERATED_BODY()

	FStrikerVRDeviceState();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString Guid;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool Connected;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 DeviceType; // TODO: Make Enum
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	uint8 CoverPins; // TODO: From HEX

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool IsReady;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 RSSI;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float BatteryLevel;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 BatteryChargeState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float PosOffsetX;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float PosOffsetY;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float PosOffsetZ;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float RotOffsetX;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float RotOffsetY;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float RotOffsetZ;
};

USTRUCT(BlueprintType)
struct FStrikerGestureData {
	GENERATED_BODY()

	FStrikerGestureData();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString GestureId;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float GestureSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float GestureProgress;
};


USTRUCT(BlueprintType)
struct FStrikerVRDeviceData
{
	GENERATED_BODY()

	FStrikerVRDeviceData() {}

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FStrikerVRDeviceState DeviceState;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FStrikerVRDeviceButtons DeviceButtons;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FStrikerVRRawData RawData;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FStrikerVRAccell Accell;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FStrikerVRTouchPad TouchPad;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FStrikerVRCaculatedSensors CalculatedSensors;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TMap<FString, FStrikerGestureData> Gestures;
};

USTRUCT(BlueprintType)
struct FStrikerVRData {
	GENERATED_BODY()

	FStrikerVRData();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<FStrikerVRDeviceData>  Devices; 	// FIXME: StaticArray - for bounds checking
};



struct FStrikerVRKey
{
	static const FKey StrikerVR_Trigger;

	static const FKey StrikerVR_FrontTop;
	static const FKey StrikerVR_FrontBottom;

	static const FKey StrikerVR_TriggerRight;
	static const FKey StrikerVR_TriggerLeft;

	static const FKey StrikerVR_CircleRight;
	static const FKey StrikerVR_CircleLeft;	
};

struct FStrikerVRKeyNames
{
	typedef FName Type;

	static const FName StrikerVR_Trigger;

	static const FName StrikerVR_FrontTop;
	static const FName StrikerVR_FrontBottom;

	static const FName StrikerVR_TriggerRight;
	static const FName StrikerVR_TriggerLeft;

	static const FName StrikerVR_CircleRight;
	static const FName StrikerVR_CircleLeft;
};