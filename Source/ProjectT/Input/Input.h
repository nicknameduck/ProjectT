// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameCore.h"
#include "UObject/NoExportTypes.h"
#include "Input.generated.h"

class UInputAction;
class UInputMappingContext;

UCLASS()
class PROJECTT_API UGameInput : public UObject
{
	GENERATED_BODY()
	
public:
	UGameInput();

public:
	UPROPERTY()
	TObjectPtr<UInputMappingContext>	mContext;
	UPROPERTY()
	TObjectPtr<UInputAction>			mMove;
	UPROPERTY()
	TObjectPtr<UInputAction>			mRotation;
	UPROPERTY()
	TObjectPtr<UInputAction>			mAttack;
	UPROPERTY()
	TObjectPtr<UInputAction>			mSkill_Q;
	UPROPERTY()
	TObjectPtr<UInputAction>			mSkill_R;
	UPROPERTY()
	TObjectPtr<UInputAction>			mSkill_E;
	UPROPERTY()
	TObjectPtr<UInputAction>			mSkill_RMB;
	UPROPERTY()
	TObjectPtr<UInputAction>			mTest;
};
