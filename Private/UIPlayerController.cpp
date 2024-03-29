﻿// Fill out your copyright notice in the Description page of Project Settings.
#include "UIPlayerController.h"
#include "Blueprint/UserWidget.h"

void AUIPlayerController::BeginPlay()
{
	Super::BeginPlay();

	UIWidgetInstance = CreateWidget<UUserWidget>(this, UIWidgetClass);

	UIWidgetInstance->AddToViewport();

	FInputModeUIOnly mode;
	mode.SetWidgetToFocus(UIWidgetInstance->GetCachedWidget());
	SetInputMode(mode);
	bShowMouseCursor = true;
}
