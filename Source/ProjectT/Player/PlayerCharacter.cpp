// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"

#include "UIManager.h"
#include "UEAssetManager.h"

#include "Monster.h"
#include "Input.h"
#include "EnhancedInputSubsystems.h"
#include "PlayerAnimInstance.h"
#include "ProjectTPlayerState.h"
#include "TimerManager.h"
#include "Engine/DecalActor.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Components/DecalComponent.h"

#include "EnhancedInputComponent.h"
#include "AbilitySystemComponent.h"
#include "SkillComponent.h"
#include "AttributeSet/Common/CombatSet.h"
#include "AttributeSet/Common/MovementSet.h"
#include "AttributeSet/Common/ProgressionSet.h"
#include "AttributeSet/Common/ResourceSet.h"

#include "Camera/CameraShakeBase.h"

#include "HealthBarWidget.h"
#include "PlayerHUDWidget.h"

#include "Materials/MaterialParameterCollection.h"
#include "Materials/MaterialParameterCollectionInstance.h"
#include "Kismet/GameplayStatics.h"

#include "KismetAnimationLibrary.h"

// Sets default values
APlayerCharacter::APlayerCharacter()
{
	static ConstructorHelpers::FClassFinder<UCameraShakeBase> CameraShake(
		TEXT("/Script/Engine.Blueprint'/Game/Blueprints/Camera/BP_HitCameraShake.BP_HitCameraShake_C'"));

	if(CameraShake.Succeeded())
		mHitCameraShakeClass = CameraShake.Class;

	static ConstructorHelpers::FObjectFinder<UMaterialParameterCollection> HitVignette(
		TEXT("/Script/Engine.MaterialParameterCollection'/Game/Material/MPC_HitVignette.MPC_HitVignette'"));

	if (HitVignette.Succeeded())
		mHitVignetteCollection = HitVignette.Object;

	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	mASC = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("ASC"));
	mSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	mCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	mSkill = CreateDefaultSubobject<USkillComponent>(TEXT("Skill"));
	mFirePoint = CreateDefaultSubobject<USceneComponent>(TEXT("FirePoint"));
		
	//mSpringArm->SetupAttachment(GetCapsuleComponent());
	mSpringArm->SetupAttachment(nullptr);
	mCamera->SetupAttachment(mSpringArm);
	mFirePoint->SetupAttachment(RootComponent);
	//mCamera->Activate(true);

	mCombatSet = CreateDefaultSubobject<UCombatSet>(TEXT("CombatSet"));
	mMovementSet = CreateDefaultSubobject<UMovementSet>(TEXT("MovementSet"));
	mProgressionSet = CreateDefaultSubobject<UProgressionSet>(TEXT("ProgressionSet"));
	mResourceSet = CreateDefaultSubobject<UResourceSet>(TEXT("ResourceSet"));

	mASC->AddAttributeSetSubobject<UCombatSet>(mCombatSet);
	mASC->AddAttributeSetSubobject<UMovementSet>(mMovementSet);
	mASC->AddAttributeSetSubobject<UProgressionSet>(mProgressionSet);
	mASC->AddAttributeSetSubobject<UResourceSet>(mResourceSet);

	mASC->GetGameplayAttributeValueChangeDelegate(UResourceSet::GetHPAttribute()).AddUObject(this, &APlayerCharacter::OnHPChanged);
	mASC->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag(TEXT("Custom.Player")));

	mSpringArm->TargetArmLength = 1500.f;
	mSpringArm->bUsePawnControlRotation = false;
	mSpringArm->bInheritPitch = false;
	mSpringArm->bInheritYaw = false;
	mSpringArm->bInheritRoll = false;
	mSpringArm->bDoCollisionTest = false;
	//mCamera->bUsePawnControlRotation = false;

	mSpringArm->SetRelativeLocation(FVector(-210, 0, 121));
	mSpringArm->SetRelativeRotation(FRotator(-60, 0, 0));

	//[Todo:Psh] PlayerInfo를 만들면 해당 Info에서 MaxSpeed를 가져올수있도록 하자
	GetCharacterMovement()->MaxWalkSpeed = 1000.f;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 720.f, 0.f);
	bUseControllerRotationYaw = false;

	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionResponseToAllChannels(ECR_Ignore);
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	mAnimInst = Cast<UPlayerAnimInstance>(GetMesh()->GetAnimInstance());

	const FPlayerInfo* PlayerInfo = UUEAssetManager::Get()->FindDataTableRow<FPlayerInfo>(TEXT("DT_PlayerInfo"), mDataKey);
	PT_LOG_ALERT(nullptr != PlayerInfo, Warning, TEXT("PlayerInfo가 nullptr입니다 %s"), *mDataKey.ToString());
	if (nullptr != PlayerInfo)
		InitPlayerAttributeSet(PlayerInfo);

	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (IsValid(PlayerController))
	{
		mASC->InitAbilityActorInfo(PlayerController, this);
		UEnhancedInputLocalPlayerSubsystem* Subsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());

		const UGameInput* InputCDO = GetDefault<UGameInput>();

		Subsystem->AddMappingContext(InputCDO->mContext, 0);
	}

	if (mASC && mDefaultAbilitySet)
		mDefaultAbilitySet->GiveToASC(mASC, GetCharacterHandles());

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
	mPlayerHUDWidget = CUIManager::GetInstance()->FindWidget<UPlayerHUDWidget>(TEXT("PlayerHUD"));
	if (mPlayerHUDWidget)
	{
		for (int32 i = 0; i < 5; ++i)
			mPlayerHUDWidget->SetSkillSlotImage(i, nullptr);
	}

	/*if(PC)
		PC->SetViewTargetWithBlend(this, 0.f);*/

	if (mHitVignetteCollection)
		mHitVignetteInstance = GetWorld()->GetParameterCollectionInstance(mHitVignetteCollection);
}

