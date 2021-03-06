// Fill out your copyright notice in the Description page of Project Settings.


#include "SRMovableMidRangeGameMode.h"

#include "SRPlayerController.h"
#include "SRPlayerState.h"
#include "SRTargetManager.h"

ASRMovableMidRangeGameMode::ASRMovableMidRangeGameMode()
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/BP_PlayerCharacter.BP_PlayerCharacter_C"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;
	PlayerControllerClass = ASRPlayerController::StaticClass();
	PlayerStateClass = ASRPlayerState::StaticClass();

	mGameModeType = EGameModeType::Movable_MidRange;
}

void ASRMovableMidRangeGameMode::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void ASRMovableMidRangeGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	playerController = Cast<ASRPlayerController>(NewPlayer);
	FTimerHandle initTarget;
	GetWorld()->GetTimerManager().SetTimer(initTarget, this, &ASRMovableMidRangeGameMode::initTargetManagerMode, 0.5f, false);
}

void ASRMovableMidRangeGameMode::initTargetManagerMode()
{
	playerController->GetTargetManager()->SetMovableTargetMode(mGameModeType);
}
