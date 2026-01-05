// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameCore.h"
#include "GameInfo.h"
#include "Abilities/GameplayAbility.h"
#include "BaseAbility.generated.h"

/**
 * 
 */

class UResourceSet;
//class UAbilityModifier;

UENUM(BlueprintType)
enum class ECostType : uint8
{
	HP,
	MP,
	Count
};

USTRUCT(BlueprintType)
struct FAbilityCost
{
	GENERATED_BODY()

public:
	bool CanPay(const UResourceSet* SourceAttr);

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> EffectClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
	FGameplayTag	CostTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
	float			Cost;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
	ECostType		Type;
};

UCLASS()
class PROJECTT_API UBaseAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	virtual void OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData);

public:
	const TArray<FAbilityTriggerData>& GetTriggerTagDataList() const
	{
		return AbilityTriggers;
	}

	//void ApplyModifier(UAbilityModifier* Modifier);

	void ApplySkillEffectsToTargetData(const FGameplayAbilityTargetDataHandle& TargetData);

protected:
	bool mAbilityActive = false;
	FSkillInfo	mSkillInfo;
};
