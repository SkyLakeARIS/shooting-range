// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Version.h"
#include "GameModeData.h"
#include "GameFramework/GameModeBase.h"
#include "SRShortRangeGameMode.generated.h"

/*
 * ShortRange 고정형 맵을 선택할 때의 게임모드입니다.
 */
UCLASS()
class VERSION_API ASRShortRangeGameMode : public AGameModeBase
{
	GENERATED_BODY()
public:
	ASRShortRangeGameMode();
	
	virtual void PostInitializeComponents() override;

	virtual void PostLogin(APlayerController* NewPlayer) override;

private:

	void initTargetManagerMode();

private:
	EGameModeType mGameModeType;
	UPROPERTY()
	class ASRPlayerController* playerController;
};
