// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Monster.h"
#include "Chest.generated.h"

UCLASS()
class PROJECTT_API AChest : public AMonster
{
	GENERATED_BODY()

public:
	AChest();
		
protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
};
