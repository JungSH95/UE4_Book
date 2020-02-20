// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "GameFramework/PlayerController.h"
#include "ABPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class ARENABATTLE_API AABPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	/*
		폰과 플레이어 컨트롤러가 생성되는 시점은 각 액터의
		PostInitializeComponents 함수로 파악할 수 있다.
		빙의를 진행하는 시점은 플레이어 컨트롤러의 Possess,
		폰의 PossessedBy 함수로 파악 가능하다.
	*/

	virtual void PostInitializeComponents() override;

	// UE4 4.22 버전부터 Possess(APawn* aPawn)는 상속을 받을 수 없게 되었다.
	// 해결법 OnPossess으로 사용 가능하다.
	virtual void OnPossess(APawn* aPawn) override;
};
