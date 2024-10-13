#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(LogStrikerVREditor, Log, All);

//////////////////////////////////////////////////////////////////////////
// FStrikerVREditorModule

class FStrikerVREditorModule : public FDefaultModuleImpl
{
public:
	virtual void StartupModule() override
	{
	}

	virtual void ShutdownModule() override
	{
	}
};
