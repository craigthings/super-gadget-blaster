#pragma once

#include "CoreTypes.h"
#include "Containers/Array.h"
#include "Containers/UnrealString.h"
#include "Containers/StringFwd.h"
#include "StrikerVRState.h"

class FMemoryWriter;

class FOPKBase {
protected:
	typedef TStringBuilder<256> TCSVBuilder;

public:	
	virtual void Serialize(FMemoryWriter& writer) = 0;
	virtual ~FOPKBase() {}

	uint16_t GetDeviceIndex() const { return DeviceIndex; }
	bool HasValidDeviceIndex() const { return GetDeviceIndex() != kInvalidDeviceIndex; }

	static const uint16_t kInvalidDeviceIndex = (uint16_t)-1;

protected:
	virtual void WriteBinary(FMemoryWriter& writer);
	virtual void BuildCSV(TCSVBuilder& writer);
	
	void WriteCSV(FMemoryWriter& writer);
protected:
	FOPKBase(const int32 PacketType) : PacketType(PacketType), PacketVersion(1), DeviceIndex(kInvalidDeviceIndex) {}
	FOPKBase(const int32 PacketType, const uint16_t deviceIdx) : PacketType(PacketType), PacketVersion(1), DeviceIndex(deviceIdx) {}

private:
	const int32 PacketType;
	uint16 PacketVersion;
	uint16 DeviceIndex;
};

class FOPKCmdPlayHaptic : public FOPKBase
{
public:
	static const int32 PacketType = 120;

	virtual void Serialize(FMemoryWriter& writer) { WriteCSV(writer); }

	static FOPKCmdPlayHaptic* Create(uint16_t deviceIdx, FString const& libraryIdentifier, FString const& effectId);
protected:
	virtual void BuildCSV(TCSVBuilder& writer);
protected:
	FOPKCmdPlayHaptic(uint16_t deviceIdx, FString const& libraryIdentifier, FString const& effectId)
		: FOPKBase(PacketType, deviceIdx)
		, LibraryIdentifier(libraryIdentifier)
		, EffectId(effectId)
	{}

	FString const LibraryIdentifier;
	FString const EffectId;
};

class FOPKHapticLibrary : public FOPKBase
{
public:
	static const int32 PacketType = 200;

	virtual void Serialize(FMemoryWriter& writer) { WriteBinary(writer); }

	static FOPKHapticLibrary* Create(FString LibraryIdentifier, FString JSON)
	{
		return new FOPKHapticLibrary(LibraryIdentifier, JSON);
	}

protected:
	virtual void WriteBinary(FMemoryWriter& writer) override;

protected:
	FOPKHapticLibrary(FString LibraryIdentifier, FString JSON) 
		: FOPKBase(PacketType) 
		, LibraryIdentifier(LibraryIdentifier)
		, JSON(JSON)
	{}	

	FString LibraryIdentifier;
	FString JSON;
};

class FPKBase {
public:
	FPKBase(const int32 PacketType) : PacketType(PacketType) {}
	virtual ~FPKBase() {}

	virtual void ApplyToState(FStrikerVRData& state) {};
	virtual void Log() = 0;
	virtual bool ReadFromCSV(const TArray<const TCHAR*>& row, uint32_t& offset);
	virtual bool ReadFromArray(TArray<uint8> const& RawFrame, size_t& Offset);
protected:
	const int32 PacketType;
	int32 PacketVersion;
	int32 DeviceIndex;
};

class FPKDeviceState : public FPKBase {
public:
	static const int32 PacketType = 10;

	FPKDeviceState() : FPKBase (PacketType) {}

	virtual void ApplyToState(FStrikerVRData& state);
	virtual void Log() override;

	virtual bool ReadFromCSV(const TArray<const TCHAR*>& row, uint32_t& offset) override;
protected:
	FString Guid;
	bool Connected;
	
	int32 DeviceType; // TODO: Make Enum
	uint8 CoverPins; // TODO: From HEX
	
	bool IsReady;
	int32 RSSI;
	
	float BatteryLevel;
	int32 BatteryChargeState;

	float PosOffsetX;
	float PosOffsetY;
	float PosOffsetZ;

	float RotOffsetX;
	float RotOffsetY;
	float RotOffsetZ;
};

class FPKDeviceButtons: public FPKBase {
public:
	static const int32 PacketType = 22; // TODO: Should come from an enum

