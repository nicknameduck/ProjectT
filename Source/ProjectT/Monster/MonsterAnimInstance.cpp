// Fill out your copyright notice in the Description page of Project Settings.


#include "MonsterAnimInstance.h"
#include "Monster.h"

#include "BehaviorTree/BehaviorTreeComponent.h"
#include "AIController.h"
#include "UEAssetManager.h"

#include "AbilitySystemComponent.h"

UMonsterAnimInstance::UMonsterAnimInstance()
{
}

void UMonsterAnimInstance::PostInitProperties()
{
	Super::PostInitProperties();
}

void UMonsterAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	FMonsterAnimInfo* MonsterAnimInfo = UUEAssetManager::Get()->FindDataTableRow<FMonsterAnimInfo>(TEXT("DT_MonsterAnimInfo"), mAnimKey);
	if (MonsterAnimInfo)
	{
		mSequenceMap = MonsterAnimInfo->SequenceMap;
		mBlendSpaceMap = MonsterAnimInfo->BlendSpaceMap;
		mMontageMap = MonsterAnimInfo->MontageMap;
	}
}

void UMonsterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
}

void UMonsterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
}

bool UMonsterAnimInstance::PlayMontage(const FName& MontageName, const FName& SectionName)
{
	TObjectPtr<UAnimMontage>* Montage = mMontageMap.Find(MontageName);

	if (!Montage)
		return false;
	
	Montage_Play(Montage->Get());

	if(!SectionName.IsNone())
		Montage_JumpToSection(SectionName);
	
	return true;
}

UAnimMontage* UMonsterAnimInstance::FindAnimMontage(const FName& Name)
{
	TObjectPtr<UAnimMontage>* Montage = mMontageMap.Find(Name);
	return Montage == nullptr ? nullptr : Montage->Get();
}

void UMonsterAnimInstance::AnimNotify_HitEnd()
{
	AMonster* Monster = Cast<AMonster>(TryGetPawnOwner());
	if (Monster)
		Monster->HitEnd();
}

void UMonsterAnimInstance::AnimNotify_DieEnd()
{
	AMonster* Monster = Cast<AMonster>(TryGetPawnOwner());
	if (Monster)
		Monster->Die();
}

void UMonsterAnimInstance::AnimNotify_AttackStart()
{
	AMonster* Monster = Cast<AMonster>(TryGetPawnOwner());

	if (!IsValid(Monster))
		return;

	UAbilitySystemComponent* ASC = Monster->GetAbilitySystemComponent();

	if (!ASC)
		return;

	// GameplayEvent 데이터 준비
	FGameplayEventData EventData;
	EventData.EventTag = FGameplayTag::RequestGameplayTag(FName("Custom.Monster.Event.AttackStart"));
	EventData.Instigator = Monster;

	// 이벤트 트리거
	ASC->HandleGameplayEvent(EventData.EventTag, &EventData);
}

void UMonsterAnimInstance::AnimNotify_AttackEnd()
{
	
}

void UMonsterAnimInstance::AnimNotify_SkillStart()
{
}

void UMonsterAnimInstance::AnimNotify_SkillEnd()
{
}
