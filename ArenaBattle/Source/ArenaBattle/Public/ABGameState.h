// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "GameFramework/GameStateBase.h"
#include "ABGameState.generated.h"

/**
 *  멀티 플레이를 고려한다면 : 게임 스테이트 클래스를 이용
	싱글 플레이는 플레이어 스테이트에서 관리해도 무방
 */
UCLASS()
class ARENABATTLE_API AABGameState : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	AABGameState();

public:
	int32 GetTotalGameScore() const;
	void AddGameScore();

	void SetGameCleared();
	bool IsGameCleared() const;

private:
	UPROPERTY(Transient)
	int32 TotalGameScore;

	UPROPERTY(Transient)
	bool bGameCleared;
};
