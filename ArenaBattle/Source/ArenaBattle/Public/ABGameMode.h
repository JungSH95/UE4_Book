// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "GameFramework/GameModeBase.h"
#include "ABGameMode.generated.h"

/**
 * 
 */
UCLASS()
class ARENABATTLE_API AABGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AABGameMode();

	virtual void PostInitializeComponents() override;
	// 플레이어가 로그인을 완료하면 이벤트 함수 호출
	virtual void PostLogin(APlayerController* NewPlayer) override;
	void AddScore(class AABPlayerController *ScoredPlayer);
	int32 GetScore() const;

private:
	UPROPERTY()
	class AABGameState* ABGameState;
};