void APlayerCharacter::EndPlay(const EEndPlayReason::Type Reason)
{
	Super::EndPlay(Reason);

	if (mHealthBarWidget)
	{
		mHealthBarWidget->RemoveFromParent();
		mHealthBarWidget = nullptr;
	}
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (!PlayerController) 
		return;

	FVector WorldOrigin, WorldDirection;
	if (PlayerController->DeprojectMousePositionToWorld(WorldOrigin, WorldDirection))
	{
		// 월드에서 평면(Z=캐릭터 위치)에 맞게 재계산
		FVector PlayerPos = GetActorLocation();
		float ZPlane = PlayerPos.Z;

		float T = (ZPlane - WorldOrigin.Z) / WorldDirection.Z;
		FVector AimTarget = WorldOrigin + WorldDirection * T;

		FVector ToTarget = AimTarget - PlayerPos;
		FVector Dir = PlayerController->GetControlRotation().Vector();

		DrawDebugDirectionalArrow(
			GetWorld(),
			GetActorLocation(),
			GetActorLocation() + ToTarget.GetSafeNormal() * 200.f,
			120.f,
			FColor::Blue,
			false,
			-1.f,
			0,
			5.f
		);
	}

	FVector CameraTargetLoc = GetActorLocation();
	//mSpringArm->SetRelativeLocation(CameraTargetLocation);
	FVector CurrentLoc = mSpringArm->GetComponentLocation();
	FVector NewPos = FMath::VInterpTo(CurrentLoc, CameraTargetLoc, DeltaTime, 8.0f);
	mSpringArm->SetWorldLocation(NewPos);

	DrawDebugDirectionalArrow(
		GetWorld(),
		GetActorLocation(),
		GetActorLocation() + GetActorForwardVector() * 200.f,
		120.f,                // 화살표 크기
		FColor::Green,
		false,
		-1.f,
		0,
		3.f                  // 두께
	);

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
}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* InputCompo = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (IsValid(InputCompo))
	{
		const UGameInput* InputCDO = GetDefault<UGameInput>();

		InputCompo->BindAction(InputCDO->mMove, ETriggerEvent::Triggered, this, &APlayerCharacter::MoveKey);

		InputCompo->BindAction(InputCDO->mAttack, ETriggerEvent::Triggered, this, &APlayerCharacter::AttackKey);
		InputCompo->BindAction(InputCDO->mAttack, ETriggerEvent::Completed, this, &APlayerCharacter::AttackKeyReleased);

		InputCompo->BindAction(InputCDO->mSkill_Q, ETriggerEvent::Started, this, &APlayerCharacter::Skill_QKey);
		InputCompo->BindAction(InputCDO->mSkill_Q, ETriggerEvent::Completed, this, &APlayerCharacter::Skill_QReleased);

		InputCompo->BindAction(InputCDO->mSkill_E, ETriggerEvent::Started, this, &APlayerCharacter::Skill_EKey);
		InputCompo->BindAction(InputCDO->mSkill_E, ETriggerEvent::Completed, this, &APlayerCharacter::Skill_EReleased);

		InputCompo->BindAction(InputCDO->mSkill_R, ETriggerEvent::Started, this, &APlayerCharacter::Skill_RKey);
		InputCompo->BindAction(InputCDO->mSkill_R, ETriggerEvent::Completed, this, &APlayerCharacter::Skill_RReleased);

		InputCompo->BindAction(InputCDO->mSkill_RMB, ETriggerEvent::Started, this, &APlayerCharacter::Skill_RMBKey);
		InputCompo->BindAction(InputCDO->mSkill_RMB, ETriggerEvent::Completed, this, &APlayerCharacter::Skill_RMBReleased);
	}
}

