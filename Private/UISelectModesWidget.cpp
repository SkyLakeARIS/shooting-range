﻿// Fill out your copyright notice in the Description page of Project Settings.
#include "UISelectModesWidget.h"
#include "SRGameMode.h"
#include "SRTargetManager.h"
#include "Components/Button.h"
#include "Engine/AssetManager.h"

void UUISelectModesWidget::NativeConstruct()
{
	Super::NativeConstruct();
	mBtnAR = Cast<UButton>(GetWidgetFromName(TEXT("Btn_AR")));
	mBtnHG = Cast<UButton>(GetWidgetFromName(TEXT("Btn_HG")));
	mBtnSR = Cast<UButton>(GetWidgetFromName(TEXT("Btn_SR")));
	mBtn1X = Cast<UButton>(GetWidgetFromName(TEXT("Btn_1X")));
	mBtn2Dot5X = Cast<UButton>(GetWidgetFromName(TEXT("Btn_2Dot5X")));
	mBtn6X = Cast<UButton>(GetWidgetFromName(TEXT("Btn_6X")));
	mBtnBattlefield = Cast<UButton>(GetWidgetFromName(TEXT("Btn_Battlefield")));
	mBtnRainbowSix = Cast<UButton>(GetWidgetFromName(TEXT("Btn_RainbowSix")));
	mBtnTarkov = Cast<UButton>(GetWidgetFromName(TEXT("Btn_Tarkov")));
	mBtnStart = Cast<UButton>(GetWidgetFromName(TEXT("Btn_Start")));
	mBtnPlateType = Cast<UButton>(GetWidgetFromName(TEXT("Btn_PlateType")));
	mBtnCharacterType = Cast<UButton>(GetWidgetFromName(TEXT("Btn_CharacterType")));

	mBtnAR->OnClicked.AddDynamic(this, &UUISelectModesWidget::clickedAR);
	mBtnHG->OnClicked.AddDynamic(this, &UUISelectModesWidget::clickedHG);
	mBtnSR->OnClicked.AddDynamic(this, &UUISelectModesWidget::clickedSR);
	mBtn1X->OnClicked.AddDynamic(this, &UUISelectModesWidget::clicked1X);
	mBtn2Dot5X->OnClicked.AddDynamic(this, &UUISelectModesWidget::clicked2Dot5X);
	mBtn6X->OnClicked.AddDynamic(this, &UUISelectModesWidget::clicked6X);
	mBtnBattlefield->OnClicked.AddDynamic(this, &UUISelectModesWidget::clickedBattlefield);
	mBtnRainbowSix->OnClicked.AddDynamic(this, &UUISelectModesWidget::clickedRainbowSix);
	mBtnTarkov->OnClicked.AddDynamic(this, &UUISelectModesWidget::clickedTarkov);
	mBtnStart->OnClicked.AddDynamic(this, &UUISelectModesWidget::clickedStart);
	mBtnPlateType->OnClicked.AddDynamic(this, &UUISelectModesWidget::clickedPlateType);
	mBtnCharacterType->OnClicked.AddDynamic(this, &UUISelectModesWidget::clickedCharacterType);

	mSelectionFlag = 0;
}

void UUISelectModesWidget::NativeDestruct()
{
	Super::NativeDestruct();
}


void UUISelectModesWidget::clickedAR()
{
	mBtnAR->SetIsEnabled(false);
	mBtnHG->SetIsEnabled(true);
	mBtnSR->SetIsEnabled(true);
	mSelectedModes.weapon = eWeaponType::AR;

	// AR의 경우에는 모든 조준경을 장착할 수 있음.
	mBtn2Dot5X->SetIsEnabled(true);
	mBtn6X->SetIsEnabled(true);

	mSelectionFlag |= WEAPON_SELECT_BIT;
}

