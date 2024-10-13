#include "FPKFactory.h"

#include "StrikerVR.h"
#include "StrikerVRState.h"

#include "Containers/StringFwd.h"
#include "Serialization/MemoryWriter.h"

#define READ_COL_STRING(row, offset) Row[Col++]
#define READ_COL_INT(row, offset)  FCString::Atoi(Row[Col++])
#define READ_COL_BOOL(row, offset)  FCString::Atoi(Row[Col++]) == 1
#define READ_COL_FLOAT(row, offset)  FCString::Atof(Row[Col++])

#define READ_FLOAT(array, offset)  (offset += sizeof(float), *(float*)&array[offset - sizeof(float)])
#define READ_INT(array, offset)  (offset += sizeof(int32), *(int32*)&array[offset - sizeof(int32)])
#define READ_SHORT(array, offset) (offset += sizeof(int16), *(int16*)&array[offset - sizeof(int16)])
#define READ_UBYTE(array, offset) array[offset++]
#define READ_BOOL(array, offset) array[offset++] != 0

#define BIT(u8, bit) ((u8 >> bit) & 1)

void FOPKBase::WriteCSV(FMemoryWriter& writer)
{
	// 256 bytes on the stack
	TCSVBuilder builder;
	BuildCSV(builder);
	builder.AppendChar('\0');

	// TODO: Memory allocs?
	FTCHARToUTF8 UTF8CSV(builder.GetData());
	writer.Serialize((ANSICHAR*)UTF8CSV.Get(), UTF8CSV.Length());
}

void FOPKBase::WriteBinary(FMemoryWriter &writer)
{
	writer.Serialize((void*)&PacketType, sizeof(PacketType));
	writer.Serialize((void*)&PacketVersion, sizeof(PacketVersion));
	writer.Serialize((void*)&DeviceIndex, sizeof(DeviceIndex));
}

void FOPKBase::BuildCSV(TCSVBuilder& writer)
{
	writer.Append(FString::FromInt(PacketType));

	writer.AppendChar(',');
	writer.Append(FString::FromInt(PacketVersion));

	writer.AppendChar(',');
	writer.Append(FString::FromInt(DeviceIndex));
}

void FOPKHapticLibrary::WriteBinary(FMemoryWriter &writer)
{
	FOPKBase::WriteBinary(writer);

	// TODO: Memory allocs?
	FTCHARToUTF8 UTF8LibraryIdentifier(LibraryIdentifier);
	FTCHARToUTF8 UTF8JSON(JSON);

	int32 len = UTF8LibraryIdentifier.Length();
	writer << len;
	writer.Serialize((ANSICHAR*)UTF8LibraryIdentifier.Get(), UTF8LibraryIdentifier.Length());
	
	len = UTF8JSON.Length();
	writer << len;
	writer.Serialize((ANSICHAR*)UTF8JSON.Get(), UTF8JSON.Length());
}


FOPKCmdPlayHaptic* FOPKCmdPlayHaptic::Create(uint16_t deviceIdx, FString const& libraryIdentifier, FString const& effectId)
{
	return new FOPKCmdPlayHaptic(deviceIdx, libraryIdentifier, effectId);
}

///////////////////////////////////////////////////////////////////////////////////////

void FOPKCmdPlayHaptic::BuildCSV(TCSVBuilder& writer)
{
	FOPKBase::BuildCSV(writer);

	writer.AppendChar(',');
	writer.Append(LibraryIdentifier);

	writer.AppendChar(',');
	writer.Append(EffectId);

	writer.AppendChar(',');
	writer.Append("1");

	writer.AppendChar(',');
	writer.Append("1");

	writer.AppendChar(',');
	writer.Append("1");
}

///////////////////////////////////////////////////////////////////////////////////////

FPKBase* FPKFactory::PacketFromType(int32 PacketType)
{
	switch (PacketType)
	{
	case FPKDeviceState::PacketType:
		return new FPKDeviceState();
	case FPKDeviceButtons::PacketType:
		return new FPKDeviceButtons();
	case FPKRawSensors::PacketType:
		return new FPKRawSensors();
	case FPKDeviceAcc::PacketType:
		return new FPKDeviceAcc();
	case FPKTouchpad::PacketType:
		return new FPKTouchpad();
	case FPKCalculatedSensors::PacketType:
		return new FPKCalculatedSensors();
	case FPKGestureEvent::PacketType:
		return new FPKGestureEvent();
	default:
		UE_LOG(LogStrikerVR, Warning, TEXT("Unknown packet type received: %d"), PacketType);
		return nullptr;
	}
	return nullptr;
}

