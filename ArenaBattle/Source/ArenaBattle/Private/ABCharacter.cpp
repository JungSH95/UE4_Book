// Fill out your copyright notice in the Description page of Project Settings.


#include "ABCharacter.h"
#include "ABAnimInstance.h"
#include "ABWeapon.h"

// ĳ���Ϳ� ������ ĳ���� ���ȿ� ���� ������ ���� ������Ʈ�� �ϵ���
#include "ABCharacterStatComponent.h"

// ����� ����� : �پ��� �׸��� �Լ����� ���������. -> ex) ���� ������ �ð������� ǥ��
#include "DrawDebugHelpers.h"
#include "Components/WidgetComponent.h"
#include "ABCharacterWidget.h"
#include "ABAIController.h"
#include "ABCharacterSetting.h"
#include "ABGameInstance.h"
#include "ABPlayerController.h"
#include "ABPlayerState.h"
#include "ABHUDWidget.h"
#include "ABGameMode.h"

// Sets default values
AABCharacter::AABCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArm = CreateDefaultSubobject <USpringArmComponent>(TEXT("SPRINGARM"));
	Camera = CreateDefaultSubobject <UCameraComponent>(TEXT("CAMERA"));

	CharacterStat = CreateDefaultSubobject<UABCharacterStatComponent>(TEXT("CHARACTERSTAT"));
	HPBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HPBARWIDGET"));

	SpringArm->SetupAttachment(GetCapsuleComponent());
	Camera->SetupAttachment(SpringArm);
	HPBarWidget->SetupAttachment(GetMesh());

	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -88.0f),
		FRotator(0.0f, -90.0f, 0.0f));
	SpringArm->TargetArmLength = 400.0f;
	SpringArm->SetRelativeRotation(FRotator(-15.0f, 0.0f, 0.0f));

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_CARDBOARD(TEXT("/Game/InfinityBladeWarriors/Character/CompleteCharacters/SK_CharM_Cardboard.SK_CharM_Cardboard"));
	if (SK_CARDBOARD.Succeeded())
		GetMesh()->SetSkeletalMesh(SK_CARDBOARD.Object);

	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	
	static ConstructorHelpers::FClassFinder<UAnimInstance> WARRIOR_ANIM(TEXT("/Game/Book/Animations/WarriorAnimBlueprint"));

	if (WARRIOR_ANIM.Succeeded())
		GetMesh()->SetAnimInstanceClass(WARRIOR_ANIM.Class);

	SetControlMode(EControlMode::DIABLO);

	ArmLengthSpeed = 3.0f;
	ArmRotationSpeed = 10.0f;

	// ������ ���� (420 : �⺻��)
	GetCharacterMovement()->JumpZVelocity = 800.0f;

	IsAttacking = false;

	MaxCombo = 4;
	AttackEndComboState();

	// ĳ������ ĸ�� ������Ʈ�� �ִ� �ݸ��� �������� ���� Pawn���� ABCharacter�� ����
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("ABCharacter"));

	AttackRange = 80.0f;
	AttackRadius = 50.0f;

	HPBarWidget->SetRelativeLocation(FVector(0.0f, 0.0f, 180.0f));
	HPBarWidget->SetWidgetSpace(EWidgetSpace::Screen);
	static ConstructorHelpers::FClassFinder<UUserWidget> UI_HUD(TEXT("/Game/Book/UI/UI_HPBar.UI_HPBar_C"));
	if (UI_HUD.Succeeded())
	{
		HPBarWidget->SetWidgetClass(UI_HUD.Class);
		HPBarWidget->SetDrawSize(FVector2D(150.0f, 50.0f));
	}

	AIControllerClass = AABAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	/*	���� ����� �о���� �� �α� ���
	auto DefaultSetting = GetDefault<UABCharacterSetting>();
	if (DefaultSetting->CharacterAssets.Num() > 0)
		for (auto CharacterAsset : DefaultSetting->CharacterAssets)
			ABLOG(Warning, TEXT("Character Asset : %s"), *CharacterAsset.ToString());
	*/

	AssetIndex = 4;

	SetActorHiddenInGame(true);
	HPBarWidget->SetHiddenInGame(true);

	// 4.24 ������ �ٷ� ���� �Ұ��� bCanBeDamaged = false;
	SetCanBeDamaged(false);

	DeadTimer = 5.0f;
}

