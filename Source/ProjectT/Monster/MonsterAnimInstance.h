// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameCore.h"
#include "GameInfo.h"
#include "Animation/AnimInstance.h"
#include "MonsterAnimInstance.generated.h"

UCLASS()
class PROJECTT_API UMonsterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UMonsterAnimInstance();	

public:
	virtual void PostInitProperties() override;
	virtual void NativeBeginPlay() override;
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

public:
	bool PlayMontage(const FName& MontageName, const FName& SectionName = NAME_None);

	UAnimMontage* FindAnimMontage(const FName& Name);

public:
	UFUNCTION()
	void AnimNotify_HitEnd();

	UFUNCTION()
	void AnimNotify_DieEnd();

	UFUNCTION()
	void AnimNotify_AttackStart();

	UFUNCTION()
	void AnimNotify_AttackEnd();

	UFUNCTION()
	void AnimNotify_SkillStart();

	UFUNCTION()
	void AnimNotify_SkillEnd();
public:
	//몬스터 애니메이션 변경시 SetAnimType을 바로 호출하지말고 Monster의 SetMonsterAnimType을 호출하자.
	void SetAnimType(EMonsterAnim Type)
	{
		mPrevAnimType = mAnimType;
		mAnimType = Type;
	}

	const EMonsterAnim GetCurrentAnimType() const
	{
		return mAnimType;
	}

public:
	UPROPERTY(EditAnywhere)
	FName									mAnimKey;

	UPROPERTY(BlueprintReadOnly)
	EMonsterAnim							mAnimType = EMonsterAnim::Idle;

	UPROPERTY()
	EMonsterAnim							mPrevAnimType = EMonsterAnim::Idle;

	UPROPERTY(BlueprintReadOnly)
	TMap<FName, TObjectPtr<UAnimSequence>>	mSequenceMap;

	UPROPERTY(BlueprintReadOnly)
	TMap<FName, TObjectPtr<UBlendSpace>>	mBlendSpaceMap;

	UPROPERTY(BlueprintReadOnly)
	TMap<FName, TObjectPtr<UAnimMontage>>	mMontageMap;
};
