// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "Components/ActorComponent.h"
#include "ABCharacterStatComponent.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnHPIsZeroDelegate);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ARENABATTLE_API UABCharacterStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UABCharacterStatComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	/*
	������ PostInitializeComponents�� �����ϴ� ������Ʈ�� �Լ� InitializeComponent()
	�� �Լ��� ������ PostInitializeComponents �Լ��� ȣ��Ǳ� �ٷ� ���� ȣ���

	�� �Լ��� ����� ������Ʈ�� �ʱ�ȭ ������ ����.
	�� �Լ��� ȣ��Ƿ��� �����ڿ��� bWantsInitializeComponent ���� true�� ����
	*/
	virtual void InitializeComponent() override;

public:
	void SetNewLevel(int32 NewLevel);
	void SetDamage(float NewDamage);
	float GetAttack();

	// ĳ���Ϳ� �������� ������ �ʵ��� ��������Ʈ�� ���
	FOnHPIsZeroDelegate OnHPIsZero;

private:
	struct FABCharacterData* CurrentStatData = nullptr;

	UPROPERTY(EditInstanceOnly, Category = Stat, Meta = (AllowPrivateAccess = true))
	int32 Level;

	// Transient Ű���带 ����� �ش� �Ӽ��� ����ȭ���� ����
	// ������ ������ ������ ����ǹǷ� �� ���� �����ϴ� ���� �ǹ̰� ����.
	UPROPERTY(Transient, VisibleInstanceOnly, Category = Stat,
		Meta = (AllowPrivateAccess = true))
	float CurremtHP;
};