// Called when the game starts or when spawned
void AABCharacter::BeginPlay()
{
	Super::BeginPlay();

	/*   hand_rSocket �� ���� ����
	FName WeaponSocket(TEXT("hand_rSocket"));
	auto CurWeapon = GetWorld()->SpawnActor<AABWeapon>(FVector::ZeroVector, FRotator::ZeroRotator);

	if (nullptr != CurWeapon)
		CurWeapon->AttachToComponent(GetMesh(),
			FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSocket);
	*/

	/*     �̰� ������ �� �� ��

	UI �ý����� �÷��̾� ��Ʈ�ѷ��� BeginPlay() ���� �����ȴ�.
	���� �� Ŭ������ PostInitializeComponents()�� ���� ������ ȿ���� ���� BeginPlay()���� �۾��� �ؾ��Ѵ�.

	-> å������ PostInitializeComponents() �� �ۼ� : 4.21 ������ �������� ---------------------
	4.21 ������ UI�� �ʱ�ȭ�� BeginPlay �������� ���� ��
	*/
	auto CharacterWidget = Cast<UABCharacterWidget>(HPBarWidget->GetUserWidgetObject());
	if (nullptr != CharacterWidget)
		CharacterWidget->BindCharacterStat(CharacterStat);
	// --------------------------------------------------------------------------------

	/*
	if (!IsPlayerControlled())
	{
		auto DefaultSetting = GetDefault<UABCharacterSetting>();
		int32 RandIndex = FMath::RandRange(0, DefaultSetting->CharacterAssets.Num() - 1);
		CharacterAssetToLoad = DefaultSetting->CharacterAssets[RandIndex];

		auto ABGameInstance = Cast<UABGameInstance>(GetGameInstance());
		if (nullptr != ABGameInstance)
			// �񵿱� ������� ������ �ε��ϴ� �Լ� : RequestAsyncLoad()
			// FStreamableDelegate ������ ��������Ʈ�� �Ѱ��� ���, �ּ��� �ε��� �Ϸ��ϸ� �ش� ��������Ʈ�� �����
			// �Լ��� ȣ�����ش�.
			AssetStreamingHandle = ABGameInstance->StreamableManager.RequestAsyncLoad(CharacterAssetToLoad,
				FStreamableDelegate::CreateUObject(this, &AABCharacter::OnAssetLoadCompleted));
	}
	*/


	// Chapter 14 - ���� �÷����� ����
	bIsPlayer = IsPlayerControlled();
	if (bIsPlayer)
	{
		ABPlayerController = Cast<AABPlayerController>(GetController());
		ABCHECK(nullptr != ABPlayerController);
	}
	else
	{
		ABAIController = Cast<AABAIController>(GetController());
		ABCHECK(nullptr != ABAIController);
	}

	auto DefaultSetting = GetDefault<UABCharacterSetting>();

	if (bIsPlayer)
	{
		//AssetIndex = 4;

		auto ABPlayerStat = Cast<AABPlayerState>(GetPlayerState());
		ABCHECK(nullptr != ABPlayerStat);
		AssetIndex = ABPlayerStat->GetCharacterIndex();
	}
	else
		AssetIndex = FMath::RandRange(0, DefaultSetting->CharacterAssets.Num() - 1);

	CharacterAssetToLoad = DefaultSetting->CharacterAssets[AssetIndex];
	auto ABGameInstance = Cast<UABGameInstance>(GetGameInstance());
	ABCHECK(nullptr != ABGameInstance);
	AssetStreamingHandle = ABGameInstance->StreamableManager.RequestAsyncLoad(CharacterAssetToLoad,
		FStreamableDelegate::CreateUObject(this, &AABCharacter::OnAssetLoadCompleted));
	SetCharacterState(ECharacterState::LOADING);
}

