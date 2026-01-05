// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PlayerCharacter.h"
#include "Kallari.generated.h"

UCLASS()
class PROJECTT_API AKallari : public APlayerCharacter
{
	GENERATED_BODY()
	

public:
	AKallari();

protected:
	virtual void HandleAttackInput() override;
};
