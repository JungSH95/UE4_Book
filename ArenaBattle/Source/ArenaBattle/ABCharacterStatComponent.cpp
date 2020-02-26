// Fill out your copyright notice in the Description page of Project Settings.


#include "ABCharacterStatComponent.h"
#include "ABGameInstance.h"

// Sets default values for this component's properties
UABCharacterStatComponent::UABCharacterStatComponent()
{
	// ������ ����� ��쿡�� ó���� ���̹Ƿ� �ʿ� ����
	PrimaryComponentTick.bCanEverTick = false;

	// InitializeComponent() �Լ��� ȣ���ϱ� ���ؼ��� ��������� ��
	bWantsInitializeComponent = true;

	Level = 1;
}


// Called when the game starts
void UABCharacterStatComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UABCharacterStatComponent::InitializeComponent()
{
	Super::InitializeComponent();
	SetNewLevel(Level);
}

void UABCharacterStatComponent::SetNewLevel(int32 NewLevel)
{
	auto ABGameInstance = Cast<UABGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));

	ABCHECK(nullptr != ABGameInstance);
	CurrentStatData = ABGameInstance->GetABCharacterData(NewLevel);
	
	if (nullptr != CurrentStatData)
	{
		Level = NewLevel;
		CurremtHP = CurrentStatData->MaxHP;
	}
	else
		ABLOG(Error, TEXT("Level (%d) data doesn;t exist"), NewLevel);
}

void UABCharacterStatComponent::SetDamage(float NewDamage)
{
	ABCHECK(nullptr != CurrentStatData);
	CurremtHP = FMath::Clamp<float>(CurremtHP - NewDamage, 0.0f, CurrentStatData->MaxHP);

	// ���
	if (CurremtHP <= 0.0f)
		OnHPIsZero.Broadcast();
}

float UABCharacterStatComponent::GetAttack()
{
	ABCHECK(nullptr != CurrentStatData, 0.0f);
	return CurrentStatData->Attack;
}