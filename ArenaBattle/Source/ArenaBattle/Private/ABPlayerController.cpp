// Fill out your copyright notice in the Description page of Project Settings.


#include "ABPlayerController.h"

void AABPlayerController::BeginPlay()
{
	Super::BeginPlay();

	/*
	�÷��̾� ��Ʈ�ѷ������� ���ӿ��� �÷��̾��� �Է°� ��µǴ� ȭ���� å������.

	�Ʒ��� �Լ��� ���ؼ� �÷��̾� ��Ʈ�ѷ����� UI�� �����ϰ� ���ӿ��Ը� �Է��� �����ϵ��� ����� ������.
	-> ���� �׽�Ʈ ���ϰ� �ϱ� ���ؼ�
	*/
	FInputModeGameOnly InputMode;
	SetInputMode(InputMode);
}

void AABPlayerController::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	ABLOG_S(Warning);
}

void AABPlayerController::OnPossess(APawn* aPawn)
{
	ABLOG_S(Warning);
	Super::OnPossess(aPawn);
}

/*
void AABPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	InputComponent->BindAxis(TEXT("LeftRight"), this, &AABPlayerController::LeftRight);
}

void AABPlayerController::LeftRight(float NewAxisValue)
{
	// �÷��̾� ��Ʈ�ѷ����� LeftRight�� ó���ϸ� ������ �Ѿ�� �ʴ´�.
}
*/
