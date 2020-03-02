// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//#include "CoreMinimal.h"
//#include "EngineMinimal.h"

#include "ArenaBattle.h"

// �𸮾� ���������� �����̶�� ��Ҹ� �и��� ���Ϳ� ������ �����ϵ��� �����ӿ�ũ�� ����
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
	// Called when the game starts or when spawned, ���Ͱ� ���ӿ� ������ �� ȣ��
	virtual void BeginPlay() override;

	// ���ӿ��� �� �̻� ������ ������ ��������, ���ʹ� ���ӿ��� �����ϰ� �޸� �Ҹ�
	// ���Ͱ� ���ӿ��� ������ �� ȣ��
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// ���Ϳ� ���� ��� ������Ʈ�� ������ �Ϸ�Ǹ� ȣ��
	virtual void PostInitializeComponents() override;

public:	
	// Called every frame, �� �����Ӹ��� ȣ��
	virtual void Tick(float DeltaTime) override;

	// VisibleAnywhere : ������Ʈ �Ӽ��� �����ϱ� ���� �ʿ��� Ű����
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
	// ���α׷��� �������� ĸ��ȭ (private : ��������)
	UPROPERTY(EditAnywhere, Category = Stat, Meta = (AllowPrivateAccess = true))
	float RotateSpeed;
};
