#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StrikerVrHapticDataAsset.h"
#include "StrikerDevice.generated.h"

class UStrikerVRSubsystem;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UStrikerDevice : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UStrikerDevice();

	UFUNCTION(BlueprintCallable)
	FStrikerVRDeviceData const& DeviceData() const;

	UPROPERTY(EditAnywhere, meta = (ClampMax=15, ClampMin=0))
	uint32 DeviceIndex;

	UFUNCTION(BlueprintCallable, Category = StrikerVR)
	void PlayHaptic(FString effectId, UStrikerVrHapticLibrary* library);
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	UStrikerVRSubsystem* strikerSubsystem;
		
};
