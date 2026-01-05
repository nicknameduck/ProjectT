// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster.h"
#include "MonsterAnimInstance.h"
#include "MonsterAIController.h"
#include "MonsterMovement.h"

#include "UI/HealthBarWidget.h"

#include "UEAssetManager.h"
#include "GameInfo.h"

#include "Components/WidgetComponent.h"
//#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"
//#include "BehaviorTree/BlackboardData.h"
#include "BrainComponent.h"

#include "AbilitySystemComponent.h"
#include "GameplayEffectExtension.h"

#include "AbilitySet/AbilitySet.h"
#include "Effect/MonsterEffectSet.h"

#include "AttributeSet/Common/CombatSet.h"
#include "AttributeSet/Common/MovementSet.h"
#include "AttributeSet/Common/ProgressionSet.h"
#include "AttributeSet/Common/ResourceSet.h"

#include "Navigation/PathFollowingComponent.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"

//#include "Perception/AIPerceptionStimuliSourceComponent.h"
//#include "Perception/AIPerceptionTypes.h"
//#include "Perception/AISenseConfig_Sight.h"
//#include "Perception/AISenseConfig_Hearing.h"
//#include "Perception/AISenseConfig_Damage.h"

// Sets default values
AMonster::AMonster()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UBehaviorTree> MonsterBehaviorTree(
		TEXT("/Script/AIModule.BehaviorTree'/Game/Blueprints/Monster/BT_Monster.BT_Monster'"));

	if(MonsterBehaviorTree.Succeeded())
		mBehaviorTree = MonsterBehaviorTree.Object;

	static ConstructorHelpers::FObjectFinder<UAbilitySet> MonsterAbilitySet(
		TEXT("/Script/ProjectT.AbilitySet'/Game/GAS/AbilitySet/Monster/DA_Monster_DefaultSet.DA_Monster_DefaultSet'"));

	if(MonsterAbilitySet.Succeeded())
		mDefaultAbilitySet = MonsterAbilitySet.Object;	

	static ConstructorHelpers::FObjectFinder<UMonsterEffectSet> MonsterEffectSet(
		TEXT("/Script/ProjectT.MonsterEffectSet'/Game/GAS/Effect/DA_Monster_EffectSet.DA_Monster_EffectSet'"));

	if (MonsterEffectSet.Succeeded())
		mEffectSet = MonsterEffectSet.Object;

	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> SpawnEffect(
		TEXT("/Script/Niagara.NiagaraSystem'/Game/Assets/AdvancedPortalsSystemVFX/VFX/Vortex/NS_Portal_Vortex_1.NS_Portal_Vortex_1'"));

	if (SpawnEffect.Succeeded())
		mSpawnNiagara = SpawnEffect.Object;

	mASC = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("ASC"));
	mCombatSet = CreateDefaultSubobject<UCombatSet>(TEXT("CombatSet"));
	mMovementSet = CreateDefaultSubobject<UMovementSet>(TEXT("MovementSet"));
	mProgressionSet = CreateDefaultSubobject<UProgressionSet>(TEXT("ProgressionSet"));
	mResourceSet = CreateDefaultSubobject<UResourceSet>(TEXT("ResourceSet"));

	UCapsuleComponent* Capsule = GetCapsuleComponent();
	const float HalfHeight = Capsule->GetScaledCapsuleHalfHeight();
	const FVector RelativeLoc(0.f, 0.f, -HalfHeight);

	GetCapsuleComponent()->SetCollisionProfileName("Monster");

	mASC->AddAttributeSetSubobject<UCombatSet>(mCombatSet);
	mASC->AddAttributeSetSubobject<UMovementSet>(mMovementSet);
	mASC->AddAttributeSetSubobject<UProgressionSet>(mProgressionSet);
	mASC->AddAttributeSetSubobject<UResourceSet>(mResourceSet);

	mASC->GetGameplayAttributeValueChangeDelegate(UResourceSet::GetHPAttribute()).AddUObject(this, &AMonster::OnHPChanged);

	mASC->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag(TEXT("Custom.Monster")));

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	AIControllerClass = AMonsterAIController::StaticClass();

	bUseControllerRotationYaw = true;
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->RotationRate = FRotator(0, 720, 0); // 도는 속도

	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionResponseToAllChannels(ECR_Ignore);
}

