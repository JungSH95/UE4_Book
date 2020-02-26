// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"

#include "Engine/DataTable.h"

#include "Engine/GameInstance.h"
#include "ABGameInstance.generated.h"

/*
파일을 언리얼 엔진에서 불러들이기 위해 테이블 데이터의 각 열의 이름과
유형이 동일한 구조체를 선언해야 한다.
-> 언리얼 엔진에서 제공하는 FTableRowBase 구조체를 상속받은 FABCharacterData라는
이름의 구조체를 게임 인스턴스의 헤더에 선언한다.
 */
USTRUCT(BlueprintType)
struct FABCharacterData : public FTableRowBase
{
	GENERATED_BODY()

public:
	FABCharacterData() : Level(1), MaxHP(100.0f), Attack(10.0f), DropExp(10), NextExp(30) {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	int32 Level;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	float MaxHP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	float Attack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	int32 DropExp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	int32 NextExp;
};

UCLASS()
class ARENABATTLE_API UABGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	UABGameInstance();

	virtual void Init() override;

	FABCharacterData* GetABCharacterData(int32 Level);

private:
	UPROPERTY()
	class UDataTable* ABCharacterTable;
};
