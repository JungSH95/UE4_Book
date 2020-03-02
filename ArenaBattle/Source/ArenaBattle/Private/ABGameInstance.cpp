// Fill out your copyright notice in the Description page of Project Settings.


#include "ABGameInstance.h"

UABGameInstance::UABGameInstance()
{
	FString CharacterDataPath = TEXT("/Game/Book/GameData/ABCharacterData.ABCharacterData");
	static ConstructorHelpers::FObjectFinder<UDataTable> DT_ABCHARACTER(*CharacterDataPath);
	ABCHECK(DT_ABCHARACTER.Succeeded());
	ABCharacterTable = DT_ABCHARACTER.Object;
	ABCHECK(ABCharacterTable->GetRowMap().Num() > 0);
}

/*
게임이 시작되기까지의 과정
1. 게임 앱의 초기화 : UABGameInstance::Init()
2. 레벨에 속한 액터의 초기화 : AActor::PostInitializeComponents()
3. 플레이어의 로그인 : AGameMode::PostLogin()
4. 게임의 시작 : AGameMode::StartPlay()   AActor::BeginPlay()
*/
void UABGameInstance::Init()
{
	Super::Init();
	//ABLOG(Warning, TEXT("DropExp of Level 20 ABCharacter : %d"), GetABCharacterData(20)->DropExp);
}

FABCharacterData* UABGameInstance::GetABCharacterData(int32 Level)
{
	return ABCharacterTable->FindRow<FABCharacterData>(*FString::FromInt(Level), TEXT(""));
}