// Called when the game starts or when spawned
void AMonster::BeginPlay()
{
	Super::BeginPlay();

	mAnimInst = Cast<UMonsterAnimInstance>(GetMesh()->GetAnimInstance());
	PT_LOG_ALERT(nullptr != mAnimInst, Warning, TEXT("AnimInst null"));
	
	PT_LOG_ALERT(nullptr != mHealthBarWidgetClass, Warning, TEXT("HealthBarWidgetClass가 nullptr입니다. 몬스터의 블루프린트에서 HPBar 블루프린트 세팅이 필요합니다."));

	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	mHealthBarWidget = CreateWidget<UHealthBarWidget>(PC, mHealthBarWidgetClass);

	if (mHealthBarWidget)
	{
		mHealthBarWidget->AddToViewport();
		mHealthBarWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
		// 픽셀 고정 크기 원하면:
		mHealthBarWidget->SetAlignmentInViewport(FVector2D(0.5f, 1.f)); // 가운데-아래 기준
		//mHealthBarWidget->SetDrawAtDesiredSize(false);
		mHealthBarWidget->SetDesiredSizeInViewport(FVector2D(80, 10));  // 테스트용 고정 크기
	}	

	mASC->InitAbilityActorInfo(this, this);

	const FMonsterInfo* MonsterInfo = UUEAssetManager::Get()->FindDataTableRow<FMonsterInfo>(TEXT("DT_MonsterInfo"), mMonsterKey);
	PT_LOG_ALERT(nullptr != MonsterInfo, Warning, TEXT("MonsterInfo가 nullptr입니다 %s"), *mMonsterKey.ToString());
	if (nullptr != MonsterInfo)
	{
		InitAttributeSet(MonsterInfo);
		mMonsterType = MonsterInfo->MonsterType;
	}

	if (mASC && mDefaultAbilitySet)	
		mDefaultAbilitySet->GiveToASC(mASC, GetCharacterHandles());

	int32	MtrlCount = GetMesh()->GetNumMaterials();

	for (int32 i = 0; i < MtrlCount; ++i)
	{
		UMaterialInstanceDynamic* DynMat = GetMesh()->CreateDynamicMaterialInstance(i);
		if (DynMat)
			mMaterialEffectData.Mats.Add(DynMat);
	}

	AAIController* AIController = Cast<AAIController>(GetController());

	if (AIController && mBehaviorTree)
		AIController->RunBehaviorTree(mBehaviorTree);
		
}

void AMonster::EndPlay(const EEndPlayReason::Type Reason)
{
	Super::EndPlay(Reason);

	if (mHealthBarWidget)
	{
		mHealthBarWidget->RemoveFromParent();
		mHealthBarWidget = nullptr;
	}
	UE_LOG(ProjectTLog, Warning, TEXT("EndPlay"));
	mOnMonsterDied.Broadcast(this);
}

// Called every frame
void AMonster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (mMaterialEffectData.HitEnable)
	{
		mMaterialEffectData.HitAccTime += DeltaTime;

		if (mMaterialEffectData.HitAccTime >= mMaterialEffectData.HitTime)
		{
			mMaterialEffectData.HitEnable = false;
			mMaterialEffectData.HitAccTime = 0.f;

			for (auto& Mtrl : mMaterialEffectData.Mats)
			{
				Mtrl->SetScalarParameterValue(TEXT("HitEnable"), 0.f);
			}
		}
	}

	if (mHealthBarWidget)
	{
		APlayerController* PC = GetWorld()->GetFirstPlayerController();
		const FVector WorldPos = GetActorLocation() + FVector(0, 0, 200);

		FVector2D ScreenPos;
		const bool bOnScreen = PC->ProjectWorldLocationToScreen(WorldPos, ScreenPos);

		if (bOnScreen)
		{
			mHealthBarWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
			mHealthBarWidget->SetPositionInViewport(ScreenPos);
		}
		else
			mHealthBarWidget->SetVisibility(ESlateVisibility::Collapsed);
	}

	AAIController* AIController = Cast<AAIController>(GetController());

	/*UE_LOG(LogTemp, Warning, TEXT("UseCtrlYaw=%d OrientToMove=%d UseCtrlDesired=%d AllowStrafe=%d  Focus=%s  CtrlYaw=%.1f ActYaw=%.1f"),
		bUseControllerRotationYaw,
		GetCharacterMovement()->bOrientRotationToMovement,
		GetCharacterMovement()->bUseControllerDesiredRotation,
		(AIController->GetPathFollowingComponent() ? AIController->bAllowStrafe : -1),
		*GetNameSafe(AIController->GetFocusActor()),
		AIController->GetControlRotation().Yaw, GetActorRotation().Yaw);*/
}

