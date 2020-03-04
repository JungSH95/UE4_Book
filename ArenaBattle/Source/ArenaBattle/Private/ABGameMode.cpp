// Fill out your copyright notice in the Description page of Project Settings.


#include "ABGameMode.h"
#include "ABCharacter.h"
#include "ABPlayerController.h"
#include "ABPlayerState.h"
#include "ABGameState.h"

AABGameMode::AABGameMode()
{
	// 게임 모드에 폰을 지정
	DefaultPawnClass = AABCharacter::StaticClass();

	// 플레이어 컨트롤러 적용
	PlayerControllerClass = AABPlayerController::StaticClass();

	// 캐릭터 정보
	PlayerStateClass = AABPlayerState::StaticClass();

	GameStateClass = AABGameState::StaticClass();
}

void AABGameMode::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	ABGameState = Cast<AABGameState>(GameState);
}

void AABGameMode::PostLogin(APlayerController* NewPlayer)
{
	//ABLOG(Warning, TEXT("PostLogin Begin"));
	Super::PostLogin(NewPlayer);
	//ABLOG(Warning, TEXT("PostLogin End"));

	auto ABPlayerState = Cast<AABPlayerState>(NewPlayer->PlayerState);
	ABCHECK(nullptr != ABPlayerState);
	ABPlayerState->InitPlayerData();
}

void AABGameMode::AddScore(class AABPlayerController *ScoredPlayer)
{
	// GetPlayerControllerIterator() 함수를 통해 현재 게임에 참여 중인 플레이어 컨트롤러의 목록을
	// 얻어올 수 있다.
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		const auto ABPlayerController = Cast<AABPlayerController>(It->Get());
		if ((nullptr != ABPlayerController) && (ScoredPlayer == ABPlayerController))
		{
			ABPlayerController->AddGameScore();
			break;
		}
	}

	ABGameState->AddGameScore();
}

int32 AABGameMode::GetScore() const
{
	return ABGameState->GetTotalGameScore();
}