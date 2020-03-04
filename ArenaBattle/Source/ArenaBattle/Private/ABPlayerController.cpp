// Fill out your copyright notice in the Description page of Project Settings.


#include "ABPlayerController.h"
#include "ABHUDWidget.h"
#include "ABPlayerState.h"
#include "ABCharacter.h"

AABPlayerController::AABPlayerController()
{
	static ConstructorHelpers::FClassFinder<UABHUDWidget> UI_HUD_C(TEXT("/Game/Book/UI/UI_HUD"));
	if (UI_HUD_C.Succeeded())
	{
		HUDWidgetClass = UI_HUD_C.Class;
	}
}

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

	HUDWidget = CreateWidget<UABHUDWidget>(this, HUDWidgetClass);
	ABCHECK(nullptr != HUDWidget);
	HUDWidget->AddToViewport(1);

	// HUD ������ �÷��̾� ������Ʈ�� ����
	ABPlayerState = Cast<AABPlayerState>(PlayerState);
	ABCHECK(nullptr != ABPlayerState);
	HUDWidget->BindPlayerState(ABPlayerState);
	ABPlayerState->OnPlayerStateChanged.Broadcast();
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

UABHUDWidget* AABPlayerController::GetHUDWidget() const
{
	return HUDWidget;
}

void AABPlayerController::NPCKill(class AABCharacter* KilledNPC) const
{
	ABPlayerState->AddExp(KilledNPC->GetExp());
}

void AABPlayerController::AddGameScore() const
{
	ABPlayerState->AddGameScore();
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
