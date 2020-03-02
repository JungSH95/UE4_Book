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
	
protected:
	virtual void BeginPlay() override;

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

/*
폰을 조종하기 위한 입력 로직은 폰 클래스에 구현하는 것이 일반적
언리얼 엔진 입력 시스템 : 입력 -> 플레이어 컨트롤러 -> 폰 (플레이어 컨트롤러를 거쳐서 폰에 전달)

플레이어 컨트롤러에 특정 입력을 처리하는 코드를 구현하면 해당 입력은 폰에게 전달되지 않는다.
-> 플레이어 컨트롤러에서 처리 & 필터링

protected:
	virtual void SetupInputComponent() override;
private:
	void LeftRight(float NewAxisValue);
*/
};