UPlayerAnimInstance* APlayerCharacter::GetAnimInstance()
{
	return mAnimInst;
}

USceneComponent* APlayerCharacter::GetFirePointComponent()
{
	return mFirePoint;
}

void APlayerCharacter::StatAttack()
{

}

void APlayerCharacter::EndAttack()
{
}

void APlayerCharacter::MoveKey(const FInputActionValue& Value)
{
	const FVector In = Value.Get<FVector>();
	AddMovementInput(FVector::ForwardVector, In.X);
	AddMovementInput(FVector::RightVector, In.Y);

	float TargetDeg = UKismetAnimationLibrary::CalculateDirection(
		GetCharacterMovement()->Velocity, GetActorRotation());

	mAnimInst->SetDir(TargetDeg);
}

void APlayerCharacter::AttackKey(const FInputActionValue& Value)
{
	HandleAttackInput();
	
	FGameplayTag ConfirmTag = FGameplayTag::RequestGameplayTag(FName("Custom.Player.Event.Confirm"));
	UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	if (ASC->HasMatchingGameplayTag(ConfirmTag))
	{
		FGameplayEventData Payload;
		Payload.EventTag = ConfirmTag;
		Payload.Instigator = this;

		FHitResult Hit;
		APlayerController* PlayerController = Cast<APlayerController>(GetController());
		if (PlayerController->GetHitResultUnderCursor(ECC_Visibility, true, Hit))
			Payload.TargetData = FGameplayAbilityTargetDataHandle(new FGameplayAbilityTargetData_SingleTargetHit(Hit));
		
		ASC->HandleGameplayEvent(ConfirmTag, &Payload);
	}
}

void APlayerCharacter::AttackKeyReleased(const FInputActionValue& Value)
{
	HandleAttackInputReleased();
}

void APlayerCharacter::Skill_QKey(const FInputActionValue& Value)
{
	HandleSkillInput(ESkillSlot::Q);
}

void APlayerCharacter::Skill_QReleased(const FInputActionValue& Value)
{
	HandleSkillInputReleased(ESkillSlot::Q);
}

void APlayerCharacter::Skill_EKey(const FInputActionValue& Value)
{
	HandleSkillInput(ESkillSlot::E);
}

void APlayerCharacter::Skill_EReleased(const FInputActionValue& Value)
{
	HandleSkillInputReleased(ESkillSlot::E);
}

void APlayerCharacter::Skill_RKey(const FInputActionValue& Value)
{
	HandleSkillInput(ESkillSlot::R);
}

void APlayerCharacter::Skill_RReleased(const FInputActionValue& Value)
{
	HandleSkillInputReleased(ESkillSlot::R);
}

void APlayerCharacter::Skill_RMBKey(const FInputActionValue& Value)
{
	HandleSkillInput(ESkillSlot::RMB);

	FGameplayTag CancelTag = FGameplayTag::RequestGameplayTag(FName("Custom.Player.Event.Cancel"));
	UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	if (ASC->HasMatchingGameplayTag(CancelTag))
	{
		FGameplayEventData Payload;
		Payload.EventTag = CancelTag;
		Payload.Instigator = this;

		ASC->HandleGameplayEvent(CancelTag, &Payload);
	}
}

