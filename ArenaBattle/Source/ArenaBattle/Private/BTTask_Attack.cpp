// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_Attack.h"

#include "ABAIController.h"
#include "ABCharacter.h"

UBTTask_Attack::UBTTask_Attack()
{
	// 노드의 Tick 기능 활성화
	bNotifyTick = true;
	IsAttacking = false;
}

EBTNodeResult::Type UBTTask_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp,
	uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	auto ABCharacter = Cast<AABCharacter>(OwnerComp.GetAIOwner()->GetPawn());
	if (nullptr == ABCharacter)
		return EBTNodeResult::Failed;

	ABCharacter->Attack();
	IsAttacking = true;
	ABCharacter->OnAttackEnd.AddLambda([this]() -> void
	{
		IsAttacking = false;
	});

	/*
	공격 태스크는 공격 애니메이션이 끝날 때 까지 대기해야 하는 지연 태스크이므로
	ExecuteTask의 결과 값을 InProgress 일단 반환하고 공격이 끝났을때 끝났다고 알려줘야함.
	-> 이를 알려주는 함수가 FinishLatentTask다.
	*/
	return EBTNodeResult::InProgress;
}

void UBTTask_Attack::TickTask(UBehaviorTreeComponent& OwnerComp,
	uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	if (!IsAttacking)
	{
		// 테스크가 끝났음을 알려주는 함수
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}