// Called to bind functionality to input
void AMonster::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AMonster::InitAttributeSet(const FMonsterInfo* MonsterInfo)
{
	mCombatSet->InitAttack(MonsterInfo->Attack);
	mCombatSet->InitDefense(MonsterInfo->Defense);
	mCombatSet->InitAttackRange(MonsterInfo->AttackRange);
	mCombatSet->InitAttackSpeed(MonsterInfo->AttackSpeed);
	mMovementSet->InitMoveSpeed(MonsterInfo->MoveSpeed);
	mProgressionSet->InitLevel(1);
	mProgressionSet->InitExp(0);
	mResourceSet->InitHP(MonsterInfo->HP);
	mResourceSet->InitHPMax(MonsterInfo->HP);
	mResourceSet->InitMP(MonsterInfo->MP);
	mResourceSet->InitMPMax(MonsterInfo->MP);
	mResourceSet->InitGold(0);

	GetCharacterMovement()->MaxWalkSpeed = MonsterInfo->MoveSpeed;
}

UBlackboardComponent* AMonster::GetBlackboardComponent()
{
	AAIController* MonsterAI = Cast<AAIController>(GetController());
	if (!MonsterAI)
		return nullptr;

	return MonsterAI->GetBlackboardComponent();
}

void AMonster::OnHPChanged(const FOnAttributeChangeData& Data)
{
	if (mAnimInst->GetCurrentAnimType() == EMonsterAnim::Die)
		return;

	mHealthBarWidget->SetHPPercent(mResourceSet->GetHpPercent());
	
	AAIController* MonsterAI = Cast<AAIController>(GetController());
	if (!MonsterAI)
		return;

	UBlackboardComponent* BBComponent = MonsterAI->GetBlackboardComponent();
	if (!BBComponent)
		return;

	if (Data.NewValue <= 0.f)
	{
		mIsDead = true;
		SetMonsterAnimType(EMonsterAnim::Die);
		mHealthBarWidget->SetVisibility(ESlateVisibility::Collapsed);

		SetActorEnableCollision(false);
		if (mHealthBarWidget)
		{
			mHealthBarWidget->RemoveFromParent();
			mHealthBarWidget = nullptr;
		}

		MonsterAI->SetFocus(nullptr);
		MonsterAI->BrainComponent->StopLogic(TEXT("Death"));
	}
	else
	{
		if(Data.NewValue < Data.OldValue)
			HitMaterialEffect();

		const FGameplayEffectSpec& Spec = Data.GEModData->EffectSpec;
		// AssetTags 가져오기
		FGameplayTagContainer EffectTags;
		Spec.GetAllAssetTags(EffectTags);

		// 여기서 확인
		if (EffectTags.HasTag(FGameplayTag::RequestGameplayTag(TEXT("Custom.Effect.Damage"))))
		{
			UE_LOG(LogTemp, Warning, TEXT("이번 Attribute 변경은 Damage Effect로부터 왔다!"));
			BBComponent->SetValueAsBool(TEXT("IsStopLogic"), true);
		}

	/*	const FGameplayEffectSpec& Spec = Data.GEModData->EffectSpec;
		bool bIsBurn = Spec.GetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(TEXT("Custom.Effect.Burn")), false, 0.f) != 0.f;
		bool bIsDamage = Spec.GetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(TEXT("Custom.Effect.Damage")), false, 0.f) != 0.f;

		if (bIsDamage)
			BBComponent->SetValueAsBool(TEXT("IsStopLogic"), true);*/
	}
}

void AMonster::Die()
{
	StartDissolve();
}

void AMonster::StartDissolve()
{
	for (auto& Mat : mMaterialEffectData.Mats)
	{
		Mat->SetScalarParameterValue(TEXT("Dissolve"), mMaterialEffectData.DissolveValue);
	}

	GetWorldTimerManager().SetTimer(mMaterialEffectData.DissolveTimer, this, &AMonster::UpdateDissolve, GetWorld()->GetDeltaSeconds(), true, mMaterialEffectData.DelayTime);
}

void AMonster::UpdateDissolve()
{
	mMaterialEffectData.DissolveValue -= mMaterialEffectData.DissolveSpeed * GetWorld()->GetDeltaSeconds();

	for (auto& Mat : mMaterialEffectData.Mats)
	{
		if (Mat)
			Mat->SetScalarParameterValue(TEXT("Dissolve"), mMaterialEffectData.DissolveValue);
	}

	if (mMaterialEffectData.DissolveValue <= 0)
	{
		GetWorldTimerManager().ClearTimer(mMaterialEffectData.DissolveTimer);
		Destroy();
	}
}

