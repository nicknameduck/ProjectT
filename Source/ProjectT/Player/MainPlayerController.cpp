// Fill out your copyright notice in the Description page of Project Settings.

#include "MainPlayerController.h"
#include "Blueprint/UserWidget.h"

AMainPlayerController::AMainPlayerController()
{
	bShowMouseCursor = true;

	SetGenericTeamId(FGenericTeamId(TeamPlayer));
}

void AMainPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController())
	{
		mMainWidgetClass = LoadClass<UUserWidget>(nullptr, TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/UI/Main/UI_Main.UI_Main_C'"));

		if (IsValid(mMainWidgetClass))
		{
			UUserWidget* MainWidget = CreateWidget<UUserWidget>(this, mMainWidgetClass);
			if (MainWidget)
				MainWidget->AddToViewport();
		}
	}
}

void AMainPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMainPlayerController::SetGenericTeamId(const FGenericTeamId& TeamID)
{
	mTeamID = TeamID;
}

FGenericTeamId AMainPlayerController::GetGenericTeamId() const
{
	return mTeamID;
}
