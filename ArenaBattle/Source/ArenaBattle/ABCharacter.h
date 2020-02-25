// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "GameFramework/Character.h"
#include "ABCharacter.generated.h"

/*
폰 모델을 상속받은 클래스와의 차이점
: 캐릭터 무브먼트 컴포넌트를 사용한다는 것

UFloatingPawnMovement에 비해 가지는 장점
: 점프와 같은 중력을 반영한 움직임을 제공
: 다양한 움직임을 설정할 수 있다. 움직임에 대한 더 많은 정보를 전달함
: 멀티 플레이 네트워크 환경에서 캐릭터들의 움직임을 자동으로 동기화
*/

UCLASS()
class ARENABATTLE_API AABCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AABCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	enum class EControlMode
	{
		GTA,
		DIABLO
	};

	void SetControlMode(EControlMode NewControlMode);
	EControlMode CurrentControlMode = EControlMode::GTA;

	FVector DirctionToMove = FVector::ZeroVector;

	// 캐릭터 컨트롤 변경할 때마다 부드럽게 변경하기 위한 기능 추가
	float ArmLengthTo = 0.0f;
	FRotator ArmRotationTo = FRotator::ZeroRotator;
	float ArmLengthSpeed = 0.0f;
	float ArmRotationSpeed = 0.0f;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	virtual void PostInitializeComponents() override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
		class AController* EventInstigator, AActor* DamageCauser) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleAnywhere, Category = Weapon)
	USkeletalMeshComponent* Weapon;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	UCameraComponent* Camera;

private:
	void UpDown(float NewAxisValue);
	void LeftRight(float NewAxisValue);
	void LookUp(float NewAxisValue);
	void Turn(float NewAxisValue);

	void ViewChange();
	void Attack();

	/*
	언리얼에서 델리게이트는 C++ 객체에만 사용할 수 있는 델리게이트와
	C++, 블루프린트 객체 모두 사용할 수 있는 델리게이트로 나뉜다.

	블루프린트 오브젝트 -> 멤버 함수에 대한 정보를 저장하고 로딩하는 직렬화 메커니즘이
						   들어있기 때문에 일반 C++ 언어가 관리하는 방법으로 멤버 함수를
						   관리할 수 없다.
	=> 때문에 블루프린트와 관련된 C++ 함수는 모두 UFUNCTION 매크로를 사용해야 한다.

	: 이렇게 블루프린트 객체와도 연동하는 델리게이트를 언리얼 엔진에서는
	  다이내믹 델리게이트라고 한다.
	*/
	UFUNCTION()
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	// 공격 시작과 끝날 때 관련 속성을 지정하는 함수
	void AttackStartComboState();
	void AttackEndComboState();

	void AttackCheck();
private:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack,
		Meta = (AllowPrivateAccess = true))
	bool IsAttacking;

	// 다음 콤보로의 이동 가능 여부
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack,
		Meta = (AllowPrivateAccess = true))
	bool CanNextCombo;

	// 콤보 입력 여부
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack,
		Meta = (AllowPrivateAccess = true))
	bool IsComboInputOn;

	// 현재 콤보 카운터
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack,
		Meta = (AllowPrivateAccess = true))
	int32 CurrentCombo;

	// 콤보 카운터의 최대치
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack,
		Meta = (AllowPrivateAccess = true))
	int32 MaxCombo;

	// 자주 사용하므로 멤버 변수로 선언
	UPROPERTY()
	class UABAnimInstance* ABAnim;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack,
		Meta = (AllowPrivateAccess = true))
	float AttackRange;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack,
		Meta = (AllowPrivateAccess = true))
	float AttackRadius;
};
