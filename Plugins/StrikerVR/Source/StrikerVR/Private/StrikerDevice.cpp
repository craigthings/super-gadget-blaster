#include "StrikerDevice.h"

#include "StrikerVR.h"
#include "StrikerVRSubsystem.h"
#include "FPKFactory.h"

UStrikerDevice::UStrikerDevice()
{
	PrimaryComponentTick.bCanEverTick = true;
}

FStrikerVRDeviceData const& UStrikerDevice::DeviceData() const
{
	return strikerSubsystem->DeviceData().Devices[DeviceIndex];
}

void UStrikerDevice::PlayHaptic(FString effectId, UStrikerVrHapticLibrary* library)
{
	if (library == nullptr)
	{
		UE_LOG(LogStrikerVR, Error, TEXT("No Striker Haptic Library specified"));
		return;
	}

	auto* packet = FOPKCmdPlayHaptic::Create(DeviceIndex, library->LibraryIdentifier, effectId);
	strikerSubsystem->SendStringPacket(packet);
}

void UStrikerDevice::BeginPlay()
{
	Super::BeginPlay();
	strikerSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UStrikerVRSubsystem>();	
}


void UStrikerDevice::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

