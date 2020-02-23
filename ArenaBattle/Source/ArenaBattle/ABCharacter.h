// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "GameFramework/Character.h"
#include "ABCharacter.generated.h"

/*
�� ���� ��ӹ��� Ŭ�������� ������
: ĳ���� �����Ʈ ������Ʈ�� ����Ѵٴ� ��

UFloatingPawnMovement�� ���� ������ ����
: ������ ���� �߷��� �ݿ��� �������� ����
: �پ��� �������� ������ �� �ִ�. �����ӿ� ���� �� ���� ������ ������
: ��Ƽ �÷��� ��Ʈ��ũ ȯ�濡�� ĳ���͵��� �������� �ڵ����� ����ȭ
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
