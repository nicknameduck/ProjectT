// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAnimInstance.h"
#include "UEAssetManager.h"
#include "GameInfo.h"
#include "PlayerCharacter.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"

UPlayerAnimInstance::UPlayerAnimInstance()
{	
	
}

void UPlayerAnimInstance::PostInitProperties()
{
	Super::PostInitProperties();
}

void UPlayerAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	FPlayerAnimInfo* PlayerAnimInfo = UUEAssetManager::Get()->FindDataTableRow<FPlayerAnimInfo>(TEXT("DT_PlayerAnimInfo"), mAnimKey);
	if (PlayerAnimInfo)
	{
		mSequenceMap = PlayerAnimInfo->SequenceMap;
		mBlendSpaceMap = PlayerAnimInfo->BlendSpaceMap;
		mMontageMap = PlayerAnimInfo->MontageMap;
		mAttackSectionArray = PlayerAnimInfo->AttackSectionArray;
	}
}

void UPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
}

void UPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	APlayerCharacter* PlayerCharacter =
		Cast<APlayerCharacter>(TryGetPawnOwner());

	if (IsValid(PlayerCharacter))
	{
		UCharacterMovementComponent* Movement =
			PlayerCharacter->GetCharacterMovement();

		if (Movement)
		{
			mMoveSpeed = Movement->Velocity.Size();
		}
	}
}

void UPlayerAnimInstance::SetDir(float Dir)
{
	mDir = Dir;
}

UAnimMontage* UPlayerAnimInstance::FindAnimMontage(const FName& Name)
{
	TObjectPtr<UAnimMontage>* Montage = mMontageMap.Find(Name);
	return Montage == nullptr ? nullptr : Montage->Get();
}

bool UPlayerAnimInstance::IsPlayingMontage(const FName& Name)
{
	return Montage_IsPlaying(FindAnimMontage(TEXT("Attack")));
}

void UPlayerAnimInstance::AnimNotify_AttackStart()
{
	mAttacking = true;

	APlayerCharacter* PlayerCharacter =	Cast<APlayerCharacter>(TryGetPawnOwner());

	if (!IsValid(PlayerCharacter))
		return;

	/*PlayerCharacter->StatAttack();*/

	UAbilitySystemComponent* ASC = PlayerCharacter->GetAbilitySystemComponent();

	if (!ASC)
		return;

	// GameplayEvent 데이터 준비
	FGameplayEventData EventData;
	EventData.EventTag = FGameplayTag::RequestGameplayTag(FName("Custom.Player.Event.AttackStart"));
	EventData.Instigator = PlayerCharacter;

	// 이벤트 트리거
	ASC->HandleGameplayEvent(EventData.EventTag, &EventData);
}

void UPlayerAnimInstance::AnimNotify_AttackEnd()
{
	mAttacking = false;

	APlayerCharacter* PlayerCharacter =	Cast<APlayerCharacter>(TryGetPawnOwner());

	if (IsValid(PlayerCharacter))
		PlayerCharacter->EndAttack();
}

void UPlayerAnimInstance::AnimNotify_ReloadStart()
{
}

void UPlayerAnimInstance::AnimNotify_ReloadEnd()
{
}

void UPlayerAnimInstance::AnimNotify_LoopStart()
{
	Montage_JumpToSection("Loop");
}

void UPlayerAnimInstance::AnimNotify_SkillStart()
{
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(TryGetPawnOwner());

	if (!IsValid(PlayerCharacter))
		return;

	UAbilitySystemComponent* ASC = PlayerCharacter->GetAbilitySystemComponent();

	if (!ASC)
		return;

	// GameplayEvent 데이터 준비
	FGameplayEventData EventData;
	EventData.EventTag = FGameplayTag::RequestGameplayTag(FName("Custom.Player.Event.SkillStart"));
	EventData.Instigator = PlayerCharacter;

	// 이벤트 트리거
	ASC->HandleGameplayEvent(EventData.EventTag, &EventData);
}

void UPlayerAnimInstance::AnimNotify_SkillEnd()
{
}