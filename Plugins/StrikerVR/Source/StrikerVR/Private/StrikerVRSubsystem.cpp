#include "StrikerVRSubsystem.h"

#include "CoreMinimal.h"
#include "Serialization/Csv/CsvParser.h"
#include "Serialization/JsonWriter.h"
#include "Serialization/JsonSerializer.h"
#include "Dom/JsonObject.h"
#include "Policies/CondensedJsonPrintPolicy.h"

#include "FPKFactory.h"
#include "StrikerVR.h"

void UStrikerVRSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	UE_LOG(LogStrikerVR, Log, TEXT("UStrikerVRSubsystem::Initialize"));
	const FString ServerURL = TEXT("ws://127.0.0.1:5060/app"); // Your server URL. You can use ws, wss or wss+insecure.
	const FString ServerProtocol = TEXT("ws");              // The WebServer protocol you want to use.

	TMap<FString, FString> UpgradeHeaders;
	UpgradeHeaders.Add("Host", "127.0.0.1:5060");

	// Bug: https://forums.unrealengine.com/t/websockets-module-not-loaded-properly-in-packaged-builds-only-other-modules-ok/137684/14?u=lmg_james
	FModuleManager::Get().LoadModuleChecked("StrikerWS");
	Socket = FWebSocketsModule::Get().CreateWebSocket(ServerURL, ServerProtocol, UpgradeHeaders);


	UE_LOG(LogStrikerVR, Log, TEXT("Initiating connection to StrikerLink using WebSocket %s"), *ServerURL);

	// We bind all available events
	Socket->OnConnected().AddLambda([this]() -> void {
	    UE_LOG(LogStrikerVR, Log, TEXT("Websocket connection established"));
		OnStrikerConnected.Broadcast();
	});

	Socket->OnRawMessage().AddLambda([this](const void* Data, size_t Length, size_t BytesLeft, bool IsBinary) { OnRawMessage((const uint8*)Data, Length, BytesLeft, IsBinary); });

	Socket->OnConnectionError().AddLambda([](const FString& Error) -> void {
		UE_LOG(LogStrikerVR, Log, TEXT("Connection to websocket failed %s"), *Error);
		});

	Socket->OnClosed().AddLambda([](int32 /* StatusCode */, const FString& /* Reason */, bool /* bWasClean */) -> void {
		UE_LOG(LogStrikerVR, Log, TEXT("Websocket closed"));
		});

	// And we finally connect to the server. 
	Socket->Connect();

	SetStrikerSubsystem(this);
}

void UStrikerVRSubsystem::Deinitialize()
{
	SetStrikerSubsystem(nullptr);

	Socket->Close();
	OnStrikerConnected.Clear();
}

static const FString SampleDeviceNames[] = {
	"hammerTop",
	"fosters",
	"fosterFront",
	"fosterBack",
};
static_assert((int)UStrikerHapticSampleDevice::COUNT == UE_ARRAY_COUNT(SampleDeviceNames));

static const FString SampleCommandNames[] = {
	"tick",
	"pulse",
	"vibrate",
	"pause",
};
static_assert((int)UStrikerHapticEffectCommand::COUNT == UE_ARRAY_COUNT(SampleCommandNames));


