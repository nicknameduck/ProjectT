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
	TObjectPtr<UInputMappingContext>	mContext;
	TObjectPtr<UInputAction>			mMove;
	TObjectPtr<UInputAction>			mRotation;
	TObjectPtr<UInputAction>			mAttack;
	TObjectPtr<UInputAction>			mSkill_Q;
	TObjectPtr<UInputAction>			mSkill_R;
	TObjectPtr<UInputAction>			mSkill_E;
	TObjectPtr<UInputAction>			mSkill_RMB;
};
