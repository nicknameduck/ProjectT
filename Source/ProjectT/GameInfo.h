#pragma once

#include "GameplayTagContainer.h"
#include "GameInfo.generated.h"

class UGameplayEffect;
class UGroundTargetSkillData;
class UStreamSkillData;

USTRUCT(BlueprintType)
struct FPlayerAnimInfo : public FTableRowBase
{
	GENERATED_BODY()

public:	
	UPROPERTY(EditAnywhere)
	TMap<FName, TObjectPtr<UAnimSequence>>	SequenceMap;

	UPROPERTY(EditAnywhere)
	TMap<FName, TObjectPtr<UBlendSpace>>	BlendSpaceMap;

	UPROPERTY(EditAnywhere)
	TMap<FName, TObjectPtr<UAnimMontage>>	MontageMap;

	UPROPERTY(EditAnywhere)
	TArray<FName>							AttackSectionArray;
};

UENUM(BlueprintType)
enum class ESkillSlot : uint8
{
	Q,
	E,
	R,
	RMB,
	Count
};

UENUM(BlueprintType)
enum class EPlayerClass : uint8
{
	Kallari,
	Revenant
};

UENUM(BlueprintType)
enum class ERotationMode : uint8
{
	Movement,
	Cursor
};

UENUM(BlueprintType)
enum class EAbilityInputID : uint8
{
	None        UMETA(DisplayName = "None"),

	//마우스 왼쪽 클릭
	Attack      UMETA(DisplayName = "Attack"),
};

UENUM(BlueprintType)
enum class ESkillCastType : uint8
{
	// 클릭 즉시 발동 (예: 점멸, 버프형, 즉발 스킬)
	Instant UMETA(DisplayName = "Instant"),

	// 조준 후, 클릭해서 발동 (지점 선택형 스킬)
	GroundTarget UMETA(DisplayName = "Ground Target"),

	// 적 유닛 선택 후 발동 (대상 지정형 스킬)
	Target UMETA(DisplayName = "Target"),

	// 꾹 눌러서 차징 후 발동 (차지형 스킬)
	Charge UMETA(DisplayName = "Charge"),

	// 캐릭터 전방 고정형 (방향형 / Line형 스킬)
	Direction UMETA(DisplayName = "Directional")
};

USTRUCT(BlueprintType)
struct FPlayerInfo : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	int32		Attack;

	UPROPERTY(EditAnywhere)
	int32		Defense;

	UPROPERTY(EditAnywhere)
	int32		HP;

	UPROPERTY(EditAnywhere)
	int32		MP;

	UPROPERTY(EditAnywhere)
	EPlayerClass	Class;

	UPROPERTY(EditAnywhere)
	float		AttackRange;

	UPROPERTY(EditAnywhere)
	float		AttackSpeed = 1.f;

	UPROPERTY(EditAnywhere)
	float		MoveSpeed = 600.f;
};

UENUM(BlueprintType)
enum class EMonsterAnim : uint8
{
	Idle,
	IdleUnique,
	IdleBattle,
	Walk,
	Run,
	Die,
	Dizzy,
	Hit
};

USTRUCT(BlueprintType)
struct FMonsterAnimInfo : public  FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TMap<FName, TObjectPtr<UAnimSequence>>	SequenceMap;

	UPROPERTY(EditAnywhere)
	TMap<FName, TObjectPtr<UBlendSpace>>	BlendSpaceMap;

	UPROPERTY(EditAnywhere)
	TMap<FName, TObjectPtr<UAnimMontage>>	MontageMap;
};

UENUM(BlueprintType)
enum class EMonsterType : uint8
{
	//근거리 몬스터
	Melee  UMETA(DisplayName = "Melee"),

	//원거리 몬스터
	Ranged UMETA(DisplayName = "Ranged"),
};

USTRUCT(BlueprintType)
struct FMonsterInfo : public  FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	FString		Name;

	UPROPERTY(EditAnywhere)
	int32		Attack;

	UPROPERTY(EditAnywhere)
	int32		Defense;

	UPROPERTY(EditAnywhere)
	int32		HP;

	UPROPERTY(EditAnywhere)
	int32		MP;

	UPROPERTY(EditAnywhere)
	float		AttackRange;

	UPROPERTY(EditAnywhere)
	float		AttackSpeed = 1.f;

	UPROPERTY(EditAnywhere)
	float		MoveSpeed = 600.f;

	UPROPERTY(EditAnywhere)
	float		DetectRange = 1000.f;

	UPROPERTY(EditAnywhere)
	float		ViewHalfAngle = 45.f;

	UPROPERTY(EditAnywhere)
	int32		Level;

	UPROPERTY(EditAnywhere)
	int32		Exp;

	UPROPERTY(EditAnywhere)
	int32		Gold;

	UPROPERTY(EditAnywhere)
	EMonsterType		MonsterType;
};

USTRUCT(BlueprintType)
struct FGameplayEffectData
{
	GENERATED_BODY()
public:
	// 적용할 GameplayEffect
	UPROPERTY(EditAnywhere) 
	TSubclassOf<UGameplayEffect> EffectClass;

	// GameplayEffect의 태그 및 Value
	UPROPERTY(EditAnywhere) 
	TMap<FGameplayTag, float> SetByCallerParams;
};

USTRUCT(BlueprintType)
struct FSkillInfo : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CoolDown")
	FGameplayTag	CoolDownTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CoolDown")
	TSubclassOf<UGameplayEffect>	CoolDownEffect;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage")
	float BaseDamage = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage")
	float DamageMultiplier = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CoolDown")
	float CoolDown = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SkillProperty")
	float Duration = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SkillProperty")
	ESkillCastType	CastType = ESkillCastType::Instant;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SkillData")
	TObjectPtr<UGroundTargetSkillData>	GroundTargetSkillData;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GameplayEffect")
	TArray<FGameplayEffectData>			GameplayEffects;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SkillProperty")
	FString			SkillName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SkillProperty")
	FString			SkillDesc;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SkillData")
	TObjectPtr<UStreamSkillData>	StreamSkillData;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SkillProperty")
	TObjectPtr<UTexture2D>			SkillIcon;
	
};

UENUM(BlueprintType)
enum class ERoomType : uint8
{
	Combat,
	Elite,
	Boss,
	Chest,
	Shop,
	Rest
};

USTRUCT(BlueprintType)
struct FMapNode
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D Position;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<int32> ConnectedNodes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ERoomType RoomType; // 전투, 보상, 보스 등

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsCleared;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsCurrentNode;
};