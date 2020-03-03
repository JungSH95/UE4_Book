// Fill out your copyright notice in the Description page of Project Settings.


#include "ABGameMode.h"
#include "ABCharacter.h"
#include "ABPlayerController.h"
#include "ABPlayerState.h"

AABGameMode::AABGameMode()
{
	// ���� ��忡 ���� ����
	DefaultPawnClass = AABCharacter::StaticClass();

	// �÷��̾� ��Ʈ�ѷ� ����
	PlayerControllerClass = AABPlayerController::StaticClass();

	PlayerStateClass = AABPlayerState::StaticClass();
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