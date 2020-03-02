// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_IsInAttackRange.generated.h"

/**
 * 
 */
UCLASS()
class ARENABATTLE_API UBTDecorator_IsInAttackRange : public UBTDecorator
{
	GENERATED_BODY()

public:
	UBTDecorator_IsInAttackRange();

protected:
	// ���ڷ����� Ŭ������ CalculateRawConditionValue �Լ��� ��ӹ޾�
	// ���ϴ� ������ �޼��ƴ����� �ľ��ϵ��� ����ƴ�.
	// �� �Լ��� const�� ����� ���ڷ����� Ŭ������ ��� ���� ���� ������ �� ����.
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, 
		uint8* NodeMemory) const override;
};
