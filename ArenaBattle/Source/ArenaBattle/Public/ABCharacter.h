// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArenaBattle.h"
#include "GameFramework/Character.h"
#include "ABCharacter.generated.h"

/*
�� ���� ��ӹ��� Ŭ�������� ������
: ĳ���� �����Ʈ ������Ʈ�� ����Ѵٴ� ��

UFloatingPawnMovement�� ���� ������ ����
: ������ ���� �߷��� �ݿ��� �������� ����
: �پ��� �������� ������ �� �ִ�. �����ӿ� ���� �� ���� ������ ������
: ��Ƽ �÷��� ��Ʈ��ũ ȯ�濡�� ĳ���͵��� �������� �ڵ����� ����ȭ
*/

DECLARE_MULTICAST_DELEGATE(FOnAttackEndDelegate);

UCLASS()
class ARENABATTLE_API AABCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AABCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	enum class EControlMode
	{
		GTA,
		DIABLO,
		NPC
	};

	void SetControlMode(EControlMode NewControlMode);
	EControlMode CurrentControlMode = EControlMode::GTA;

	FVector DirctionToMove = FVector::ZeroVector;

	// ĳ���� ��Ʈ�� ������ ������ �ε巴�� �����ϱ� ���� ��� �߰�
	float ArmLengthTo = 0.0f;
	FRotator ArmRotationTo = FRotator::ZeroRotator;
	float ArmLengthSpeed = 0.0f;
	float ArmRotationSpeed = 0.0f;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	virtual void PostInitializeComponents() override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
		class AController* EventInstigator, AActor* DamageCauser) override;

	virtual void PossessedBy(AController* NewController) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	bool CanSetWeapon();
	void SetWeapon(class AABWeapon* NewWeapon);

	UPROPERTY(VisibleAnywhere, Category = Weapon)
	class AABWeapon* CurrentWeapon;

	UPROPERTY(VisibleAnywhere, Category = Stat)
	class UABCharacterStatComponent* CharacterStat;

	UPROPERTY(VisibleAnywhere, Category = Weapon)
	USkeletalMeshComponent* Weapon;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	UCameraComponent* Camera;

	// �ش� ���� �� ������ ���� �� 'Ȯ���� �� ���� �ܺ� ����'��� �޽����� ���� �߻�
	// ���� : ���� ������Ʈ ������ UI ���õ� ���� ����� �������� �ʾұ� ����
	// -> ���� ����ϴ� �𸮾� ������ ������ ArenaBattle.Build.cs ���Ͽ��� Ȯ�� ����
	// PublicDependencyModuleNames.AddRange�� "UMG" �߰� �ϸ� ���� ������Ʈ�� ��� ����
	UPROPERTY(VisibleAnywhere, Category = UI)
	class UWidgetComponent* HPBarWidget;

	// AI ��Ʈ�ѷ������� ���� ����� ���� �� �ֵ��� ���ٱ��� public���� ����
	void Attack();
	FOnAttackEndDelegate OnAttackEnd;

private:
	void UpDown(float NewAxisValue);
	void LeftRight(float NewAxisValue);
	void LookUp(float NewAxisValue);
	void Turn(float NewAxisValue);

	void ViewChange();

	/*
	�𸮾󿡼� ��������Ʈ�� C++ ��ü���� ����� �� �ִ� ��������Ʈ��
	C++, �������Ʈ ��ü ��� ����� �� �ִ� ��������Ʈ�� ������.

	�������Ʈ ������Ʈ -> ��� �Լ��� ���� ������ �����ϰ� �ε��ϴ� ����ȭ ��Ŀ������
						   ����ֱ� ������ �Ϲ� C++ �� �����ϴ� ������� ��� �Լ���
						   ������ �� ����.
	=> ������ �������Ʈ�� ���õ� C++ �Լ��� ��� UFUNCTION ��ũ�θ� ����ؾ� �Ѵ�.

	: �̷��� �������Ʈ ��ü�͵� �����ϴ� ��������Ʈ�� �𸮾� ����������
	  ���̳��� ��������Ʈ��� �Ѵ�.
	*/
	UFUNCTION()
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	// ���� ���۰� ���� �� ���� �Ӽ��� �����ϴ� �Լ�
	void AttackStartComboState();
	void AttackEndComboState();

	void AttackCheck();

	void OnAssetLoadCompleted();
private:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack,
		Meta = (AllowPrivateAccess = true))
	bool IsAttacking;

	// ���� �޺����� �̵� ���� ����
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack,
		Meta = (AllowPrivateAccess = true))
	bool CanNextCombo;

	// �޺� �Է� ����
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack,
		Meta = (AllowPrivateAccess = true))
	bool IsComboInputOn;

	// ���� �޺� ī����
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack,
		Meta = (AllowPrivateAccess = true))
	int32 CurrentCombo;

	// �޺� ī������ �ִ�ġ
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack,
		Meta = (AllowPrivateAccess = true))
	int32 MaxCombo;

	// ���� ����ϹǷ� ��� ������ ����
	UPROPERTY()
	class UABAnimInstance* ABAnim;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack,
		Meta = (AllowPrivateAccess = true))
	float AttackRange;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack,
		Meta = (AllowPrivateAccess = true))
	float AttackRadius;

	FSoftObjectPath CharacterAssetToLoad = FSoftObjectPath(nullptr);
	TSharedPtr<struct FStreamableHandle> AssetStreamingHandle;
};
