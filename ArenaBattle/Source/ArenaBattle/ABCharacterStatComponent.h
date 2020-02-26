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
	액터의 PostInitializeComponents에 대응하는 컴포넌트의 함수 InitializeComponent()
	이 함수는 액터의 PostInitializeComponents 함수가 호출되기 바로 전에 호출됨

	이 함수를 사용해 컴포넌트의 초기화 로직을 구현.
	이 함수가 호출되려면 생성자에서 bWantsInitializeComponent 값을 true로 설정
	*/
	virtual void InitializeComponent() override;

public:
	void SetNewLevel(int32 NewLevel);
	void SetDamage(float NewDamage);
	float GetAttack();

	// 캐릭터에 의존성을 가지지 않도록 델리게이트를 사용
	FOnHPIsZeroDelegate OnHPIsZero;

private:
	struct FABCharacterData* CurrentStatData = nullptr;

	UPROPERTY(EditInstanceOnly, Category = Stat, Meta = (AllowPrivateAccess = true))
	int32 Level;

	// Transient 키워드를 사용해 해당 속성을 직렬화에서 제외
	// 게임을 시작할 때마다 변경되므로 이 값을 보관하는 것은 의미가 없다.
	UPROPERTY(Transient, VisibleInstanceOnly, Category = Stat,
		Meta = (AllowPrivateAccess = true))
	float CurremtHP;
};
