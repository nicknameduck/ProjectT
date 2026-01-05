// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameCore.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Pawn.h"
#include "AbilitySet/AbilitySet.h"

#include "Monster.generated.h"

class UCombatSet;
class UMovementSet;
class UProgressionSet;
class UResourceSet;
class UWidgetComponent;
class UHealthBarWidget;
class UMonsterMovement;
class UBehaviorTree;
class UMonsterAnimInstance;
class UMonsterEffectSet;
class UBlackboardComponent;
class UNiagaraComponent;
class UNiagaraSystem;
struct FMonsterInfo;
struct FOnAttributeChangeData;

enum class EMonsterAnim : uint8;
enum class EMonsterType : uint8;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMonsterDied, AMonster*, Monster);

USTRUCT()
struct FMaterialEffectData
{
	GENERATED_BODY()

	FTimerHandle DissolveTimer;								// 타이머 핸들

	UPROPERTY()
	TArray<TObjectPtr<UMaterialInstanceDynamic>> Mats;		// 동적 머티리얼 인스턴스

	float	DissolveValue = 1.f;								// 진행도 (0.0 ~ 1.0)
	float	DissolveSpeed = 0.5f;								// 속도 Speed * DeltaTime
	float	DelayTime = 0.3f;

	bool	HitEnable = false;
	float	HitTime = 0.2f;
	float	HitAccTime = 0.f;
};

UCLASS()
class PROJECTT_API AMonster : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AMonster();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type Reason) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	virtual void SpawnProjectile() {};

public:
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override
	{
		return mASC;
	}

	void InitAttributeSet(const FMonsterInfo* MonsterInfo);

	UBehaviorTree* GetBehaviorTree()
	{
		return mBehaviorTree;
	}

	UMonsterAnimInstance* GetAnimInstance()
	{
		return mAnimInst;
	}

	const bool IsDead() const
	{
		return mIsDead;
	}

	const EMonsterType GetMonsterType() const
	{
		return mMonsterType;
	}

	UBlackboardComponent* GetBlackboardComponent();

public:
	void OnHPChanged(const FOnAttributeChangeData& Data);
	void Die();
	void StartDissolve();
	void UpdateDissolve();
	void HitEnd();
	void PlaySpawnEffect();
	void UpdateSpawnEffect();
	void UpdateFadeOutSpawnEffect();

	void AddGamePlayTag(const FName& TagName);
	void RemoveGamePlayTag(const FName& TagName);

	void HitMaterialEffect();

	void SetMonsterAnimType(EMonsterAnim Type);
	void SetBehaviorTreeStopLogic(bool IsStop);

public:
	// -- DELECATE --
	UPROPERTY(BlueprintAssignable, Category = "Delecate")
	FOnMonsterDied							mOnMonsterDied;

protected:
	UPROPERTY(EditAnywhere, Category = "Data")
	FName									mMonsterKey;

	UPROPERTY()
	TObjectPtr<class UMonsterAnimInstance>	mAnimInst;

	UPROPERTY(EditAnywhere, Category="Widget")
	TSubclassOf<UHealthBarWidget>			mHealthBarWidgetClass;

	UPROPERTY()
	TObjectPtr<UHealthBarWidget>			mHealthBarWidget;

	UPROPERTY(EditAnywhere, Category="AI")
	TObjectPtr<UBehaviorTree>				mBehaviorTree;

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent>		mASC;

	UPROPERTY()
	TObjectPtr<UNiagaraSystem>				mSpawnNiagara;

	UPROPERTY()
	TObjectPtr<UNiagaraComponent>			mSpawnNiagaraComp;

	// -- AbilitySet --
	UPROPERTY(EditDefaultsOnly, Category = "Abilities")
	TObjectPtr<UAbilitySet>					mDefaultAbilitySet;

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

	// -- EffectSet --
	UPROPERTY(EditDefaultsOnly, Category = "EffectSet")
	TObjectPtr<UMonsterEffectSet>			mEffectSet;

	UPROPERTY()
	FMaterialEffectData						mMaterialEffectData;

	// -- SpawnEffect --
	FTimerHandle mSpawnEffectTimerHandle;

	FTimerHandle mFadeOutTimerHandle;

	UPROPERTY(EditAnywhere, Category = "Spawn Effect")
	float mSpawnEffectDuration = 0.3f;    // 전체 재생 시간

	UPROPERTY(EditAnywhere, Category = "Spawn Effect")
	float mSpawnEffectTickInterval = 0.01f;

	UPROPERTY(EditAnywhere, Category = "Spawn Effect")
	float mSpawnStartScale = 0.1f;

	float mSpawnElapsed = 0.f;

	float mFadeOutElapsed = 0.f;

	UPROPERTY()
	EMonsterType							mMonsterType;

	bool									mIsDead = false;

};
