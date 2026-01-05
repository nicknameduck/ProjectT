// Fill out your copyright notice in the Description page of Project Settings.


#include "Chest.h"
#include "Components/WidgetComponent.h"
#include "AttributeSet/Common/CombatSet.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Effect/MonsterEffectSet.h"
#include "ProjectTPlayerState.h"

AChest::AChest()
{
	ConstructorHelpers::FObjectFinder<USkeletalMesh>
		MeshAsset(TEXT("/Script/Engine.SkeletalMesh'/Game/Assets/MonsterForSurvivalGame/Mesh/PBR/ChestMonster_SK.ChestMonster_SK'"));

	if (MeshAsset.Succeeded())
		GetMesh()->SetSkeletalMeshAsset(MeshAsset.Object);

	static ConstructorHelpers::FClassFinder<UAnimInstance>
		AnimAsset(TEXT("/Script/Engine.AnimBlueprint'/Game/Blueprints/Monster/Chest/ABP_Chest.ABP_Chest_C'"));

	if (AnimAsset.Succeeded())
		GetMesh()->SetAnimInstanceClass(AnimAsset.Class);

	//mHealthBarWidgetComponent->SetRelativeLocation(FVector(0.f, 100.f, 300.f));
}

void AChest::BeginPlay()
{
	Super::BeginPlay();
}

void AChest::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}