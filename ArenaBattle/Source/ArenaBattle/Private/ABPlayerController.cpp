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
	플레이어 컨트롤러에서는 게임에서 플레이어의 입력과 출력되는 화면을 책임진다.

	아래의 함수를 통해서 플레이어 컨트롤러에게 UI를 배제하고 게임에게만 입력을 전달하도록 명령을 내린다.
	-> 게임 테스트 편하게 하기 위해서
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

	// HUD 위젯과 플레이어 스테이트를 연결
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

// bShowMouseCursor가 True이면 마우스 커서가 보인다.
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
	// 결과창에서 데이터 값을 사용하기위해 ABGameState를 바인딩
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
	// 플레이어 컨트롤러에서 LeftRight를 처리하면 폰에게 넘어가지 않는다.
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