void AABCharacter::SetCharacterState(ECharacterState NewState)
{
	ABCHECK(CurrentState != NewState);
	CurrentState = NewState;

	switch (CurrentState)
	{
	case ECharacterState::LOADING:
	{
		if (bIsPlayer)
		{
			DisableInput(ABPlayerController);

			// HUD ������ ĳ���� ���� ������Ʈ�� ����
			ABPlayerController->GetHUDWidget()->BindCharacterStat(CharacterStat);

			auto ABPlayerState = Cast<AABPlayerState>(GetPlayerState());
			ABCHECK(nullptr != ABPlayerState);
			CharacterStat->SetNewLevel(ABPlayerState->GetCharacterLevel());
		}
		// ���� ���ھ ������� NPC �ɷ½� ����
		else
		{
			auto ABGameMode = Cast<AABGameMode>(GetWorld()->GetAuthGameMode());
			ABCHECK(nullptr != ABGameMode);
			int32 TargetLevel = FMath::CeilToInt(((float)ABGameMode->GetScore() * 0.8f));
			int32 FinalLevel = FMath::Clamp<int32>(TargetLevel, 1, 20);
			ABLOG(Warning, TEXT("New NPC Level : %d"), FinalLevel);
			CharacterStat->SetNewLevel(FinalLevel);
		}

		SetActorHiddenInGame(true);
		HPBarWidget->SetHiddenInGame(true);
		SetCanBeDamaged(false);
		break;
	}
	case ECharacterState::READY:
	{
		SetActorHiddenInGame(false);
		HPBarWidget->SetHiddenInGame(false);
		SetCanBeDamaged(true);

		CharacterStat->OnHPIsZero.AddLambda([this]() -> void {
			SetCharacterState(ECharacterState::DEAD);
		});

		auto CharacterWidget = Cast<UABCharacterWidget>(HPBarWidget->GetUserWidgetObject());
		ABCHECK(nullptr != CharacterWidget);
		CharacterWidget->BindCharacterStat(CharacterStat);

		if (bIsPlayer)
		{
			SetControlMode(EControlMode::DIABLO);
			GetCharacterMovement()->MaxWalkSpeed = 600.0f;
			EnableInput(ABPlayerController);
		}
		else
		{
			SetControlMode(EControlMode::NPC);
			GetCharacterMovement()->MaxWalkSpeed = 400.0f;
			ABAIController->RunAI();
		}
		break;
	}
	case ECharacterState::DEAD:
	{
		SetActorEnableCollision(false);
		GetMesh()->SetHiddenInGame(false);
		HPBarWidget->SetHiddenInGame(true);
		ABAnim->SetDeadAnim();
		SetCanBeDamaged(false);

		if (bIsPlayer)
			DisableInput(ABPlayerController);
		else
			ABAIController->StopAI();

		GetWorld()->GetTimerManager().SetTimer(DeadTimerHandle, FTimerDelegate::CreateLambda([this]() -> void
		{
			if (bIsPlayer)
				ABPlayerController->RestartLevel();
			else
				Destroy();
		}), DeadTimer, false);
		break;
	}
	}
}

ECharacterState AABCharacter::GetCharacterState() const
{
	return CurrentState;
}

int32 AABCharacter::GetExp() const
{
	return CharacterStat->GetDropExp();
}

// ���� ������ ���� ���� ����
float AABCharacter::GetFinalAttackRange() const
{
	return (nullptr != CurrentWeapon) ? CurrentWeapon->GetAttackRange() : AttackRange;
}

float AABCharacter::GetFinalAttackDamage() const
{
	float AttackDamage = (nullptr != CurrentWeapon) ? (CharacterStat->GetAttack() +
		CurrentWeapon->GetAttackDamage()) : CharacterStat->GetAttack();

	float AttackModifier = (nullptr != CurrentWeapon) ? CurrentWeapon->GetAttackModifier() : 1.0f;

	return AttackDamage * AttackModifier;
}

bool AABCharacter::CanSetWeapon()
{
	//return (nullptr == CurrentWeapon);
	return true;
}

