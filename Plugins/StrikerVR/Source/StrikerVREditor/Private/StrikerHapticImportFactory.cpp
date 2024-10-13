#include "StrikerHapticImportFactory.h"

#include "StrikerVREditor.h"
#include "StrikerVrHapticDataAsset.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonReader.h"
#include "Misc/Paths.h"
#include "AssetRegistry/AssetRegistryModule.h"

UStrikerHapticImportFactory::UStrikerHapticImportFactory(FObjectInitializer const &)
{
	bEditorImport = true;
	bText = true;
	SupportedClass = UStrikerVrHapticLibrary::StaticClass();

	Formats.Add(TEXT("haptic;Striver haptic file"));
}


FText UStrikerHapticImportFactory::GetDisplayName() const
{
	return FText::FromString("StrikerVR Haptic Library");
}

static UStrikerHapticEffectCommand HapticEffectCommandFromString(FString const& string)
{
	if (string == "tick") return UStrikerHapticEffectCommand::TICK;
	if (string == "pulse") return UStrikerHapticEffectCommand::PULSE;
	if (string == "vibrate") return UStrikerHapticEffectCommand::VIBRATE;
	if (string == "pause") return UStrikerHapticEffectCommand::PAUSE;
	
	UE_LOG(LogStrikerVREditor, Warning, TEXT("Unrecognized HapticEffectCommand %s"), *string);
	return UStrikerHapticEffectCommand::COUNT;
}

static UStrikerHapticSampleDevice HapticSampleDeviceFromString(FString const& string)
{
	if (string == "fosters") return UStrikerHapticSampleDevice::CRICKETS_BOTH;
	if (string == "fosterBack") return UStrikerHapticSampleDevice::CIRCKETS_BACK;
	if (string == "fosterFront") return UStrikerHapticSampleDevice::CRICKETS_FRONT;
	if (string == "hammerTop") return UStrikerHapticSampleDevice::THUNDER;

	UE_LOG(LogStrikerVREditor, Warning, TEXT("Unrecognized StrikerHapticSampleDevice %s"), *string);
	return UStrikerHapticSampleDevice::COUNT;
}

