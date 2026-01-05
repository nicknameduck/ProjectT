// Fill out your copyright notice in the Description page of Project Settings.


#include "Input.h"
#include "InputAction.h"
#include "InputMappingContext.h"

UGameInput::UGameInput()
{
	static ConstructorHelpers::FObjectFinder<UInputMappingContext> Context(TEXT("/Script/EnhancedInput.InputMappingContext'/Game/Blueprints/Input/IMC_Default.IMC_Default'"));
	if (Context.Succeeded())
		mContext = Context.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> Move(TEXT("/Script/EnhancedInput.InputAction'/Game/Blueprints/Input/IA_Move.IA_Move'"));
	if (Move.Succeeded())
		mMove = Move.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> Rotation(TEXT("/Script/EnhancedInput.InputAction'/Game/Blueprints/Input/IA_Rotation.IA_Rotation'"));
	if (Rotation.Succeeded())
		mRotation = Rotation.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> Attack(TEXT("/Script/EnhancedInput.InputAction'/Game/Blueprints/Input/IA_Attack.IA_Attack'"));
	if (Attack.Succeeded())
		mAttack = Attack.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> Skill_Q(TEXT("/Script/EnhancedInput.InputAction'/Game/Blueprints/Input/IA_Skill_Q.IA_Skill_Q'"));
	if (Skill_Q.Succeeded())
		mSkill_Q = Skill_Q.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> Skill_E(TEXT("/Script/EnhancedInput.InputAction'/Game/Blueprints/Input/IA_Skill_E.IA_Skill_E'"));
	if (Skill_E.Succeeded())
		mSkill_E = Skill_E.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> Skill_R(TEXT("/Script/EnhancedInput.InputAction'/Game/Blueprints/Input/IA_Skill_R.IA_Skill_R'"));
	if (Skill_R.Succeeded())
		mSkill_R = Skill_R.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> Skill_RMB(TEXT("/Script/EnhancedInput.InputAction'/Game/Blueprints/Input/IA_Skill_RMB.IA_Skill_RMB'"));
	if (Skill_RMB.Succeeded())
		mSkill_RMB = Skill_RMB.Object;
}
