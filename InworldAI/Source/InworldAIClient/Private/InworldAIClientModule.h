/**
 * Copyright 2022-2024 Theai, Inc. dba Inworld AI
 *
 * Use of this source code is governed by the Inworld.ai Software Development Kit License Agreement
 * that can be found in the LICENSE.md file or at https://www.inworld.ai/sdk-license
 */

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

INWORLDAICLIENT_API DECLARE_LOG_CATEGORY_EXTERN(LogInworldAIClient, Log, All);
INWORLDAICLIENT_API DECLARE_LOG_CATEGORY_EXTERN(LogInworldAINDK, Log, All);

class FInworldAIClientModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
