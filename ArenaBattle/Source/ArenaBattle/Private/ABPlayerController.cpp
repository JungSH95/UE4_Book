// Fill out your copyright notice in the Description page of Project Settings.


#include "ABPlayerController.h"
#include "ABHUDWidget.h"
#include "ABPlayerState.h"
#include "ABCharacter.h"
#include "ABGameplayWidget.h"
#include "ABGameplayResultWidget.h"
#include "ABGameState.h"

AABPlayerController::AABPlayerController()
{
	static ConstructorHelpers::FClassFinder<UABHUDWidget> UI_HUD_C(TEXT("/Game/Book/UI/UI_HUD"));
	if (UI_HUD_C.Succeeded())
	{
		HUDWidgetClass = UI_HUD_C.Class;
	}

	static ConstructorHelpers::FClassFinder<UABGameplayWidget> UI_MENU_C(TEXT("/Game/Book/UI/UI_Menu"));
	if (UI_MENU_C.Succeeded())
		MenuWidgetClass = UI_MENU_C.Class;

	static ConstructorHelpers::FClassFinder<UABGameplayResultWidget> UI_RESULT_C(TEXT("/Game/Book/UI/UI_Result"));
	if (UI_RESULT_C.Succeeded())
		ResultWidgetClass = UI_RESULT_C.Class;
}

void AABPlayerController::BeginPlay()
{
	Super::BeginPlay();


	/*
	�÷��̾� ��Ʈ�ѷ������� ���ӿ��� �÷��̾��� �Է°� ��µǴ� ȭ���� å������.

	�Ʒ��� �Լ��� ���ؼ� �÷��̾� ��Ʈ�ѷ����� UI�� �����ϰ� ���ӿ��Ը� �Է��� �����ϵ��� ����� ������.
	-> ���� �׽�Ʈ ���ϰ� �ϱ� ���ؼ�
	*/
	/*
	FInputModeGameOnly InputMode;
	SetInputMode(InputMode);
	*/

	ChangeInputMode(true);

	HUDWidget = CreateWidget<UABHUDWidget>(this, HUDWidgetClass);
	ABCHECK(nullptr != HUDWidget);
	HUDWidget->AddToViewport(1);

	ResultWidget = CreateWidget<UABGameplayResultWidget>(this, ResultWidgetClass);
	ABCHECK(nullptr != ResultWidget);

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

// bShowMouseCursor�� True�̸� ���콺 Ŀ���� ���δ�.
void AABPlayerController::ChangeInputMode(bool bGameMode)
{
	if (bGameMode)
	{
		SetInputMode(GameInputMode);
		bShowMouseCursor = false;
	}
	else
	{
		SetInputMode(UIInputMode);
		bShowMouseCursor = true;
	}
}

void AABPlayerController::ShowResultUI()
{
	// ���â���� ������ ���� ����ϱ����� ABGameState�� ���ε�
	auto ABGameState = Cast<AABGameState>(UGameplayStatics::GetGameState(this));
	ABCHECK(nullptr != ABGameState);
	ResultWidget->BindGameState(ABGameState);

	ResultWidget->AddToViewport();
	ChangeInputMode(false);
}

void AABPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	//InputComponent->BindAxis(TEXT("LeftRight"), this, &AABPlayerController::LeftRight);

	InputComponent->BindAction(TEXT("GamePause"), EInputEvent::IE_Pressed, this,
		&AABPlayerController::OnGamePause);
}

/*
void AABPlayerController::LeftRight(float NewAxisValue)
{
	// �÷��̾� ��Ʈ�ѷ����� LeftRight�� ó���ϸ� ������ �Ѿ�� �ʴ´�.
}
*/

//void AABPlayerController::ShowResultUI()
//{
//	auto ABGameState = Cast<AABGameState>(UGameplayStatics::GetGameState(this));
//	ABCHECK(nullptr != ABGameState);
//	ResultWidget->BindGameState(ABGameState);
//
//	ResultWidget->AddToViewport();
//	ChangeInputMode(false);
//}

void AABPlayerController::OnGamePause()
{
	MenuWidget = CreateWidget<UABGameplayWidget>(this, MenuWidgetClass);
	ABCHECK(nullptr != MenuWidget);
	MenuWidget->AddToViewport(3);

	SetPause(true);
	ChangeInputMode(false);
}