void APlayerCharacter::Skill_RMBReleased(const FInputActionValue& Value)
{
	HandleSkillInputReleased(ESkillSlot::RMB);
}

void APlayerCharacter::OnHPChanged(const FOnAttributeChangeData& Data)
{
	bool IsHeal = Data.OldValue < Data.NewValue;
	if (IsHeal)
	{

	}
	else
	{
		PlayHitVignetteEffect();
	}
	
	float HPPercent = mASC->GetSet<UResourceSet>()->GetHpPercent();
	mHealthBarWidget->SetHPPercent(HPPercent);

	if (mPlayerHUDWidget)
		mPlayerHUDWidget->SetHPPercent(HPPercent);

	if (!mHitCameraShakeClass)
	{
		//log
		return;
	}

	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC && PC->PlayerCameraManager && mHitCameraShakeClass)
		PC->PlayerCameraManager->StartCameraShake(mHitCameraShakeClass, 1.0f);
}

void APlayerCharacter::InitPlayerAttributeSet(const FPlayerInfo* PlayerInfo)
{
	mCombatSet->InitAttack(PlayerInfo->Attack);
	mCombatSet->InitDefense(PlayerInfo->Defense);
	mCombatSet->InitAttackRange(PlayerInfo->AttackRange);
	mCombatSet->InitAttackSpeed(PlayerInfo->AttackSpeed);
	mMovementSet->InitMoveSpeed(PlayerInfo->MoveSpeed);
	mProgressionSet->InitLevel(1);
	mProgressionSet->InitExp(0);
	mResourceSet->InitHP(PlayerInfo->HP);
	mResourceSet->InitHPMax(PlayerInfo->HP);
	mResourceSet->InitMP(PlayerInfo->MP);
	mResourceSet->InitMPMax(PlayerInfo->MP);
	mResourceSet->InitGold(0);
}

void APlayerCharacter::HandleAttackInput()
{

}

void APlayerCharacter::HandleAttackInputReleased()
{

}

void APlayerCharacter::HandleSkillInput(ESkillSlot SlotType)
{

}

void APlayerCharacter::HandleSkillInputReleased(ESkillSlot SlotType)
{

}

FRotator APlayerCharacter::GetMouseTargetRotator()
{
	FRotator OutRotator = FRotator::ZeroRotator;

	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (!PlayerController)
		return OutRotator;

	FVector WorldOrigin, WorldDirection;
	if (PlayerController->DeprojectMousePositionToWorld(WorldOrigin, WorldDirection))
	{
		FVector ActorLoc = GetActorLocation();
		// 월드에서 평면(Z=캐릭터 위치)에 맞게 재계산
		float ZPlane = ActorLoc.Z;

		float T = (ZPlane - WorldOrigin.Z) / WorldDirection.Z;
		FVector AimTarget = WorldOrigin + WorldDirection * T;

		FVector ToTarget = AimTarget - ActorLoc;
		ToTarget.Normalize();
		FRotator LookAtRot = ToTarget.Rotation();
		OutRotator = LookAtRot;
	}

	return OutRotator;
}

