#include "StrikerVR.h"

#include "Engine/GameInstance.h"
#include "Engine.h"

#include "IInputDevice.h"

#include "StrikerVRState.h"
#include "StrikerVRSubsystem.h"

#define LOCTEXT_NAMESPACE "FStrikerVRModule"

static UStrikerVRSubsystem* g_CurrentStrikerSubsytem = nullptr;

class FStrikerController : public IInputDevice
{
	TSharedPtr< FGenericApplicationMessageHandler > MessageHandler;
	FStrikerVRDeviceButtons LastButtonState;

public:
	
	FStrikerController()	
	{
	}

	/** Tick the interface (e.g. check for new controllers) */
	virtual void Tick(float DeltaTime)
	{
	}

	/** Poll for controller state and send events if needed */
	virtual void SendControllerEvents()
	{
		if (g_CurrentStrikerSubsytem == nullptr)
			return;		

		FPlatformUserId PlatUser = IPlatformInputDeviceMapper::Get().GetPrimaryPlatformUser();
		FInputDeviceId DeviceId = IPlatformInputDeviceMapper::Get().GetDefaultInputDevice();
	
		FStrikerVRDeviceButtons const& buttons = g_CurrentStrikerSubsytem->DeviceData().Devices[0].DeviceButtons;

		bool IsRepeat = false;

		{
			bool downStroke = buttons.TriggerAxis > 200;
			bool upStroke = buttons.TriggerAxis < 50;

			if (downStroke) {
				MessageHandler->OnControllerButtonPressed(FStrikerVRKeyNames::StrikerVR_Trigger, PlatUser, DeviceId, IsRepeat);
			}

			if (upStroke) {
				MessageHandler->OnControllerButtonReleased(FStrikerVRKeyNames::StrikerVR_Trigger, PlatUser, DeviceId, IsRepeat);
			}
		}

		if (!LastButtonState.MenuTop && buttons.MenuTop) {
			MessageHandler->OnControllerButtonPressed(FStrikerVRKeyNames::StrikerVR_FrontTop, PlatUser, IsRepeat);
		} else if (LastButtonState.MenuTop && !buttons.TouchpadRight) {
			MessageHandler->OnControllerButtonReleased(FStrikerVRKeyNames::StrikerVR_FrontTop, PlatUser,  IsRepeat);
		}

		if (!LastButtonState.MenuBottom && buttons.MenuBottom) {
			MessageHandler->OnControllerButtonPressed(FStrikerVRKeyNames::StrikerVR_FrontBottom, PlatUser, IsRepeat);
		}
		else  if (LastButtonState.MenuBottom && !buttons.TouchpadRight) {
			MessageHandler->OnControllerButtonReleased(FStrikerVRKeyNames::StrikerVR_FrontBottom, PlatUser, IsRepeat);
		}

		if (!LastButtonState.SideLeft && buttons.SideLeft) {
			MessageHandler->OnControllerButtonPressed(FStrikerVRKeyNames::StrikerVR_TriggerLeft, PlatUser, IsRepeat);
		}
		else  if (LastButtonState.SideLeft && !buttons.TouchpadRight) {
			MessageHandler->OnControllerButtonReleased(FStrikerVRKeyNames::StrikerVR_TriggerLeft, PlatUser, IsRepeat);
		}

		if (!LastButtonState.SideRight && buttons.SideRight) {
			MessageHandler->OnControllerButtonPressed(FStrikerVRKeyNames::StrikerVR_TriggerRight, PlatUser, IsRepeat);
		}
		else  if (LastButtonState.SideRight && !buttons.TouchpadRight) {
			MessageHandler->OnControllerButtonReleased(FStrikerVRKeyNames::StrikerVR_TriggerRight, PlatUser, IsRepeat);
		}

		if (!LastButtonState.TouchpadLeft && buttons.TouchpadLeft) {
			MessageHandler->OnControllerButtonPressed(FStrikerVRKeyNames::StrikerVR_CircleLeft, PlatUser, IsRepeat);
		}
		else  if (LastButtonState.TouchpadLeft && !buttons.TouchpadRight) {
			MessageHandler->OnControllerButtonReleased(FStrikerVRKeyNames::StrikerVR_CircleLeft, PlatUser, IsRepeat);
		}

		if (!LastButtonState.TouchpadRight && buttons.TouchpadRight) {
			MessageHandler->OnControllerButtonPressed(FStrikerVRKeyNames::StrikerVR_CircleRight, PlatUser, IsRepeat);
		}
		else if (LastButtonState.TouchpadRight && !buttons.TouchpadRight) {
			MessageHandler->OnControllerButtonReleased(FStrikerVRKeyNames::StrikerVR_CircleRight, PlatUser, IsRepeat);
		}

		LastButtonState = buttons;
	}



