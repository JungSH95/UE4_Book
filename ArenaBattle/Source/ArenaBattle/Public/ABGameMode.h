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

	AABGameMode();

public:
	// �÷��̾ �α����� �Ϸ��ϸ� �̺�Ʈ �Լ� ȣ��
	virtual void PostLogin(APlayerController* NewPlayer) override;
};