	FPKDeviceButtons() : FPKBase(PacketType) {} //TODO: This should be more of an assert

	virtual void ApplyToState(FStrikerVRData& state);
	virtual void Log() override;

	virtual bool ReadFromArray(TArray<uint8> const& RawFrame, size_t& Offset) override;
protected:
	bool SideLeft;
	bool SideRight;
	bool MenuTop;
	bool MenuBottom;
	bool TouchpadLeft;
	bool TouchpadRight;
	bool TriggerActive;
	bool TriggerMode;
	uint8 TriggerAxis;
};


class FPKRawSensors : public FPKBase {
public:
	static const int32 PacketType = 20; // TODO: Should come from an enum

	FPKRawSensors() : FPKBase(PacketType) {} //TODO: This should be more of an assert

	virtual void ApplyToState(FStrikerVRData& state);
	virtual void Log() override;

	virtual bool ReadFromArray(TArray<uint8> const& RawFrame, size_t& Offset) override;
protected:
	// Should be 2 bytes long, made up of 12 bits (0-11)
	uint16 Slide;

	// 3 bits (0-3)
	uint8 Reload;

	bool TriggerGripLeft;
	bool TriggerGripRight;
	bool TriggerTouch;
		
	// 2 bytes long, 9 bits (0-8)
	uint16 ForwardBarGrip;

	// 6 bits (0-5)
	uint8 UnderTouchpadGrip;

	// 5 bits (0-4)
	uint8 FrontHandGrip;
};

class FPKDeviceAcc: public FPKBase {
public:
	static const int32 PacketType = 21; // TODO: Should come from an enum

	FPKDeviceAcc() : FPKBase(PacketType) {} //TODO: This should be more of an assert

	virtual void ApplyToState(FStrikerVRData& state);
	virtual void Log() override;

	virtual bool ReadFromArray(TArray<uint8> const& RawFrame, size_t& Offset) override;
protected:
	float AccX;
	float AccY;
	float AccZ;
};

class FPKTouchpad : public FPKBase {
public:
	static const int32 PacketType = 23; // TODO: Should come from an enum

	FPKTouchpad() : FPKBase(PacketType) {} //TODO: This should be more of an assert

	virtual void ApplyToState(FStrikerVRData& state);
	virtual void Log() override;

	virtual bool ReadFromArray(TArray<uint8> const& RawFrame, size_t& Offset) override;
protected:
	float TouchpadLeftX;
	float TouchpadLeftY;
	float TouchpadLeftZ;

	float TouchpadRightX;
	float TouchpadRightY;
	float TouchpadRightZ;
};

class FPKCalculatedSensors : public FPKBase {
public:
	static const int32 PacketType = 24; // TODO: Should come from an enum

	FPKCalculatedSensors() : FPKBase(PacketType) {} //TODO: This should be more of an assert

	virtual void ApplyToState(FStrikerVRData& state);
	virtual void Log() override;

	virtual bool ReadFromArray(TArray<uint8> const& RawFrame, size_t& Offset) override;
protected:
	bool SlideTouched;
	float SlideTouchAmount;
	float SlidePosition;

	bool ReloadTouched;
	float ReloadTouchAmount;

	bool ForwardBarGripTouched;
	float ForwardBarGripTouchAmount;
	float ForwardBarGripPosition;

	bool UnderTouchpadGripTouched;
	float UnderTouchpadGripTouchAmount;
	float UnderTouchpadGripPosition;

	bool FrontHandGripTouched;
	float FrontHandGripTouchAmount;
	bool FrontHandGripFaceTouched;
	float FrontHandGripFaceTouchAmount;

	bool TriggerGripTouched;
	float TriggerGripTouchAmount;
};

enum GestureEventType {
	Start,
	Progress,
	Complete,
	Failed
};

class FPKGestureEvent: public FPKBase {
public:
	static const int32 PacketType = 25; // TODO: Should come from an enum

	FPKGestureEvent() : FPKBase(PacketType) {} //TODO: This should be more of an assert

	virtual void ApplyToState(FStrikerVRData& state);
	virtual void Log() override;

	virtual bool ReadFromCSV(const TArray<const TCHAR*>& Row, uint32_t& Col) override;
protected:
	FString GestureId;
	GestureEventType EventType;
	float GestureSpeed;
	float GestureProgress;
};

class FPKFactory {
public:
	static FPKBase* PacketFromType(int32 PacketType);
};