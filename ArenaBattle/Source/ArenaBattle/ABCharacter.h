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

	void SetControlMode(int32 ControlMode);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	UCameraComponent* Camera;

private:
	void UpDown(float NewAxisValue);
	void LeftRight(float NewAxisValue);
	void LookUp(float NewAxisValue);
	void Turn(float NewAxisValue);
};
