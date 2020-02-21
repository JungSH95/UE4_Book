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
		���� �÷��̾� ��Ʈ�ѷ��� �����Ǵ� ������ �� ������
		PostInitializeComponents �Լ��� �ľ��� �� �ִ�.
		���Ǹ� �����ϴ� ������ �÷��̾� ��Ʈ�ѷ��� Possess,
		���� PossessedBy �Լ��� �ľ� �����ϴ�.
	*/

	virtual void PostInitializeComponents() override;

	// UE4 4.22 �������� Possess(APawn* aPawn)�� ����� ���� �� ���� �Ǿ���.
	// �ذ�� OnPossess���� ��� �����ϴ�.
	virtual void OnPossess(APawn* aPawn) override;

/*
���� �����ϱ� ���� �Է� ������ �� Ŭ������ �����ϴ� ���� �Ϲ���
�𸮾� ���� �Է� �ý��� : �Է� -> �÷��̾� ��Ʈ�ѷ� -> �� (�÷��̾� ��Ʈ�ѷ��� ���ļ� ���� ����)

�÷��̾� ��Ʈ�ѷ��� Ư�� �Է��� ó���ϴ� �ڵ带 �����ϸ� �ش� �Է��� ������ ���޵��� �ʴ´�.
-> �÷��̾� ��Ʈ�ѷ����� ó�� & ���͸�

protected:
	virtual void SetupInputComponent() override;
private:
	void LeftRight(float NewAxisValue);
*/
};