	virtual bool Exec(UWorld* InWorld, const TCHAR* Cmd, FOutputDevice& Ar)
	{
		return false;
	}

	virtual void SetChannelValue (int32 ControllerId, FForceFeedbackChannelType ChannelType, float Value)
	{
	}
	virtual void SetChannelValues (int32 ControllerId, const FForceFeedbackValues &values)
	{
	}

	virtual void SetMessageHandler(const TSharedRef<FGenericApplicationMessageHandler>& InMessageHandler)
	{
		MessageHandler = InMessageHandler;
	}

	virtual bool IsGamepadAttached() const
	{
		// TODO: Return the connected state of StrikerLink
		return false;
	}
};

void SetStrikerSubsystem(UStrikerVRSubsystem* current)
{
	g_CurrentStrikerSubsytem = current;
}

static void PreInit()
{
	EKeys::AddMenuCategoryDisplayInfo("StrikerVR", LOCTEXT("StrikerVRSubCategory", "StrikerVR"), TEXT("GraphEditor.PadEvent_16x"));
	EKeys::AddKey(FKeyDetails(FStrikerVRKey::StrikerVR_Trigger, LOCTEXT("StrikerVR_Trigger", "Trigger"), FKeyDetails::GamepadKey | FKeyDetails::GamepadKey, "StrikerVR"));

	EKeys::AddKey(FKeyDetails(FStrikerVRKey::StrikerVR_FrontTop, LOCTEXT("StrikerVR_FrontTop", "FrontTop"), FKeyDetails::GamepadKey | FKeyDetails::GamepadKey, "StrikerVR"));
	EKeys::AddKey(FKeyDetails(FStrikerVRKey::StrikerVR_FrontBottom, LOCTEXT("StrikerVR_FrontBottom", "FrontBottom"), FKeyDetails::GamepadKey | FKeyDetails::GamepadKey, "StrikerVR"));

	EKeys::AddKey(FKeyDetails(FStrikerVRKey::StrikerVR_TriggerRight, LOCTEXT("StrikerVR_TriggerRight", "TriggerRight"), FKeyDetails::GamepadKey | FKeyDetails::GamepadKey, "StrikerVR"));
	EKeys::AddKey(FKeyDetails(FStrikerVRKey::StrikerVR_TriggerLeft, LOCTEXT("StrikerVR_TriggerLeft", "TriggerLeft"), FKeyDetails::GamepadKey | FKeyDetails::GamepadKey, "StrikerVR"));

	EKeys::AddKey(FKeyDetails(FStrikerVRKey::StrikerVR_CircleRight, LOCTEXT("StrikerVR_CircleRight", "CircleRight"), FKeyDetails::GamepadKey | FKeyDetails::GamepadKey, "StrikerVR"));
	EKeys::AddKey(FKeyDetails(FStrikerVRKey::StrikerVR_CircleLeft, LOCTEXT("StrikerVR_CircleLeft", "CircleLeft"), FKeyDetails::GamepadKey | FKeyDetails::GamepadKey, "StrikerVR"));

	UE_LOG(LogStrikerVR, Log, TEXT("FStrikerVRModule::PreInit"));
}

void FStrikerVRModule::StartupModule()
{
	UE_LOG(LogStrikerVR, Log, TEXT("FStrikerVRModule::StartupModule"));
	IInputDeviceModule::StartupModule();
	PreInit();
}

void FStrikerVRModule::ShutdownModule()
{
	IInputDeviceModule::ShutdownModule();
}

TSharedPtr<class IInputDevice> FStrikerVRModule::CreateInputDevice(const TSharedRef<FGenericApplicationMessageHandler>& InMessageHandler)
{
	UE_LOG(LogStrikerVR, Log, TEXT("FStrikerVRModule::CreateInputDevice"));

	TSharedPtr<class IInputDevice> inputDevice = TSharedPtr<class IInputDevice>(new FStrikerController());	
	inputDevice->SetMessageHandler(InMessageHandler);

	return inputDevice;
}


#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FStrikerVRModule, StrikerVR)

DEFINE_LOG_CATEGORY(LogStrikerVR);