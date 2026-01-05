// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseAbility.h"
//#include "Modifier/AbilityModifier.h"
#include "AttributeSet/BaseSet.h"
#include "AttributeSet/Common/ResourceSet.h"
#include "AbilitySet/AbilityInitData.h"
#include "Ability/SkillObjects/SkillInfoObject.h"

#include "UEAssetManager.h"

bool FAbilityCost::CanPay(const UResourceSet* SourceAttr)
{
	switch (Type)
	{
	case ECostType::MP:
		return Cost <= SourceAttr->GetMP();
	case ECostType::HP:
		return Cost <= SourceAttr->GetHP();
	default:
		{
			PT_LOG_ALERT(false, Warning, TEXT("ECostType이 존재하지 않는 타입입니다. %d"), (int32)(Type));
			return false;
		}
	}
}

void UBaseAbility::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec);

	USkillInfoObject* SkillInfoObject = Cast<USkillInfoObject>(Spec.SourceObject);
	const FSkillInfo* SkillInfo = SkillInfoObject->GetSkillInfo();
	mSkillInfo = *SkillInfo;
	if (SkillInfo)
		ActivationBlockedTags.AddTag(mSkillInfo.CoolDownTag);
	
}

void UBaseAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	mAbilityActive = true;

	if (!ActorInfo || !ActorInfo->AvatarActor.IsValid())
	{
		mAbilityActive = false;
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	AActor* SourceActor = GetAvatarActorFromActorInfo();
	UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo();

	const UResourceSet* SourceAttr = SourceASC->GetSet<UResourceSet>();
	if (!SourceAttr)
	{
		mAbilityActive = false;
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	// Cost 체크
	/*for (auto& Cost : mCostArray)
	{
		if (!Cost.CanPay(SourceAttr))
		{
			mAbilityActive = false;
			EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
			return;
		}
	}*/
	// 쿨다운 체크
	if (mSkillInfo.CoolDown > 0.f)
	{
		float CoolDown = mSkillInfo.CoolDown;

		// .스킬 쿨감 20% 감소일 경우
		//CoolDown *= (1.f - 0.2f);

		if (!mSkillInfo.CoolDownEffect)
		{
			//[todo:psh] Info의 RowName을 가져오는 방법을 찾아보자
			PT_LOG_ALERT(false, Warning, TEXT("CoolDown이 존재하는데 CoolDownEffect가 존재하지않습니다 데이터를 확인해주세요."));
			return;
		}

		//FGameplayEffectContextHandle Context = SourceASC->MakeEffectContext();
		//Context.AddInstigator(SourceASC->GetAvatarActor(), SourceASC->GetAvatarActor());

		//// 또는 완전히 비워도 됨 (Ability와 연결 끊기)
		//Context.SetAbility(nullptr);

		//FGameplayEffectSpecHandle	CoolDownSpec = SourceASC->MakeOutgoingSpec(mCoolDownEffect, 1.f, Context);
		FGameplayEffectSpecHandle	CoolDownSpec = MakeOutgoingGameplayEffectSpec(mSkillInfo.CoolDownEffect, 1.f);

		CoolDownSpec.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(TEXT("Custom.Effect.CoolDown")), CoolDown);

		// 동적으로 추가되는 태그를 저장하는 컨테이너이다.
		CoolDownSpec.Data->DynamicGrantedTags.AddTag(mSkillInfo.CoolDownTag);
		SourceASC->ApplyGameplayEffectSpecToSelf(*CoolDownSpec.Data);
	}

	/*for (const FAbilityCost& Cost : mCostArray)
	{
		if (Cost.EffectClass)
		{
			FGameplayEffectSpecHandle Spec = MakeOutgoingGameplayEffectSpec(Cost.EffectClass, GetAbilityLevel());
			Spec.Data->SetSetByCallerMagnitude(Cost.CostTag, -Cost.Cost);
			SourceASC->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
		}
	}*/
}

void UBaseAbility::ApplySkillEffectsToTargetData(const FGameplayAbilityTargetDataHandle& TargetData)
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		return;
	}

	for (auto& GEData : mSkillInfo.GameplayEffects)
	{
		if (!GEData.EffectClass)
			continue;

		FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
		ContextHandle.AddSourceObject(this);

		AActor* Avatar = GetAvatarActorFromActorInfo();
		if (Avatar)
			ContextHandle.AddInstigator(Avatar, Avatar->GetInstigatorController());

		FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(GEData.EffectClass, 1.f, ContextHandle);

		if (!SpecHandle.IsValid())
			continue;

		for (auto& Param : GEData.SetByCallerParams)
			SpecHandle.Data->SetSetByCallerMagnitude(Param.Key, Param.Value);

		ApplyGameplayEffectSpecToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, SpecHandle, TargetData);
	}
}

//void UBaseAbility::ApplyModifier(UAbilityModifier* Modifier)
//{
//	if (Modifier)
//	{
//		Modifier->ApplyModifier(this);
//		mAppliedModifiers.Add(Modifier->StaticClass());
//	}
//}
