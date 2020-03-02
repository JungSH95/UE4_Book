// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ABCharacterSetting.generated.h"

/**
 *  ABCharacterSetting은 앞으로 사용할 캐릭터 애셋의 목록을 보관
 */

// config=ArenaBattle 이라는 설정으로 인해, 언리얼 엔진은 초기화 단계에서
// Config 폴더에 위치한 DefaultArenaBattle.ini 파일을 읽어들여
// ABCharacterSetting의 CharacterAssets 값을 설정한다.
UCLASS(config=ArenaBattle)
class ARENABATTLESETTING_API UABCharacterSetting : public UObject
{
	GENERATED_BODY()
	
public:
	UABCharacterSetting();

	UPROPERTY(config)
	TArray<FSoftObjectPath> CharacterAssets;
};