FRotator APlayerCharacter::GetNearestMonsterAimRotation(FVector PlayerPos)
{
	AMonster* TargetMonster = nullptr;
	float BestDot = -1.0f;

	FVector ViewDir = FVector::ZeroVector;  // 또는 마우스 기준 방향

	float CosThreshold = (30.f > 0.0f) ?
		FMath::Cos(FMath::DegreesToRadians(30.f)) : -1.0f;

	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (!PlayerController)
		return FRotator::ZeroRotator;

	FVector WorldOrigin, WorldDirection;
	if (PlayerController->DeprojectMousePositionToWorld(WorldOrigin, WorldDirection))
	{
		// 월드에서 평면(Z=캐릭터 위치)에 맞게 재계산
		float ZPlane = PlayerPos.Z;

		float T = (ZPlane - WorldOrigin.Z) / WorldDirection.Z;
		FVector AimTarget = WorldOrigin + WorldDirection * T;

		FVector ToTarget = AimTarget - PlayerPos;
		ViewDir = ToTarget.GetSafeNormal();
	}

	float MinDistance = BIG_NUMBER;
	for (TActorIterator<AMonster> It(GetWorld()); It; ++It)
	{
		AMonster* Monster = *It;
		if (!Monster || Monster->IsDead())
			continue;

		FVector ToMonster = (Monster->GetActorLocation() - PlayerPos);
		ToMonster.Z = 0.f;
		ToMonster.Normalize();

		float Dist = FVector::Dist(PlayerPos, Monster->GetActorLocation());

		if (MinDistance <= Dist)
			continue;

		if (Dist <= 1000.f)
		{
			float Dot = FVector::DotProduct(ViewDir, ToMonster);

			if (Dot > BestDot && (CosThreshold < 0.0f || Dot > CosThreshold))
			{
				BestDot = Dot;
				TargetMonster = Monster;
			}
		}
	}

	if (TargetMonster)
	{
		FVector TargetLoc = (TargetMonster->GetActorLocation() - PlayerPos).GetSafeNormal();
		return TargetLoc.Rotation();
	}

	return ViewDir.Rotation();
}

void APlayerCharacter::SetRotationMode(ERotationMode RotationMode)
{
	if (mRotationMode == RotationMode)
		return;

	mRotationMode = RotationMode;

	switch (RotationMode)
	{
		case ERotationMode::Movement:
			GetCharacterMovement()->bOrientRotationToMovement = true;
			/*Getcharactermovement()->busecontrollerdesiredrotation = false;
			busecontrollerrotationyaw = false;*/
			break;
		case ERotationMode::Cursor:		
			GetCharacterMovement()->bOrientRotationToMovement = false;
			/*GetCharacterMovement()->bUseControllerDesiredRotation = true;
			bUseControllerRotationYaw = true;*/
			break;		
		default:
			break;
	}
}

void APlayerCharacter::PlayHitVignetteEffect()
{
	if (!mHitVignetteInstance)
		return;

	// 타이머 중복 방지
	GetWorld()->GetTimerManager().ClearTimer(mHitVignetteFadeHandle);

	// 즉시 붉게
	mHitVignetteInstance->SetScalarParameterValue(FName("HitIntensity"), 1.0f);

	const float FadeDuration = 0.25f;
	const int32 Steps = 10;
	const float StepTime = FadeDuration / Steps;

	float Current = 1.0f;

	GetWorld()->GetTimerManager().SetTimer(
		mHitVignetteFadeHandle,
		[this, Current]() mutable
		{
			Current -= 1.0f / Steps;
			Current = FMath::Clamp(Current, 0.0f, 1.0f);

			if (Current < 0.01f)
				Current = 0.0f;

			if (mHitVignetteInstance)
				mHitVignetteInstance->SetScalarParameterValue(FName("HitIntensity"), Current);

			if (Current <= 0.0f)
			{
				mHitVignetteInstance->SetScalarParameterValue(FName("HitIntensity"), 0.f);
				GetWorld()->GetTimerManager().ClearTimer(mHitVignetteFadeHandle);
			}
		},
		StepTime, true
	);
}

void APlayerCharacter::ShowRange(bool bShow, float Range)
{
	if (mRange)
	{
		mRange->DestroyComponent();
		mRange = nullptr;
	}

	if (!bShow)
		return;

	mRange = NewObject<UStaticMeshComponent>(this);
	mRange->RegisterComponent();
	mRange->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	mRange->SetStaticMesh(LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Plane")));
	mRange->SetMaterial(0, mRangeMaterialInterface);
	mRange->SetWorldScale3D(FVector(Range / 50.f));
	mRange->SetWorldRotation(FRotator(0, 0, 0));
	mRange->SetRelativeLocation(FVector(0, 0, 50));
	mRange->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	float CapsuleHalfHeight = 0.0f;
	if (UCapsuleComponent* Capsule = GetCapsuleComponent())
		CapsuleHalfHeight = Capsule->GetScaledCapsuleHalfHeight();

	mRange->SetWorldLocation(GetActorLocation() - FVector(0, 0, CapsuleHalfHeight - 5.f));
}