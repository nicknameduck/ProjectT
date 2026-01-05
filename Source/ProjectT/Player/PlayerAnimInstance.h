// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameCore.h"
#include "Animation/AnimInstance.h"
#include "PlayerAnimInstance.generated.h"

UCLASS()
class PROJECTT_API UPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UPlayerAnimInstance();

public:
	virtual void PostInitProperties() override;
	virtual void NativeBeginPlay() override;
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

public:
	void SetDir(float Dir);
	UAnimMontage* FindAnimMontage(const FName& Name);
	bool IsPlayingMontage(const FName& Name);

public:
	UFUNCTION()
	void AnimNotify_AttackStart();

	UFUNCTION()
	void AnimNotify_AttackEnd();

	UFUNCTION()
	void AnimNotify_ReloadStart();

	UFUNCTION()
	void AnimNotify_ReloadEnd();

	UFUNCTION()
	void AnimNotify_LoopStart();

	UFUNCTION()
	void AnimNotify_SkillStart();

	UFUNCTION()
	void AnimNotify_SkillEnd();

public:
	UPROPERTY(EditAnywhere)
	FName									mAnimKey;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TMap<FName, TObjectPtr<UAnimSequence>>	mSequenceMap;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TMap<FName, TObjectPtr<UBlendSpace>>	mBlendSpaceMap;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TMap<FName, TObjectPtr<UAnimMontage>>	mMontageMap;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TArray<FName>							mAttackSectionArray;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float									mMoveSpeed = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float									mDir = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float									mLastDir = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float									mAimYaw = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float									mTurnDegree = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool									mAttacking = false;
};
