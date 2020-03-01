// Fill out your copyright notice in the Description page of Project Settings.


#include "ABAIController.h"

/*
#include "NavigationSystem.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
*/

#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "BehaviorTree/BlackboardComponent.h"

const FName AABAIController::HomePosKey(TEXT("HomePos"));
const FName AABAIController::PatrolPosKey(TEXT("PatrolPos"));
const FName AABAIController::TargetKey(TEXT("Target"));

AABAIController::AABAIController()
{
	//RepeatInterval = 3.0f;

	static ConstructorHelpers::FObjectFinder<UBlackboardData> BBObject(TEXT("/Game/Book/AI/BB_ABCharacter.BB_ABCharacter"));
	if (BBObject.Succeeded())
	{
		BBAsset = BBObject.Object;
	}

	static ConstructorHelpers::FObjectFinder<UBehaviorTree> BTObject(TEXT("/Game/Book/AI/BT_ABCharacter.BT_ABCharacter"));
	if (BTObject.Succeeded())
	{
		BTAsset = BTObject.Object;
	}
}

void AABAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	/*
	// 타이머 설정
	GetWorld()->GetTimerManager().SetTimer(RepeatTimerHandle, this, &AABAIController::OnRepeatTimer,
		RepeatInterval, true);
	*/

	if (UseBlackboard(BBAsset, Blackboard))
	{
		Blackboard->SetValueAsVector(HomePosKey, GetPawn()->GetActorLocation());

		if (!RunBehaviorTree(BTAsset))
			ABLOG(Error, TEXT("AIController couldn't run behavior tree !"));
	}
}

/*
void AABAIController::OnUnPossess()
{
	Super::OnUnPossess();

	// 타이머 삭제
	GetWorld()->GetTimerManager().ClearTimer(RepeatTimerHandle);
}

void AABAIController::OnRepeatTimer()
{
	auto CurrentPawn = GetPawn();
	ABCHECK(nullptr != CurrentPawn);

	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(GetWorld());
	if (nullptr == NavSystem)
		return;

	FNavLocation NextLocation;
	// GetRandomPointInNavigableRadius() : 이동 가능한 목적지를 랜덤으로 가져오는 함수
	if (NavSystem->GetRandomPointInNavigableRadius(FVector::ZeroVector, 500.0f, NextLocation))
	{
		// SimpleMoveToLocation() : 목적지로 폰을 이동시키는 함수
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, NextLocation.Location);
		ABLOG(Warning, TEXT("Next Location : %s"), *NextLocation.Location.ToString());
	}
}
*/
