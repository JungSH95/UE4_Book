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

	// TryGetPawnOwner 함수로 유효한 Pawn 오브젝트의 포인터를 받아오면
	// 여기서 속력을 구해 CurrentPawnSpeed에 저장한다.
	auto Pawn = TryGetPawnOwner();

	// 유효하지 않으면 함수 종료
	if (!::IsValid(Pawn)) return;

	// 살아 있다면
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

	// 몽타주가 재생중이 아닐 경우
	
	// ABCharacter Class에서 IsAttacking를 통해 체크하므로 필요 없어짐
	//if (!Montage_IsPlaying(AttackMontage))

	Montage_Play(AttackMontage, 1.0f);
}

void UABAnimInstance::JumpToAttackMontageSection(int32 NewSection)
{
	ABCHECK(!IsDead);

	ABCHECK(Montage_IsPlaying(AttackMontage));
	Montage_JumpToSection(GetAttackMontageSectionName(NewSection), AttackMontage);
}

// 멀티캐스트 델리게이트에 등록된 모든 함수를 호출하는 멀티캐스트 델리게이트 명령은 Broadcast이다.
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