// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../GameCore.h"
#include "GameFramework/Character.h"
#include "GameInfo.h"
#include "GameplayEffect.h"
#include "AbilitySystemInterface.h"
#include "AbilitySet/AbilitySet.h"
#include "PlayerCharacter.generated.h"

class UPlayerAnimInstance;
class AMonster;
class UHealthBarWidget;
class UGameplayAbility;
class USkillComponent;
class UAbilitySystemComponent;
class ADecalActor;
class UCombatSet;
class UMovementSet;
class UProgressionSet;
class UResourceSet;
class UPlayerHUDWidget;
struct FInputActionValue;

UCLASS()
class PROJECTT_API APlayerCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type Reason) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override
	{
		return mASC;
	}

public:
	UPlayerAnimInstance* GetAnimInstance();
	USceneComponent*	GetFirePointComponent();

	virtual void StatAttack();
	virtual void EndAttack();

	UFUNCTION()
	void MoveKey(const FInputActionValue& Value);

	UFUNCTION()
	void AttackKey(const FInputActionValue& Value);

	UFUNCTION()
	void AttackKeyReleased(const FInputActionValue& Value);

	UFUNCTION()
	void Skill_QKey(const FInputActionValue& Value);

	UFUNCTION()
	void Skill_QReleased(const FInputActionValue& Value);

	UFUNCTION()
	void Skill_EKey(const FInputActionValue& Value);

	UFUNCTION()
	void Skill_EReleased(const FInputActionValue& Value);

	UFUNCTION()
	void Skill_RKey(const FInputActionValue& Value);

	UFUNCTION()
	void Skill_RReleased(const FInputActionValue& Value);

	UFUNCTION()
	void Skill_RMBKey(const FInputActionValue& Value);

	UFUNCTION()
	void Skill_RMBReleased(const FInputActionValue& Value);
		
public:
	void OnHPChanged(const FOnAttributeChangeData& Data);
	FRotator GetNearestMonsterAimRotation(FVector PlayerPos);
	FRotator GetMouseTargetRotator();

	void SetRotationMode(ERotationMode RotationMode);
	void ShowRange(bool bShow, float Range = 0.f);

protected:
	void InitPlayerAttributeSet(const FPlayerInfo* PlayerInfo);

	virtual void HandleAttackInput();
	virtual void HandleAttackInputReleased();

	virtual void HandleSkillInput(ESkillSlot SlotType);
	virtual void HandleSkillInputReleased(ESkillSlot SlotType);

	void PlayHitVignetteEffect();
protected:
	// -- Component -- 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UAbilitySystemComponent>		mASC;
	UPROPERTY(EditAnywhere, Category = "Component")
	TObjectPtr<USpringArmComponent>			mSpringArm;

	UPROPERTY()
	TObjectPtr<USkillComponent>				mSkill;

	UPROPERTY(EditAnywhere, Category = "Component")
	TObjectPtr<USceneComponent>				mFirePoint;

	UPROPERTY(EditAnywhere, Category = "Component")
	TObjectPtr<UCameraComponent>			mCamera;

	UPROPERTY()
	TObjectPtr<UStaticMeshComponent>		mRange;

	// -- AbilitySet --
	UPROPERTY(EditDefaultsOnly, Category = "Abilities")
	TObjectPtr<UAbilitySet>					mDefaultAbilitySet;

	// -- AbilitySet --
	DECLARE_ABILITYSET(Character)

	// -- AttributeSet --
	UPROPERTY()
	TObjectPtr<UCombatSet>					mCombatSet;

	UPROPERTY()
	TObjectPtr<UMovementSet>				mMovementSet;

	UPROPERTY()
	TObjectPtr<UProgressionSet>				mProgressionSet;

	UPROPERTY()
	TObjectPtr<UResourceSet>				mResourceSet;

	// -- FrameWork References --
	UPROPERTY(EditAnywhere)
	TObjectPtr<UPlayerAnimInstance>			mAnimInst;

	UPROPERTY(EditAnywhere, Category = "Widget")
	TSubclassOf<UHealthBarWidget>			mHealthBarWidgetClass;

	UPROPERTY()
	TObjectPtr<UHealthBarWidget>			mHealthBarWidget;

	UPROPERTY(EditDefaultsOnly, Category = "Camera")
	TSubclassOf<UCameraShakeBase>			mHitCameraShakeClass;

	UPROPERTY()
	TObjectPtr<UPlayerHUDWidget>			mPlayerHUDWidget;

	// -- HitVigentte --
	UPROPERTY(EditAnywhere, Category = "Vigentte")
	TObjectPtr<UMaterialParameterCollection> mHitVignetteCollection;

	UPROPERTY()
	TObjectPtr<UMaterialParameterCollectionInstance> mHitVignetteInstance;

	// -- Data -- 
	UPROPERTY()
	TObjectPtr<ADecalActor>					mRangeDecal;

	UPROPERTY(EditDefaultsOnly, Category = "Material")
	TObjectPtr<UMaterialInterface>			mRangeMaterialInterface;

	UPROPERTY(EditAnywhere)
	FName									mDataKey;	


protected:
	FTimerHandle	mHitVignetteFadeHandle;
	ERotationMode	mRotationMode = ERotationMode::Movement;
	
};
