// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once
#include "Version.h"
#include "GameFramework/GameModeBase.h"
#include "VersionGameMode.generated.h"

UCLASS(minimalapi)
class AVersionGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AVersionGameMode();

	UFUNCTION(BlueprintCallable, Category=TUTO)
	void SelectButton1();

	UFUNCTION(BlueprintCallable, Category=TUTO)
	void SelectButton2();
};