UObject* UStrikerHapticImportFactory::FactoryCreateText(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, const TCHAR* Type, const TCHAR*& Buffer, const TCHAR* BufferEnd, FFeedbackContext* Warn, bool& bOutOperationCanceled)
{
	TMap<FString, UStrikerHapticEffectSample*> referencedSamples;

	const size_t bufferLen = BufferEnd - Buffer;
	const FString jsonData = FString(bufferLen, Buffer);
	
	TSharedPtr<FJsonObject> HapticLibraryObj;
	auto JsonReader = TJsonReaderFactory<>::Create( jsonData );

	if( FJsonSerializer::Deserialize( JsonReader, HapticLibraryObj) == false)
	{
		UE_LOG(LogStrikerVREditor, Error, TEXT("Unable to parse haptic file during import %s"), *InName.ToString());
		return nullptr;
	}

	const auto samplesArrayObj = HapticLibraryObj->Values.FindRef(TEXT("samples_palette"));
	const auto effectsArrayObj = HapticLibraryObj->Values.FindRef(TEXT("effects"));

	auto& samplesArray = samplesArrayObj->AsArray();
	auto& effectsArray = effectsArrayObj->AsArray();	

	// For each sample, create a new sample in the current folder
	for (auto& sample : samplesArray)
	{
		auto &sampleObj = sample->AsObject();

		auto sampleId = sampleObj->GetStringField("sample_id");
		auto& sequence = sampleObj->GetArrayField("sequence");

		// TODO: Create a UStrikerHapticEffectSample asset
		UE_LOG(LogStrikerVREditor, Verbose, TEXT("Sample %s"), *sampleId);
		UStrikerHapticEffectSample* newSample = NewObject<UStrikerHapticEffectSample>(InParent, UStrikerHapticEffectSample::StaticClass(), FName(sampleId), Flags);
		newSample->SampleID = sampleId;

		for (auto& sequenceItem : sequence) {
			auto& sequenceItemObj = sequenceItem->AsObject();

			auto version = sequenceItemObj->GetStringField("version");
			auto command = sequenceItemObj->GetStringField("command");
			auto frequency = sequenceItemObj->GetNumberField("frequency");
			auto frequency_time = sequenceItemObj->GetNumberField("frequency_time");
			auto durationBase = sequenceItemObj->GetNumberField("duration_base");
			auto intensity = sequenceItemObj->GetNumberField("intensity");
			auto intensity_time = sequenceItemObj->GetNumberField("intensity_time");
			auto waveform = sequenceItemObj->GetNumberField("waveform");

			auto overlay = sequenceItemObj->GetBoolField("overlay");
			auto overDrive= sequenceItemObj->GetBoolField("overDrive");		
			

			FStrikerHapticSampleSequenceCommand sequenceCommand;
			sequenceCommand.Version = version;
			sequenceCommand.Command = HapticEffectCommandFromString(command);
			sequenceCommand.Frequency = frequency;
			sequenceCommand.FrequencyTime = frequency_time;
			sequenceCommand.DurationBase = durationBase;
			sequenceCommand.Intensity = (float)intensity;
			sequenceCommand.IntensityTime = intensity_time;
			sequenceCommand.Waveform = waveform;

			sequenceCommand.Overlay = overlay;
			sequenceCommand.Overdrive = overDrive;

			newSample->Sequence.Add(sequenceCommand);

			UE_LOG(LogStrikerVREditor, Verbose, TEXT("-- %s %s %lg %lg %lg %lg %lg %lg %d %d"), *version, *command, frequency, frequency_time, durationBase, intensity, intensity_time, waveform, overlay, overDrive);
		}

		AdditionalImportedObjects.Add(newSample);

		newSample->MarkPackageDirty();
		newSample->PostEditChange();
		FAssetRegistryModule::AssetCreated(newSample);

		referencedSamples.Add(newSample->SampleID, newSample);
	}	

	UStrikerVrHapticLibrary* newLibrary = NewObject<UStrikerVrHapticLibrary>(InParent, UStrikerVrHapticLibrary::StaticClass(), InName, Flags);
	newLibrary->LibraryIdentifier = InName.ToString();

	for (auto& effect : effectsArray) 
	{
		auto& effectObj = effect->AsObject();
		FStrikerHapticLibraryEffect newEffect;
		newEffect.EffectID = effectObj->GetStringField("effect_id");

		auto& tracksArray = effectObj->GetArrayField("tracks");
		for (auto& track : tracksArray) {
			auto& trackObj = track->AsObject();
			
			FStrikerHapticLibraryTrack newTrack;
			newTrack.DeviceId = HapticSampleDeviceFromString(trackObj->GetStringField("deviceId"));

			auto& sequenceArray = trackObj->GetArrayField("sequence");
			for (auto& sequenceItem : sequenceArray)
			{
				auto& sequenceObj = sequenceItem->AsObject();
				auto modifyIntensity = sequenceObj->GetBoolField("is_modify_intensity");
				auto modifyFrequency = sequenceObj->GetBoolField("is_modify_frequency");
				auto modifyDuration = sequenceObj->GetBoolField("is_modify_duration");
				auto durationTarget = sequenceObj->GetNumberField("duration_target");
				auto overlay = sequenceObj->GetBoolField("overlay");
				auto overdrive = sequenceObj->GetBoolField("overdrive");
				auto waveform = sequenceObj->GetNumberField("Waveform");
				auto sampleId = sequenceObj->GetStringField("sample_id_to_play");
				
				// Factor intensity array			
				// Factor frequency array

				if (referencedSamples.Contains(sampleId) == false) {
					UE_LOG(LogStrikerVREditor, Warning, TEXT("Effect references unknown sample %s: %"), *newEffect.EffectID, *sampleId);
					continue;
				}

				FStrikerHapticLibraryTrackSample trackSample;
				trackSample.Sample = referencedSamples[sampleId];
				trackSample.IsModifyIntensity = modifyIntensity; 
				trackSample.IsModifyIntensity = modifyFrequency; 
				trackSample.IsModifyIntensity = modifyDuration; 
				trackSample.DurationTarget = durationTarget; 
				trackSample.Overlay = overlay; 
				trackSample.Overdrive = overdrive; 
				trackSample.Waveform = waveform; 

				newTrack.Sequence.Add(trackSample);
			}
			newEffect.Tracks.Add(newTrack);
		}
		newLibrary->Effects.Add(newEffect);
	}

	return newLibrary;
}

bool UStrikerHapticImportFactory::FactoryCanImport(const FString& Filename) 
{
	const FString Extension = FPaths::GetExtension(Filename);

	if (Extension == TEXT("haptic"))
	{
		return true;
	}
	return false;
}