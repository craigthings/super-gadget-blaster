#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Factories/Factory.h"
//#include "Factories/ImportSettings.h"
//#include "EditorReimportHandler.h"

#include "StrikerHapticImportFactory.generated.h"

UCLASS(hidecategories=Object)
class UStrikerHapticImportFactory : public UFactory
{
	GENERATED_UCLASS_BODY()

	//~ Begin UFactory Interface
	virtual FText GetDisplayName() const override;
	virtual UObject* FactoryCreateText(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, const TCHAR* Type, const TCHAR*& Buffer, const TCHAR* BufferEnd, FFeedbackContext* Warn, bool& bOutOperationCanceled) override;
	//virtual bool DoesSupportClass(UClass * Class) override;
	virtual bool FactoryCanImport(const FString& Filename) override;
	//virtual void CleanUp() override;


//virtual	IImportSettingsParser* GetImportSettingsParser() override;
};