void AABCharacter::SetWeapon(AABWeapon* NewWeapon)
{
	//ABCHECK(nullptr != NewWeapon && nullptr == CurrentWeapon);
	ABCHECK(nullptr != NewWeapon);

	// ���� �������� ��� ���� ���� ����
	if (nullptr != CurrentWeapon)
	{
		CurrentWeapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		CurrentWeapon->Destroy();
		CurrentWeapon = nullptr;
	}

	FName WeaponSocket(TEXT("hand_rSocket"));
	if (nullptr != NewWeapon)
	{
		NewWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSocket);
		NewWeapon->SetOwner(this);
		CurrentWeapon = NewWeapon;
	}
}

// ��� ����ŷ�� ������ ������� ĳ���͸� �����ϵ��� �����
void AABCharacter::SetControlMode(EControlMode NewControlMode)
{
	CurrentControlMode = NewControlMode;

	switch (CurrentControlMode)
	{
	case EControlMode::GTA:
		// ī�޶� ������ ����
		//SpringArm->TargetArmLength = 450.0f;
		ArmLengthTo = 450.0f;

		// ĳ������ ȸ��
		//SpringArm->SetRelativeRotation(FRotator::ZeroRotator);

		// ���� �����̼� ���� ����
		SpringArm->bUsePawnControlRotation = true;

		SpringArm->bInheritPitch = true;
		SpringArm->bInheritRoll = true;
		SpringArm->bInheritYaw = true;

		// ī�޶�� ĳ���� ���̿� ��ֹ� ������ ĳ���Ͱ� ���̵��� ī�޶� ��ֹ� ������ ����
		SpringArm->bDoCollisionTest = true;

		// ī�޶� ȸ��
		bUseControllerRotationYaw = false;

		// ĳ���Ͱ� �����̴� �������� ĳ���͸� �ڵ����� ȸ�� �����ش�.
		GetCharacterMovement()->bOrientRotationToMovement = true;
		GetCharacterMovement()->bUseControllerDesiredRotation = false;
		GetCharacterMovement()->RotationRate = FRotator(0.0f, 720.0f, 0.0f);
		break;
	case EControlMode::DIABLO:
		//SpringArm->TargetArmLength = 800.0f;
		//SpringArm->SetRelativeRotation(FRotator(-45.0f, 0.0f, 0.0f));

		ArmLengthTo = 800.0f;
		ArmRotationTo = FRotator(-45.0f, 0.0f, 0.0f);

		SpringArm->bUsePawnControlRotation = false;
		SpringArm->bInheritPitch = false;
		SpringArm->bInheritRoll = false;
		SpringArm->bInheritYaw = false;
		SpringArm->bDoCollisionTest = false;

		// true �ϰ�� 45�� ������ ���ܼ� ȸ��
		bUseControllerRotationYaw = false;
		// �Ʒ��� �Ӽ��� ����Ͽ� �ε巴�� ȸ��
		GetCharacterMovement()->bOrientRotationToMovement = false;
		GetCharacterMovement()->bUseControllerDesiredRotation = true;
		GetCharacterMovement()->RotationRate = FRotator(0.0f, 720.0f, 0.0f);
		break;
	case EControlMode::NPC:
		bUseControllerRotationYaw = false;
		GetCharacterMovement()->bUseControllerDesiredRotation = false;
		GetCharacterMovement()->bOrientRotationToMovement = true;
		GetCharacterMovement()->RotationRate = FRotator(0.0f, 480.0f, 0.0f);
		break;
	}
}