void UUISelectModesWidget::clickedHG()
{
	mBtnAR->SetIsEnabled(true);
	mBtnHG->SetIsEnabled(false);
	mBtnSR->SetIsEnabled(true);

	// HG의 경우 1배율 조준경만 장착 가능하도록 제한
	mBtn1X->SetIsEnabled(false);
	mBtn2Dot5X->SetIsEnabled(true);
	mBtn6X->SetIsEnabled(true);

	mBtn2Dot5X->SetIsEnabled(false);
	mBtn6X->SetIsEnabled(false);

	mSelectedModes.weapon = eWeaponType::HG;
	mSelectedModes.scope = eScopeType::Scope1X;

	mSelectionFlag |= WEAPON_SELECT_BIT;
	mSelectionFlag |= OPTIC_SELECT_BIT;
}

void UUISelectModesWidget::clickedSR()
{
	mBtnAR->SetIsEnabled(true);
	mBtnHG->SetIsEnabled(true);
	mBtnSR->SetIsEnabled(false);
	mSelectedModes.weapon = eWeaponType::SR;

	// SR의 경우에는 모든 조준경을 장착할 수 있음.
	mBtn2Dot5X->SetIsEnabled(true);
	mBtn6X->SetIsEnabled(true);

	mSelectionFlag |= WEAPON_SELECT_BIT;
}

void UUISelectModesWidget::clicked1X()
{
	mBtn1X->SetIsEnabled(false);
	mBtn2Dot5X->SetIsEnabled(true);
	mBtn6X->SetIsEnabled(true);
	mSelectedModes.scope = eScopeType::Scope1X;

	mSelectionFlag |= OPTIC_SELECT_BIT;
}

void UUISelectModesWidget::clicked2Dot5X()
{
	mBtn1X->SetIsEnabled(true);
	mBtn2Dot5X->SetIsEnabled(false);
	mBtn6X->SetIsEnabled(true);
	mSelectedModes.scope = eScopeType::Scope2dot5X;

	mSelectionFlag |= OPTIC_SELECT_BIT;
}

void UUISelectModesWidget::clicked6X()
{
	mBtn1X->SetIsEnabled(true);
	mBtn2Dot5X->SetIsEnabled(true);
	mBtn6X->SetIsEnabled(false);
	mSelectedModes.scope = eScopeType::Scope6X;

	mSelectionFlag |= OPTIC_SELECT_BIT;
}

void UUISelectModesWidget::clickedBattlefield()
{
	mBtnBattlefield->SetIsEnabled(false);
	mBtnRainbowSix->SetIsEnabled(true);
	mBtnTarkov->SetIsEnabled(true);
	mSelectedModes.game = eGameType::Battlefield;

	mSelectionFlag |= GAMEMODE_SELECT_BIT;
}

void UUISelectModesWidget::clickedRainbowSix()
{
	mBtnBattlefield->SetIsEnabled(true);
	mBtnRainbowSix->SetIsEnabled(false);
	mBtnTarkov->SetIsEnabled(true);
	mSelectedModes.game = eGameType::RainbowSix;

	mSelectionFlag |= GAMEMODE_SELECT_BIT;
}

void UUISelectModesWidget::clickedTarkov()
{
	mBtnBattlefield->SetIsEnabled(true);
	mBtnRainbowSix->SetIsEnabled(true);
	mBtnTarkov->SetIsEnabled(false);
	mSelectedModes.game = eGameType::Tarkov;

	mSelectionFlag |= GAMEMODE_SELECT_BIT;
}

void UUISelectModesWidget::clickedPlateType()
{
	mBtnPlateType->SetIsEnabled(false);
	mBtnCharacterType->SetIsEnabled(true);
	mbIsCharacterType = false;

	mSelectionFlag |= TARGET_SELECT_BIT;
}

void UUISelectModesWidget::clickedCharacterType()
{
	mBtnPlateType->SetIsEnabled(true);
	mBtnCharacterType->SetIsEnabled(false);
	mbIsCharacterType = true;

	mSelectionFlag |= TARGET_SELECT_BIT;
}

void UUISelectModesWidget::clickedStart()
{

	if(mSelectionFlag == SELECT_ALL_FLAG)
	{
		ASRGameMode* gameMode = Cast<ASRGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
		// 선택한 정보들을 GameMode로 전달합니다.
		gameMode->SetGameModeData(mSelectedModes, mbIsCharacterType);
		gameMode->SettingGameAndStartGame();

		RemoveFromParent();
	}
	else
	{
		mBtnStart->SetColorAndOpacity(FLinearColor::Red);
	}
}
