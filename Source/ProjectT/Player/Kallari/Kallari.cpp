// Fill out your copyright notice in the Description page of Project Settings.


#include "Kallari.h"
#include "ProjectTPlayerState.h"
#include "AbilitySystemComponent.h"
#include "Ability/Player/GA_Player_Kallari_Attack.h"

AKallari::AKallari()
{
	mDataKey = TEXT("Kallari");

	static ConstructorHelpers::FObjectFinder<USkeletalMesh>
		MeshAsset(TEXT("/Script/Engine.SkeletalMesh'/Game/Assets/ParagonKallari/Characters/Heroes/Kallari/Meshes/Kallari.Kallari'"));

	if (MeshAsset.Succeeded())
		GetMesh()->SetSkeletalMeshAsset(MeshAsset.Object);

	static ConstructorHelpers::FClassFinder<UAnimInstance>
		AnimAsset(TEXT("/Script/Engine.AnimBlueprint'/Game/Blueprints/Player/Kallari/ABP_Kallari.ABP_Kallari_C'"));

	if (AnimAsset.Succeeded())
		GetMesh()->SetAnimInstanceClass(AnimAsset.Class);
		
	GetCapsuleComponent()->SetCapsuleHalfHeight(95.f);
	GetMesh()->SetRelativeLocation(FVector(0.0, 0.0, -95.f));
	GetMesh()->SetRelativeRotation(FRotator(0.0, -90.0, 0.0));
}

void AKallari::HandleAttackInput()
{
	mASC->TryActivateAbilityByClass(UGA_Player_Kallari_Attack::StaticClass());
}