// Called every frame
void AABCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// FInterpTo() �Լ��� ���� ������ �ӷ����� ��ǥ �������� �����ϵ�,
	// ��ǥ �������� �����ϸ� �� ������ ���ߴ� ���
	SpringArm->TargetArmLength = FMath::FInterpTo(SpringArm->TargetArmLength, ArmLengthTo,
		DeltaTime, ArmLengthSpeed);
	
	switch (CurrentControlMode)
	{
	case EControlMode::DIABLO:
		SpringArm->RelativeRotation = FMath::RInterpTo(SpringArm->RelativeRotation,
			ArmRotationTo, DeltaTime, ArmRotationSpeed);
		break;
	}

	switch (CurrentControlMode)
	{
	case EControlMode::DIABLO:
		if (DirctionToMove.SizeSquared() > 0.0f)
		{
			// FRotationMatrix: ȸ���� ��ǥ�� ������ �����ϴ� ���
			GetController()->SetControlRotation(FRotationMatrix::MakeFromX(DirctionToMove).Rotator());
			AddMovementInput(DirctionToMove);
		}
		break;
	}
}

void AABCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	ABAnim = Cast<UABAnimInstance>(GetMesh()->GetAnimInstance());
	ABCHECK(nullptr != ABAnim);

	ABAnim->OnMontageEnded.AddDynamic(this, &AABCharacter::OnAttackMontageEnded);

	// ABAnimInstance�� OnNextAttackCheck ��������Ʈ�� ����� ����
	// C++�� ���ٽ� ������ ���
	ABAnim->OnNextAttackCheck.AddLambda([this]() -> void {

		ABLOG(Warning, TEXT("OnNextAttackCheck"));
		CanNextCombo = false;

		if (IsComboInputOn)
		{
			AttackStartComboState();
			ABAnim->JumpToAttackMontageSection(CurrentCombo);
		}
	});

	ABAnim->OnAttackHitCheck.AddUObject(this, &AABCharacter::AttackCheck);

	CharacterStat->OnHPIsZero.AddLambda([this]() -> void
	{
		ABLOG(Warning, TEXT("OnHPIsZero"));
		ABAnim->SetDeadAnim();
		SetActorEnableCollision(false);
	});
}

//  14�忡�� ���� �÷��� �������� �����ϸ鼭 �ش� ����� READT ������Ʈ���� ����
void AABCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	/*
	if (IsPlayerControlled())
	{
		SetControlMode(EControlMode::DIABLO);
		GetCharacterMovement()->MaxWalkSpeed = 600.0f;
	}
	else
	{
		SetControlMode(EControlMode::NPC);
		GetCharacterMovement()->MaxWalkSpeed = 300.0f;
	}
	*/
}


float AABCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
	class AController* EventInstigator, AActor* DamageCauser)
{
	float FinalDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	//ABLOG(Warning, TEXT("Actor : %s took Damage : %f"), *GetName(), FinalDamage);
	
	/*
	if (FinalDamage > 0.0f)
	{
		ABAnim->SetDeadAnim();
		SetActorEnableCollision(false);
	}
	*/

	CharacterStat->SetDamage(FinalDamage);

	/*
	�Ʒ��� ���� ���� ������ instigator�� �˻��ϴ� ��ĺ��� �Ҹ�� �� LastHitBy�� �����
	ó���ϴ� ���� ȿ����, �н��� ���� 2���� ��� ����� �� ��
	*/
	// �������� ���ߴµ� �׾�����
	if (CurrentState == ECharacterState::DEAD)
	{
		// �̺�Ʈ�� �����ڰ� �÷��̾��̸�
		if (EventInstigator->IsPlayerController())
		{
			auto instigator = Cast<AABPlayerController>(EventInstigator);
			ABCHECK(nullptr != instigator, 0.0f);
			instigator->NPCKill(this);
		}
	}

	return FinalDamage;
}

// Called to bind functionality to input
void AABCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction(TEXT("ViewChange"), EInputEvent::IE_Pressed, this,
		&AABCharacter::ViewChange);

	// ACharacter Ŭ������ Jump��� ��� �Լ��� �ִ�.
	PlayerInputComponent->BindAction(TEXT("Jump"), EInputEvent::IE_Pressed, this,
		&ACharacter::Jump);

	PlayerInputComponent->BindAction(TEXT("Attack"), EInputEvent::IE_Pressed, this,
		&AABCharacter::Attack);

	PlayerInputComponent->BindAxis(TEXT("UpDown"), this, &AABCharacter::UpDown);
	PlayerInputComponent->BindAxis(TEXT("LeftRight"), this, &AABCharacter::LeftRight);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &AABCharacter::LookUp);
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &AABCharacter::Turn);
}

