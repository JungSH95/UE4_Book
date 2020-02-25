// Fill out your copyright notice in the Description page of Project Settings.


#include "ABAnimInstance.h"

UABAnimInstance::UABAnimInstance()
{
	CurrentPawnSpeed = 0.0f;
	IsInAir = false;
	IsDead = false;

	static ConstructorHelpers::FObjectFinder<UAnimMontage> ATTACK_MONTAGE(TEXT("/Game/Book/Animations/SK_Mannequin_Skeleton_Montage.SK_Mannequin_Skeleton_Montage"));
	if (ATTACK_MONTAGE.Succeeded())
		AttackMontage = ATTACK_MONTAGE.Object;
}

void UABAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	// TryGetPawnOwner �Լ��� ��ȿ�� Pawn ������Ʈ�� �����͸� �޾ƿ���
	// ���⼭ �ӷ��� ���� CurrentPawnSpeed�� �����Ѵ�.
	auto Pawn = TryGetPawnOwner();

	// ��ȿ���� ������ �Լ� ����
	if (!::IsValid(Pawn)) return;

	// ��� �ִٸ�
	if (!IsDead)
	{
		CurrentPawnSpeed = Pawn->GetVelocity().Size();

		auto Character = Cast<ACharacter>(Pawn);
		if (Character)
			IsInAir = Character->GetMovementComponent()->IsFalling();
	}
}

void UABAnimInstance::PlayAttackMontage()
{
	ABCHECK(!IsDead);

	// ��Ÿ�ְ� ������� �ƴ� ���
	
	// ABCharacter Class���� IsAttacking�� ���� üũ�ϹǷ� �ʿ� ������
	//if (!Montage_IsPlaying(AttackMontage))

	Montage_Play(AttackMontage, 1.0f);
}

void UABAnimInstance::JumpToAttackMontageSection(int32 NewSection)
{
	ABCHECK(!IsDead);

	ABCHECK(Montage_IsPlaying(AttackMontage));
	Montage_JumpToSection(GetAttackMontageSectionName(NewSection), AttackMontage);
}

// ��Ƽĳ��Ʈ ��������Ʈ�� ��ϵ� ��� �Լ��� ȣ���ϴ� ��Ƽĳ��Ʈ ��������Ʈ ����� Broadcast�̴�.
void UABAnimInstance::AnimNotify_AttackHitCheck()
{
	//ABLOG_S(Warning);
	OnAttackHitCheck.Broadcast();
}

void UABAnimInstance::AnimNotify_NextAttackCheck()
{
	OnNextAttackCheck.Broadcast();
}

FName UABAnimInstance::GetAttackMontageSectionName(int32 Section)
{
	ABCHECK(FMath::IsWithinInclusive<int32>(Section, 1, 4), NAME_None);
	return FName(*FString::Printf(TEXT("Attack%d"), Section));
}