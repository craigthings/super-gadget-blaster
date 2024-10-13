#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "StrikerVrHapticDataAsset.generated.h"


UENUM(BlueprintType)
enum class UStrikerHapticEffectCommand : uint8 {
	TICK = 0 UMETA(DisplayName = "TICK"),
	PULSE = 1 UMETA(DisplayName = "PULSE"),
	VIBRATE = 2 UMETA(DisplayName = "VIBRATE"),
	PAUSE = 3 UMETA(DisplayName = "PAUSE"),

	COUNT = 4
};

UENUM(BlueprintType)
enum class UStrikerHapticSampleDevice: uint8 {
	THUNDER = 0 UMETA(DisplayName = "Thunder"),

	CRICKETS_BOTH = 1 UMETA(DisplayName = "CricketsBoth"),
	CRICKETS_FRONT = 2 UMETA(DisplayName = "CricketsFront"),
	CIRCKETS_BACK = 3 UMETA(DisplayName = "CricketsBack"),

	COUNT = 4
};

USTRUCT(BlueprintType)
struct STRIKERVR_API FStrikerHapticSampleSequenceCommand
{
	GENERATED_BODY()

public:
	FStrikerHapticSampleSequenceCommand()
 		: Version("2.0")
		, Command(UStrikerHapticEffectCommand::TICK)
		, Frequency(0)	
		, FrequencyTime(0)	
		, DurationBase(0)
		, Intensity(0)
		, IntensityTime(0)
		, Waveform(0)
		, Overlay(false)
		, Overdrive(false)
	{}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Version;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStrikerHapticEffectCommand Command;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Frequency;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 FrequencyTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 DurationBase;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Intensity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 IntensityTime;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Waveform;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool Overlay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool Overdrive;
};

UCLASS(BlueprintType)
class STRIKERVR_API UStrikerHapticEffectSample : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString SampleID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FStrikerHapticSampleSequenceCommand> Sequence;
};

// ----------------------------------------------

USTRUCT(BlueprintType)
struct STRIKERVR_API FStrikerHapticLibraryTrackSample {
	GENERATED_BODY()

public:
	FStrikerHapticLibraryTrackSample()
	: IsModifyIntensity(false)
	, IsModifyFrequency(false)
	, IsModifyDuration(false)
	, DurationTarget(0)
	, Overlay(false)
	, Overdrive(false)
	, Waveform(0)
	{}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsModifyIntensity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsModifyFrequency;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsModifyDuration;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int DurationTarget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool Overlay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool Overdrive;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Waveform;

	// Reference to the UStrikerHapticEffectSample
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UStrikerHapticEffectSample> Sample;
};

USTRUCT(BlueprintType)
struct STRIKERVR_API FStrikerHapticLibraryTrack {
	GENERATED_BODY()

public:
	FStrikerHapticLibraryTrack()
		: DeviceId(UStrikerHapticSampleDevice::CRICKETS_BOTH)
	{}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStrikerHapticSampleDevice DeviceId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FStrikerHapticLibraryTrackSample> Sequence;

};

USTRUCT(BlueprintType)
struct STRIKERVR_API FStrikerHapticLibraryEffect {
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString EffectID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FStrikerHapticLibraryTrack> Tracks;
};

/**
 * 
 */
UCLASS(BlueprintType, Meta = (DisplayName = "Striker VR Haptic Library"))
class STRIKERVR_API UStrikerVrHapticLibrary: public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString LibraryIdentifier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FStrikerHapticLibraryEffect> Effects;
};