void AABCharacter::UpDown(float NewAxisValue)
{
	//AddMovementInput(GetActorForwardVector(), NewAxisValue);

	// ī�޶� ���� ���� �������� ĳ���Ͱ� �̵�
	//AddMovementInput(FRotationMatrix(GetControlRotation()).GetUnitAxis(EAxis::X), NewAxisValue);

	switch (CurrentControlMode)
	{
	case EControlMode::GTA:
		AddMovementInput(FRotationMatrix(GetControlRotation()).GetUnitAxis(EAxis::X), NewAxisValue);
		break;
	case EControlMode::DIABLO:
		DirctionToMove.X = NewAxisValue;
		break;
	}
}

void AABCharacter::LeftRight(float NewAxisValue)
{
	//AddMovementInput(GetActorRightVector(), NewAxisValue);

	// ī�޶� ���� ���� �������� ĳ���Ͱ� �̵�
	//AddMovementInput(FRotationMatrix(GetControlRotation()).GetUnitAxis(EAxis::Y), NewAxisValue);

	switch (CurrentControlMode)
	{
	case EControlMode::GTA:
		AddMovementInput(FRotationMatrix(GetControlRotation()).GetUnitAxis(EAxis::Y), NewAxisValue);
		break;
	case EControlMode::DIABLO:
		DirctionToMove.Y = NewAxisValue;
		break;
	}
}

void AABCharacter::LookUp(float NewAxisValue)
{
	//AddControllerPitchInput(NewAxisValue);

	switch (CurrentControlMode)
	{
	case EControlMode::GTA:
		AddControllerPitchInput(NewAxisValue);
		break;
	}
}

void AABCharacter::Turn(float NewAxisValue)
{
	//AddControllerYawInput(NewAxisValue);

	switch (CurrentControlMode)
	{
	case EControlMode::GTA:
		AddControllerYawInput(NewAxisValue);
		break;
	}
}

// Shift + V Ű�� ���� ������ ĳ������ ��Ʈ�� ����
void AABCharacter::ViewChange()
{
	switch (CurrentControlMode)
	{
	case EControlMode::GTA:
		GetController()->SetControlRotation(GetActorRotation());
		SetControlMode(EControlMode::DIABLO);
		break;
	case EControlMode::DIABLO:
		GetController()->SetControlRotation(SpringArm->RelativeRotation);
		SetControlMode(EControlMode::GTA);
		break;
	}
}

// ���콺 ���� ��ư �������� ��� ����
void AABCharacter::Attack()
{
	if (IsAttacking)
	{
		ABCHECK(FMath::IsWithinInclusive<int32>(CurrentCombo, 1, MaxCombo));
		if (CanNextCombo)
			IsComboInputOn = true;
	}
	else
	{
		ABCHECK(CurrentCombo == 0);
		AttackStartComboState();
		ABAnim->PlayAttackMontage();
		ABAnim->JumpToAttackMontageSection(CurrentCombo);
		IsAttacking = true;
	}

	//ABLOG_S(Warning);

	//auto AnimInstance = Cast<UABAnimInstance>(GetMesh()->GetAnimInstance());
	//if (nullptr == AnimInstance)
	//	return;
	//AnimInstance->PlayAttackMontage();

	//ABAnim->PlayAttackMontage();
	//IsAttacking = true;
}

/*
OnMontageEnded ��������Ʈ�� �������Ʈ�� ȣȯ�Ǵ� ���� �ܿ��� ���� ���� �Լ��� ���� �� �־
�ൿ�� ������ ��ϵ� ��� �Լ��鿡�� ��� �˷��ִ� ��ɵ� �����Ѵ�.
=> ��Ƽĳ��Ʈ ��������Ʈ ��� �Ѵ�.
*/
void AABCharacter::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	ABCHECK(IsAttacking);
	ABCHECK(CurrentCombo > 0);
	IsAttacking = false;
	AttackEndComboState();

	OnAttackEnd.Broadcast();
}