void AMonster::HitEnd()
{
	RemoveGamePlayTag(TEXT("Custom.Common.Hit"));

	if (!mIsDead)
		SetBehaviorTreeStopLogic(false);
}

void AMonster::PlaySpawnEffect()
{
	mSpawnElapsed = 0.f;

	// 시작 스케일
	SetActorScale3D(FVector(mSpawnStartScale));

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			mSpawnEffectTimerHandle,
			this,
			&AMonster::UpdateSpawnEffect,
			mSpawnEffectTickInterval,
			true
		);

		const FVector SpawnLocation = GetActorLocation() + FVector(0.f, 0.f, 3.f);
		const FRotator SpawnRotation = GetActorRotation();

		if (mSpawnNiagara)
		{
			mSpawnNiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
				World,
				mSpawnNiagara,
				SpawnLocation,
				SpawnRotation
			);

			if (mSpawnNiagaraComp)
				mSpawnNiagaraComp->SetAutoDestroy(true);

			mSpawnNiagaraComp->SetRelativeScale3D(FVector(mSpawnStartScale));
		}
	}

	SetBehaviorTreeStopLogic(true);
}

void AMonster::UpdateSpawnEffect()
{
	mSpawnElapsed += mSpawnEffectTickInterval;

	float Alpha = FMath::Clamp(mSpawnElapsed / mSpawnEffectDuration, 0.f, 1.f);

	float Scale = FMath::Lerp(mSpawnStartScale, 1.f, Alpha);
	SetActorScale3D(FVector(Scale));
	mSpawnNiagaraComp->SetRelativeScale3D(FVector(Scale));

	if (1.f <= Alpha)
	{
		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().ClearTimer(mSpawnEffectTimerHandle);

			mFadeOutElapsed = 1;

			World->GetTimerManager().SetTimer(
				mFadeOutTimerHandle,
				this,
				&AMonster::UpdateFadeOutSpawnEffect,
				mSpawnEffectTickInterval,
				true
			);
		}

		SetActorScale3D(FVector(1.f));

		if (!mIsDead)
			SetBehaviorTreeStopLogic(false);
	}
}

void AMonster::UpdateFadeOutSpawnEffect()
{
	mFadeOutElapsed -= mSpawnEffectTickInterval;

	float Alpha = FMath::Clamp(mFadeOutElapsed / mSpawnEffectDuration, 0.f, 1.f);

	float Scale = FMath::Lerp(mSpawnStartScale, 1.f, Alpha);
	mSpawnNiagaraComp->SetRelativeScale3D(FVector(Scale));

	if (Alpha <= 0)
	{
		mSpawnNiagaraComp->Deactivate();
		mSpawnNiagaraComp->DestroyComponent();
		mSpawnNiagaraComp = nullptr;

		if (UWorld* World = GetWorld())
			World->GetTimerManager().ClearTimer(mFadeOutTimerHandle);
	}
}

void AMonster::AddGamePlayTag(const FName& TagName)
{
	mASC->AddReplicatedLooseGameplayTag(FGameplayTag::RequestGameplayTag(TagName));
}

void AMonster::RemoveGamePlayTag(const FName& TagName)
{
	mASC->RemoveReplicatedLooseGameplayTag(FGameplayTag::RequestGameplayTag(TagName));
}

void AMonster::HitMaterialEffect()
{
	mMaterialEffectData.HitEnable = true;
	mMaterialEffectData.HitAccTime = 0.f;

	for (auto& Mtrl : mMaterialEffectData.Mats)
		Mtrl->SetScalarParameterValue(TEXT("HitEnable"), 1.f);
}

void AMonster::SetMonsterAnimType(EMonsterAnim Type)
{
	if (mIsDead && Type != EMonsterAnim::Die)
		return;

	switch (Type)
	{
	case EMonsterAnim::Die:
	case EMonsterAnim::Dizzy:
		{
			if(mASC->GetCurrentMontage())
				mASC->CurrentMontageStop();

			if (!mAnimInst->GetCurrentActiveMontage())
				mAnimInst->Montage_Stop(0.f);

			break;
		}
	default:
		break;
		
	}

	mAnimInst->SetAnimType(Type);
}

void AMonster::SetBehaviorTreeStopLogic(bool IsStop)
{
	UBlackboardComponent* BBComponent = GetBlackboardComponent();
	if (!BBComponent)
		return;

	BBComponent->SetValueAsBool(TEXT("IsStopLogic"), IsStop);
}
