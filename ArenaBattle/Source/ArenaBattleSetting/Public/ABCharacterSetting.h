// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ABCharacterSetting.generated.h"

/**
 *  ABCharacterSetting�� ������ ����� ĳ���� �ּ��� ����� ����
 */

// config=ArenaBattle �̶�� �������� ����, �𸮾� ������ �ʱ�ȭ �ܰ迡��
// Config ������ ��ġ�� DefaultArenaBattle.ini ������ �о�鿩
// ABCharacterSetting�� CharacterAssets ���� �����Ѵ�.
UCLASS(config=ArenaBattle)
class ARENABATTLESETTING_API UABCharacterSetting : public UObject
{
	GENERATED_BODY()
	
public:
	UABCharacterSetting();

	UPROPERTY(config)
	TArray<FSoftObjectPath> CharacterAssets;
};