void AABCharacter::AttackStartComboState()
{
	CanNextCombo = true;
	IsComboInputOn = false;
	ABCHECK(FMath::IsWithinInclusive<int32>(CurrentCombo, 0, MaxCombo - 1));
	CurrentCombo = FMath::Clamp<int32>(CurrentCombo + 1, 1, MaxCombo);
}

void AABCharacter::AttackEndComboState()
{
	IsComboInputOn = false;
	CanNextCombo = false;
	CurrentCombo = 0;
}

void AABCharacter::AttackCheck()
{
	float FinalAttackRange = GetFinalAttackRange();

	// �浹�� ���Ϳ� ���õ� ������ ���� ����ü
	FHitResult HitResult;

	FCollisionQueryParams Params(NAME_None, false, this);

	bool bResult = GetWorld()->SweepSingleByChannel(
		HitResult,	

		// ������ Ž�� ����
		GetActorLocation(),
		GetActorLocation() + GetActorForwardVector() * FinalAttackRange,
		
		FQuat::Identity,

		// Attack Ʈ���̽� ä��
		ECollisionChannel::ECC_GameTraceChannel2,		

		// Ž���� ���� : 50cm ������ ��ü
		FCollisionShape::MakeSphere(AttackRadius),					

		// ���� ����� ������ �ڽ��� �� Ž���� �������� �ʵ��� ����
		Params);

	// ���� Ž�� ���� �ð������� ǥ�� ���ֱ�
#if ENABLE_DRAW_DEBUG
	FVector TraceVec = GetActorForwardVector() * FinalAttackRange;
	FVector Center = GetActorLocation() + TraceVec * 0.5f;
	float HalfHeight = FinalAttackRange * 0.5f + AttackRadius;
	FQuat CapsuleRot = FRotationMatrix::MakeFromZ(TraceVec).ToQuat();

	// ���� ������ �߻� : ��� �ƴϸ� ����
	FColor DrawColor = bResult ? FColor::Green : FColor::Red;
	float DebugLifeTime = 5.0f;

	DrawDebugCapsule(GetWorld(),
		Center,
		HalfHeight,
		AttackRadius,
		CapsuleRot,
		DrawColor,
		false,
		DebugLifeTime);
#endif

	/*
	FHitResult�� ��� ���� Actor�� ������ �Ϲ� ������ ����ȴٸ�
	�ش� �Լ������� ������ ���� ���ŵž� �� ���Ͱ� �޸𸮿� �����ִ� ������ �߻��� �� �ִ�.
	�̷��� ������ �����ϱ� ���� FHitResult�� �����κ��� �����Ӱ� ������ ������
	�������ִ� �� ������ ������� ��� ������ �����ߴ�.

	-> �� �����ͷ� ������ ���Ϳ� �����Ϸ��� IsValid �Լ��� ����� ����Ϸ���
	   ���Ͱ� ��ȿ���� ���� �����ϰ� ����ؾ��Ѵ�.
	*/

	if (bResult)
		if (HitResult.Actor.IsValid())
		{
			ABLOG(Warning, TEXT("Hit Actor Name : %s"), *HitResult.Actor->GetName());

			// ��� ���Ϳ� ������� ����
			FDamageEvent DamageEvent;
			HitResult.Actor->TakeDamage(GetFinalAttackDamage(), DamageEvent, GetController(), this);
		}
}

void AABCharacter::OnAssetLoadCompleted()
{
	AssetStreamingHandle->ReleaseHandle();
	TSoftObjectPtr<USkeletalMesh> LoadedAssetPath(CharacterAssetToLoad);
	
	/*
	if (LoadedAssetPath.IsValid())
		GetMesh()->SetSkeletalMesh(LoadedAssetPath.Get());
	*/

	ABCHECK(LoadedAssetPath.IsValid());
	GetMesh()->SetSkeletalMesh(LoadedAssetPath.Get());
	SetCharacterState(ECharacterState::READY);
}