bool SerializeLibrary(UStrikerVrHapticLibrary*library, FString* json)
{
	TArray<TSharedPtr<FJsonValue>> effectsJson;
	TMap<FString, UStrikerHapticEffectSample*> referencedSamples;

	for (auto& effect : library->Effects)
	{
		TSharedPtr<FJsonObject> effectJson = MakeShared<FJsonObject>();
		effectJson->SetStringField("effect_id", effect.EffectID);

 		TArray<TSharedPtr<FJsonValue>> tracksJson;
 
 		for(auto& track : effect.Tracks)
 		{
 			TSharedPtr<FJsonObject> trackJson = MakeShared<FJsonObject>();
 			trackJson->SetStringField("deviceId", SampleDeviceNames[(int)track.DeviceId]);			
 
 			TArray<TSharedPtr<FJsonValue>> samplesJson;
 			for(auto& sample : track.Sequence)
 			{
 				TSharedPtr<FJsonObject> sampleJson = MakeShared<FJsonObject>();;
 				
				sampleJson->SetBoolField("is_modify_intensity", sample.IsModifyIntensity);
				sampleJson->SetBoolField("is_modify_frequency", sample.IsModifyFrequency);
				sampleJson->SetBoolField("is_modify_duration", sample.IsModifyDuration);
				sampleJson->SetNumberField("duration_target", sample.DurationTarget);

				sampleJson->SetBoolField("overlay", sample.Overlay);
				sampleJson->SetBoolField("overdrive", sample.Overdrive);
				sampleJson->SetNumberField("Waveform", sample.Waveform);

				// Use the ID field of the referenced sample object
				sampleJson->SetStringField("sample_id_to_play", sample.Sample->SampleID);
 
				referencedSamples.Add(sample.Sample->SampleID, sample.Sample);
 				
				// TODO:
 				// factor_intensity -> []
 				// factor_frequency -> []
 
 				samplesJson.Add(MakeShared<FJsonValueObject>(sampleJson));
 			}
 			trackJson->SetArrayField("sequence", samplesJson);
 
 			tracksJson.Add(MakeShared<FJsonValueObject>(trackJson));
 		}
 		effectJson->SetArrayField("tracks", tracksJson);
		
		effectsJson.Add(MakeShared<FJsonValueObject>(effectJson));
	}

	// Sample palette
	TArray<TSharedPtr<FJsonValue>> samplesPaletteJson;

	for (auto& sampleKV : referencedSamples)
	{
		auto* sample = sampleKV.Value;
		TSharedPtr<FJsonObject> sampleJson = MakeShared<FJsonObject>();

		sampleJson->SetStringField("sample_id", sample->SampleID);

		TArray<TSharedPtr<FJsonValue>> sequenceJson;
		for (auto& command : sample->Sequence)
		{
			TSharedPtr<FJsonObject> commandJson = MakeShared<FJsonObject>();
			commandJson->SetStringField("version", command.Version);
			commandJson->SetStringField("command", SampleCommandNames[(int)command.Command]);
			commandJson->SetNumberField("frequency", command.Frequency);
			commandJson->SetNumberField("frequency_time", command.FrequencyTime);
			commandJson->SetNumberField("duration_base", command.DurationBase);			
			commandJson->SetNumberField("intensity", command.Intensity);
			commandJson->SetNumberField("intensity_time", command.IntensityTime);
			commandJson->SetNumberField("waveform", command.Waveform);
			commandJson->SetBoolField("overlay", command.Overlay);
			commandJson->SetBoolField("overDrive", command.Overdrive);		

			sequenceJson.Add(MakeShared<FJsonValueObject>(commandJson));
		}
		sampleJson->SetArrayField("sequence", sequenceJson);

		samplesPaletteJson.Add(MakeShared<FJsonValueObject>(sampleJson));
	}


	TSharedRef<FJsonObject> LibraryRoot = MakeShared<FJsonObject>();
	LibraryRoot->SetArrayField("effects", effectsJson);
	LibraryRoot->SetArrayField("samples_palette", samplesPaletteJson);	

	TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter = TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(json, /*Indent=*/0);

	return FJsonSerializer::Serialize(LibraryRoot, JsonWriter);
}

void UStrikerVRSubsystem::SendHapticLibrary(UStrikerVrHapticLibrary* library)
{
	if (library == nullptr)
	{
		UE_LOG(LogStrikerVR, Warning, TEXT("SendHapticLibrary: Library supplied is null"));
		return;
	}
		
	FString sampleLibJson;
	SerializeLibrary(library, &sampleLibJson);
		
	UE_LOG(LogStrikerVR, Log, TEXT("Sending Haptic Library: %s"), *library->LibraryIdentifier);
	auto* packet = FOPKHapticLibrary::Create(library->LibraryIdentifier, sampleLibJson);
	SendBinaryPacket(packet);
}

void UStrikerVRSubsystem::SendBinaryPacket(FOPKBase* packet)
{
	if (packet == nullptr) {
		UE_LOG(LogStrikerVR, Error, TEXT("SendPacket: Packet supplied is null"));
		return;
	}

	TArray<uint8_t> data;
	FMemoryWriter writer(data);
	packet->Serialize(writer);
	Socket->Send(data.GetData(), data.Num(), true);

	delete packet;
}

