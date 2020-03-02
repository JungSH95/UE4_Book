// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//#include "CoreMinimal.h"
//#include "EngineMinimal.h"

#include "ArenaBattle.h"

// 언리얼 엔진에서는 움직이라는 요소를 분리해 액터와 별도로 관리하도록 프레임워크를 구성
#include "GameFramework/RotatingMovementComponent.h"

#include "GameFramework/Actor.h"
#include "Fountain.generated.h"

UCLASS()
class ARENABATTLE_API AFountain : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFountain();

protected:
	// Called when the game starts or when spawned, 액터가 게임에 참여할 때 호출
	virtual void BeginPlay() override;

	// 게임에서 더 이상 액터의 역할이 없어지면, 액터는 게임에서 퇴장하고 메모리 소멸
	// 액터가 게임에서 퇴장할 때 호출
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// 액터에 속한 모드 컴포넌트의 세팅이 완료되면 호출
	virtual void PostInitializeComponents() override;

public:	
	// Called every frame, 매 프레임마다 호출
	virtual void Tick(float DeltaTime) override;

	// VisibleAnywhere : 컴포넌트 속성을 편집하기 위해 필요한 키워드
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent *Body;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent *Water;

	UPROPERTY(VisibleAnywhere)
	UPointLightComponent *Light;

	UPROPERTY(VisibleAnywhere)
	UParticleSystemComponent *Splash;

	UPROPERTY(VisibleAnywhere)
	URotatingMovementComponent* Movement;

	UPROPERTY(EditAnywhere, Category=ID)
	int32 ID;

private:
	// 프로그래밍 영역에서 캡슐화 (private : 정보은닉)
	UPROPERTY(EditAnywhere, Category = Stat, Meta = (AllowPrivateAccess = true))
	float RotateSpeed;
};
