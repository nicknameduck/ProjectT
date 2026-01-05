// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameCore.h"
#include "GameFramework/PlayerController.h"
#include "GenericTeamAgentInterface.h"

#include "MainPlayerController.generated.h"

class UUserWidget;

UCLASS()
class PROJECTT_API AMainPlayerController : public APlayerController, public IGenericTeamAgentInterface
{
	GENERATED_BODY()
	
public:
	AMainPlayerController();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

public:
	virtual void SetGenericTeamId(const FGenericTeamId& TeamID) override;
	virtual FGenericTeamId GetGenericTeamId() const override;

protected:
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget>	mMainWidgetClass;

	FGenericTeamId	mTeamID;

};
