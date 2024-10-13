#pragma once

#include "CoreMinimal.h"

#include "WebSocketsModule.h" // Module definition
#include "IWebSocket.h"       // Socket definition

#include "StrikerVRState.h"

#include "Subsystems/GameInstanceSubsystem.h"
#include "StrikerVrHapticDataAsset.h"
#include "StrikerVRSubsystem.generated.h"

class FOPKBase;

/**
 * 
 */
UCLASS()
class UStrikerVRSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UFUNCTION(BlueprintCallable, Category = StrikerVR)
	FStrikerVRData const& DeviceData() const { return currentState; }

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStrikerConnected);

	UPROPERTY(BlueprintAssignable, Category = StrikerVR)
	FOnStrikerConnected OnStrikerConnected;

	UFUNCTION(BlueprintCallable, Category = StrikerVR)
	void SendHapticLibrary(UStrikerVrHapticLibrary* library);

	void SendBinaryPacket(FOPKBase* packet);
	void SendStringPacket(FOPKBase* packet);
protected:
	void OnRawMessage(const uint8* Data, size_t Length, size_t BytesLeft, bool IsBinary);
	

	class FPKBase* ParseCSVFrame(FString InSourceString);
	class FPKBase* ParseBinaryFrame(TArray<uint8> const& RawFrame);

private:
	TSharedPtr<IWebSocket> Socket;
	TArray<uint8> RawRecieveBuffer;
	FString RecieveBuffer;

	FStrikerVRData currentState;
};