void UStrikerVRSubsystem::SendStringPacket(FOPKBase* packet)
{
	if (packet == nullptr) {
		UE_LOG(LogStrikerVR, Error, TEXT("SendPacket: Packet supplied is null"));
		return;
	}
	if (packet->HasValidDeviceIndex() == false) {
		UE_LOG(LogStrikerVR, Warning, TEXT("SendPacket: Device index invalid"));
		return;
	}

	TArray<uint8_t> data;
	FMemoryWriter writer(data);
	packet->Serialize(writer);

#if WITH_EDITOR
 	FString dataStr(data.Num(), (char*)data.GetData());
 	UE_LOG(LogStrikerVR, VeryVerbose, TEXT("Send String Packet: %s"), *dataStr);
#endif

	Socket->Send(data.GetData(), data.Num(), false);

	delete packet;
}

void UStrikerVRSubsystem::OnRawMessage(const uint8* Data, size_t Length, size_t BytesLeft, bool IsBinary)
{
	UE_LOG(LogStrikerVR, VeryVerbose, TEXT("OnRawMessage"));

	FPKBase* packet = nullptr;

	if (IsBinary)
	{
		RawRecieveBuffer.Append(Data, Length);

		if (BytesLeft == 0)
		{
			UE_LOG(LogStrikerVR, VeryVerbose, TEXT("Binary Message Received: %d bytes"), RawRecieveBuffer.Num());

			packet = ParseBinaryFrame(RawRecieveBuffer);
			RawRecieveBuffer.Empty();

			if (RawRecieveBuffer.Num() != 0)
				UE_LOG(LogStrikerVR, Warning, TEXT("Binary Message bytes left over after parsing: %d bytes"), RawRecieveBuffer.Num());
		}
	}
	else 
	{
		FUTF8ToTCHAR Convert((const ANSICHAR*)Data, Length);
		RecieveBuffer.Append(Convert.Get(), Convert.Length());

		if (BytesLeft == 0)
		{
			UE_LOG(LogStrikerVR, VeryVerbose, TEXT("Message: %s"), *RecieveBuffer);

			if (RecieveBuffer.StartsWith("json:")) {
				UE_LOG(LogStrikerVR, Verbose, TEXT("JSON data received"));

				RecieveBuffer.Empty();
			}
			else {
				UE_LOG(LogStrikerVR, Verbose, TEXT("CSV data received"));

				packet = ParseCSVFrame(MoveTemp(RecieveBuffer)); //MoveTemp?				
			}

			if (RecieveBuffer.Len() != 0)
				UE_LOG(LogStrikerVR, Warning, TEXT("String Message bytes left over after parsing: %d bytes"), RecieveBuffer.Len());
		}
	}

	if (packet != nullptr)
	{
		// Do something with this packet
		UE_LOG(LogStrikerVR, VeryVerbose, TEXT("Process packet"));
		//packet->Log();
		packet->ApplyToState(currentState);
		delete packet;
	}
}

FPKBase* UStrikerVRSubsystem::ParseCSVFrame(FString CSVString)
{
	UE_LOG(LogStrikerVR, VeryVerbose, TEXT("Parse CSV frame: %s"), *CSVString); 

	FCsvParser Parser(MoveTemp(CSVString));
	const auto& Rows = Parser.GetRows();

	if (Rows.Num() == 0)
	{
		UE_LOG(LogStrikerVR, Warning, TEXT("Invalid CSV data receieved"));
		return nullptr;
	}

	const auto& Row = Rows[0];

	int32 PacketType = FCString::Atoi(Row[0]);
	FPKBase* packet = FPKFactory::PacketFromType(PacketType);

	if (packet == nullptr)
		return nullptr;

	uint32_t Col = 0;

	if (packet->ReadFromCSV(Row, Col) == false)
	{
		UE_LOG(LogStrikerVR, Verbose, TEXT("Unable to parse packet"));
		delete packet;
		return nullptr;
	}

	return packet;
}

FPKBase* UStrikerVRSubsystem::ParseBinaryFrame(TArray<uint8> const& RawFrame)
{
	UE_LOG(LogStrikerVR, VeryVerbose, TEXT("Parse binary frame"));

	int32 PacketType = *(int32*)&RawFrame[0];
	FPKBase* packet = FPKFactory::PacketFromType(PacketType);

	if (packet == nullptr)
		return nullptr;

	size_t Offset = 0;
	if (packet->ReadFromArray(RawFrame, Offset) == false)
	{
		UE_LOG(LogStrikerVR, Warning, TEXT("Unable to parse packet"));
		delete packet;
		return nullptr;
	}

	return packet;
}