///////////////////////////////////////////////////////////////////////////////////////

bool FPKBase::ReadFromCSV(const TArray<const TCHAR*>& Row, uint32_t& Col)
{
	int32 RowPacketType = READ_COL_INT(Row, Col);

	if (PacketType != RowPacketType)
		return false;

	PacketVersion = READ_COL_INT(Row, Col);
	DeviceIndex = READ_COL_INT(Row, Col);

	return true;
}

bool FPKBase::ReadFromArray(TArray<uint8> const& RawFrame, size_t& Offset)
{
	int32 DataPacketType = READ_INT(RawFrame, Offset);

	if (PacketType != DataPacketType)
		return false;

	PacketVersion = READ_SHORT(RawFrame, Offset);
	DeviceIndex = READ_SHORT(RawFrame, Offset);

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////

void FPKDeviceState::ApplyToState(FStrikerVRData& state)
{
	auto& deviceState = state.Devices[DeviceIndex].DeviceState;

	deviceState.Guid = Guid;
	deviceState.Connected = Connected;

	deviceState.DeviceType = DeviceType;
	deviceState.CoverPins = CoverPins;

	deviceState.IsReady = IsReady;
	deviceState.RSSI = RSSI;

	deviceState.BatteryLevel = BatteryLevel;
	deviceState.BatteryChargeState = BatteryChargeState;

	deviceState.PosOffsetX = PosOffsetX;
	deviceState.PosOffsetY = PosOffsetY;
	deviceState.PosOffsetZ = PosOffsetZ;

	deviceState.RotOffsetX = RotOffsetX;
	deviceState.RotOffsetY = RotOffsetY;
	deviceState.RotOffsetZ = RotOffsetZ;
}

void FPKDeviceState::Log()
{
	return;
	UE_LOG(LogStrikerVR, Warning, TEXT("FPKDeviceState: Guid %s, Connected %d, DeviceType %d, CoverPins %d, IsReady %d, RSSI %d, BatteryLevel %f, BatteryChargeState %d,  Pos {%f %f %f} Rot {%f, %f, %f}"),
		*Guid,
		Connected,
		DeviceType,
		CoverPins,
		IsReady,
		RSSI,
		BatteryLevel,
		BatteryChargeState,
		PosOffsetX, PosOffsetY, PosOffsetZ,
		RotOffsetX, RotOffsetY, RotOffsetZ);
}



bool FPKDeviceState::ReadFromCSV(const TArray<const TCHAR*>& Row, uint32_t& Col)
{
	if (FPKBase::ReadFromCSV(Row, Col) == false)
		return false;

	// Read out our data
	Guid = READ_COL_STRING(Row, Col);
	Connected = READ_COL_BOOL(Row, Col);

	DeviceType = FCString::Atoi(Row[Col++]); // TODO: Make Enum
	CoverPins = Col++; // TODO: From HEX

	IsReady = READ_COL_BOOL(Row, Col);
	RSSI = READ_COL_INT(Row, Col);
	BatteryLevel = READ_COL_FLOAT(Row, Col);
	BatteryChargeState = READ_COL_INT(Row, Col);

	PosOffsetX = READ_COL_FLOAT(Row, Col);
	PosOffsetY = READ_COL_FLOAT(Row, Col);
	PosOffsetZ = READ_COL_FLOAT(Row, Col);

	RotOffsetX = READ_COL_FLOAT(Row, Col);
	RotOffsetY = READ_COL_FLOAT(Row, Col);
	RotOffsetZ = READ_COL_FLOAT(Row, Col);

	return true;
}


///////////////////////////////////////////////////////////

void FPKDeviceButtons::ApplyToState(FStrikerVRData& state)
{
	auto& buttons = state.Devices[DeviceIndex].DeviceButtons;

	buttons.SideLeft = SideLeft;
	buttons.SideRight = SideRight;
	buttons.MenuTop = MenuTop;
	buttons.MenuBottom = MenuBottom;
	buttons.TouchpadLeft = TouchpadLeft;
	buttons.TouchpadRight = TouchpadRight;
	buttons.TriggerActive = TriggerActive;
	buttons.TriggerMode = TriggerMode;
	buttons.TriggerAxis = TriggerAxis;
}

void FPKDeviceButtons::Log()
{
	return;
	UE_LOG(LogStrikerVR, Warning, TEXT("FPKDeviceButtons: SideLeft %d, SideRight %d, MenuTop %d, MenuBottom %d, TouchPadLeft %d, TouchPadRight %d, TriggerActive %d, TriggerMode %d,  TriggerAxis %x"),
	       SideLeft,
	       SideRight,
	       MenuTop,
	       MenuBottom,
	       TouchpadLeft,
	       TouchpadRight,
	       TriggerActive,
	       TriggerMode,
	       TriggerAxis);
}

bool FPKDeviceButtons::ReadFromArray(TArray<uint8> const& RawFrame, size_t& Offset)
{
	if (FPKBase::ReadFromArray(RawFrame, Offset) == false)
		return false;

	uint8 button_state = READ_UBYTE(RawFrame, Offset);

	SideLeft = BIT(button_state, 0);
	SideRight = BIT(button_state, 1);
	MenuTop = BIT(button_state, 2);
	MenuBottom = BIT(button_state, 3);
	TouchpadLeft = BIT(button_state, 4);
	TouchpadRight = BIT(button_state, 5);
	TriggerActive = BIT(button_state, 6);
	TriggerMode = BIT(button_state, 7);

	TriggerAxis = READ_UBYTE(RawFrame, Offset);


	return true;
}

///////////////////////////////////////////////////////////

void FPKRawSensors::ApplyToState(FStrikerVRData& state)
{
	auto& raw = state.Devices[DeviceIndex].RawData;

	raw.ForwardBarGrip = ForwardBarGrip;
	raw.FrontHandGrip = FrontHandGrip;
	raw.Reload = Reload;
	raw.Slide = Slide;
	raw.TriggerGripLeft = TriggerGripLeft;
	raw.TriggerGripRight = TriggerGripRight;
	raw.TriggerTouch = TriggerTouch;
	raw.UnderTouchpadGrip = UnderTouchpadGrip;
}

void FPKRawSensors::Log()
{
	return;
	UE_LOG(LogStrikerVR, Warning, TEXT("FPKRawSensors: Slide %d, Reload %d, TriggerGripLeft %d, TriggerGripRight %d, TriggerTouch %d, ForwardBarGrip %d, UnderTouchpadGrip %d, FrontHandGrip %d"),
		Slide,
		Reload,
		TriggerGripLeft,
		TriggerGripRight,
		TriggerTouch,
		ForwardBarGrip,
		UnderTouchpadGrip,
		FrontHandGrip);
}

bool FPKRawSensors::ReadFromArray(TArray<uint8> const& RawFrame, size_t& Offset)
{
	if (FPKBase::ReadFromArray(RawFrame, Offset) == false)
		return false;

	Slide = READ_SHORT(RawFrame, Offset);
	Reload = READ_UBYTE(RawFrame, Offset);

	uint8 TriggerBits = READ_UBYTE(RawFrame, Offset);

	TriggerGripLeft = BIT(TriggerBits, 0);
	TriggerGripRight = BIT(TriggerBits, 1);
	TriggerTouch = BIT(TriggerBits, 2);
	
	ForwardBarGrip = READ_SHORT(RawFrame, Offset);
	UnderTouchpadGrip = READ_UBYTE(RawFrame, Offset);
	FrontHandGrip = READ_UBYTE(RawFrame, Offset);

	return true;
}

///////////////////////////////////////////////////////////

void FPKDeviceAcc::ApplyToState(FStrikerVRData& state)
{
	auto& accel = state.Devices[DeviceIndex].Accell;

	accel.AccX = AccX;
	accel.AccY = AccY;
	accel.AccZ = AccZ;
}

void FPKDeviceAcc::Log()
{
	return;
	UE_LOG(LogStrikerVR, Warning, TEXT("FPKDeviceAcc: AccX %f, AccY %f, AccZ %f"),
		AccX,
		AccY,
		AccZ);
}

bool FPKDeviceAcc::ReadFromArray(TArray<uint8> const& RawFrame, size_t& Offset)
{
	if (FPKBase::ReadFromArray(RawFrame, Offset) == false)
		return false;

	AccX = READ_FLOAT(RawFrame, Offset);
	AccY = READ_FLOAT(RawFrame, Offset);
	AccZ = READ_FLOAT(RawFrame, Offset);

	return true;
}

///////////////////////////////////////////////////////////

void FPKTouchpad::ApplyToState(FStrikerVRData& state)
{
	auto& touchpad = state.Devices[DeviceIndex].TouchPad;

	touchpad.TouchpadLeftX = TouchpadLeftX;
	touchpad.TouchpadLeftY = TouchpadLeftY;
	touchpad.TouchpadLeftZ = TouchpadLeftZ;
	touchpad.TouchpadRightX = TouchpadRightX;
	touchpad.TouchpadRightY = TouchpadRightY;
	touchpad.TouchpadRightZ = TouchpadRightZ;
}

void FPKTouchpad::Log()
{
	return;
	UE_LOG(LogStrikerVR, Warning, TEXT("FPKTouchpad: TouchpadLeftX %f, TouchpadLeftY %f, TouchpadLeftZ %f, TouchpadRightX %f, TouchpadRightY %f, TouchpadRightZ %f"),
		TouchpadLeftX,
		TouchpadLeftY,
		TouchpadLeftZ,
		TouchpadRightX,
		TouchpadRightY,
		TouchpadRightZ);
}


bool FPKTouchpad::ReadFromArray(TArray<uint8> const& RawFrame, size_t& Offset)
{
	if (FPKBase::ReadFromArray(RawFrame, Offset) == false)
		return false;

	TouchpadLeftX = READ_FLOAT(RawFrame, Offset);
	TouchpadLeftY = READ_FLOAT(RawFrame, Offset);
	TouchpadLeftZ = READ_FLOAT(RawFrame, Offset);


	TouchpadRightX = READ_FLOAT(RawFrame, Offset);
	TouchpadRightY = READ_FLOAT(RawFrame, Offset);
	TouchpadRightZ = READ_FLOAT(RawFrame, Offset);

	return true;
}

///////////////////////////////////////////////////////////

void FPKCalculatedSensors::ApplyToState(FStrikerVRData& state)
{
	auto& sensors = state.Devices[DeviceIndex].CalculatedSensors;
	
	sensors.SlideTouched = SlideTouched;
	sensors.SlideTouchAmount = SlideTouchAmount;
	sensors.SlidePosition = SlidePosition;

	sensors.ReloadTouched = ReloadTouched;
	sensors.ReloadTouchAmount = ReloadTouchAmount;

	sensors.ForwardBarGripTouched = ForwardBarGripTouched;
	sensors.ForwardBarGripTouchAmount = ForwardBarGripTouchAmount;
	sensors.ForwardBarGripPosition = ForwardBarGripPosition;

	sensors.UnderTouchpadGripTouched = UnderTouchpadGripTouched;
	sensors.UnderTouchpadGripTouchAmount = UnderTouchpadGripTouchAmount;
	sensors.UnderTouchpadGripPosition = UnderTouchpadGripPosition;

	sensors.FrontHandGripTouched = FrontHandGripTouched;
	sensors.FrontHandGripTouchAmount = FrontHandGripTouchAmount;
	sensors.FrontHandGripFaceTouched = FrontHandGripFaceTouched;
	sensors.FrontHandGripFaceTouchAmount = FrontHandGripFaceTouchAmount;

	sensors.TriggerGripTouched = TriggerGripTouched;
	sensors.TriggerGripTouchAmount = TriggerGripTouchAmount;
}

void FPKCalculatedSensors::Log()
{

	UE_LOG(LogStrikerVR, Warning, TEXT("FPKCalculatedSensors: SlideTouched %d, SlideTouchAmount %f, SlidePosition %f"),
		SlideTouched,
		SlideTouchAmount,
		SlidePosition
	);

	UE_LOG(LogStrikerVR, Warning, TEXT("FPKCalculatedSensors: ReloadTouched %d, ReloadTouchAmount %f"),
		ReloadTouched,
		ReloadTouchAmount
	);

	UE_LOG(LogStrikerVR, Warning, TEXT("FPKCalculatedSensors: ForwardBarGripTouched %d, ForwardBarGripTouchAmount %f, ForwardBarGripPosition %f"),
		ForwardBarGripTouched,
		ForwardBarGripTouchAmount,
		ForwardBarGripPosition
	);

	UE_LOG(LogStrikerVR, Warning, TEXT("FPKCalculatedSensors: UnderTouchpadGripTouched %d, UnderTouchpadGripTouchAmount %f, UnderTouchpadGripPosition %f"),
		UnderTouchpadGripTouched,
		UnderTouchpadGripTouchAmount,
		UnderTouchpadGripPosition
	);

	UE_LOG(LogStrikerVR, Warning, TEXT("FPKCalculatedSensors: FrontHandGripTouched %d, FrontHandGripTouchAmount %f, FrontHandGripFaceTouched %d, FrontHandGripFaceTouchAmount %f"),
		FrontHandGripTouched,
		FrontHandGripTouchAmount,
		FrontHandGripFaceTouched,
		FrontHandGripFaceTouchAmount
	);

	UE_LOG(LogStrikerVR, Warning, TEXT("FPKCalculatedSensors: TriggerGripTouched %d, TriggerGripTouchAmount %f"),
		TriggerGripTouched,
		TriggerGripTouchAmount
	);
}



bool FPKCalculatedSensors::ReadFromArray(TArray<uint8> const& RawFrame, size_t& Offset)
{
	if (FPKBase::ReadFromArray(RawFrame, Offset) == false)
		return false;

	SlideTouched = READ_BOOL(RawFrame, Offset);
	SlideTouchAmount = READ_FLOAT(RawFrame, Offset);
	SlidePosition = READ_FLOAT(RawFrame, Offset);

	ReloadTouched = READ_BOOL(RawFrame, Offset);
	ReloadTouchAmount = READ_FLOAT(RawFrame, Offset);

	ForwardBarGripTouched = READ_BOOL(RawFrame, Offset);
	ForwardBarGripTouchAmount = READ_FLOAT(RawFrame, Offset);
	ForwardBarGripPosition = READ_FLOAT(RawFrame, Offset);

	UnderTouchpadGripTouched = READ_BOOL(RawFrame, Offset);
	UnderTouchpadGripTouchAmount = READ_FLOAT(RawFrame, Offset);
	UnderTouchpadGripPosition = READ_FLOAT(RawFrame, Offset);

	FrontHandGripTouched = READ_BOOL(RawFrame, Offset);
	FrontHandGripTouchAmount = READ_FLOAT(RawFrame, Offset);

	FrontHandGripFaceTouched = READ_BOOL(RawFrame, Offset);
	FrontHandGripFaceTouchAmount = READ_FLOAT(RawFrame, Offset);

	TriggerGripTouched = READ_BOOL(RawFrame, Offset);
	TriggerGripTouchAmount = READ_FLOAT(RawFrame, Offset);

	return true;
}

///////////////////////////////////////////////////////////

void FPKGestureEvent::ApplyToState(FStrikerVRData& state)
{	
	FStrikerGestureData gestureData;
	gestureData.GestureId = GestureId;
	gestureData.GestureSpeed = GestureSpeed;
	gestureData.GestureProgress = GestureProgress;

	state.Devices[DeviceIndex].Gestures.Add(GestureId, gestureData);
}

void FPKGestureEvent::Log()
{

	UE_LOG(LogStrikerVR, Log, TEXT("FPKGestureEvent: GestureId: %s EventType %d, GestureSpeed %f, GestureProgress %f"),
	       *GestureId,
	       EventType,
	       GestureSpeed,
	       GestureProgress
	       );
}

bool FPKGestureEvent::ReadFromCSV(const TArray<const TCHAR*>& Row, uint32_t& Col)
{
	if (FPKBase::ReadFromCSV(Row, Col) == false)
		return false;

	// Read out our data
	GestureId = READ_COL_STRING(Row, Col);
	EventType = (GestureEventType)READ_COL_INT(Row, Col);

	if (EventType == Progress)
	{
		//GestureSpeed = READ_COL_FLOAT(Row, Col); - Not sure if this is sent?
		GestureProgress = READ_COL_FLOAT(Row, Col);
	}
	
	return true;
}

///////////////////////////////////////////////////////////