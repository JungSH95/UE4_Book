// Fill out your copyright notice in the Description page of Project Settings.


#include "ABCharacter.h"
#include "ABAnimInstance.h"
#include "ABWeapon.h"

// 캐릭터에 부착해 캐릭터 스탯에 대한 관리를 액터 컴포넌트가 하도록
#include "ABCharacterStatComponent.h"

// 디버그 드로잉 : 다양한 그리기 함수들이 선언되있음. -> ex) 공격 범위를 시각적으로 표현
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

	// 점프값 설정 (420 : 기본값)
	GetCharacterMovement()->JumpZVelocity = 800.0f;

	IsAttacking = false;

	MaxCombo = 4;
	AttackEndComboState();

	// 캐릭터의 캡슐 컴포넌트에 있는 콜리전 프리셋을 기존 Pawn에서 ABCharacter로 설정
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

	/*	에셋 목록을 읽어들인 후 로그 출력
	auto DefaultSetting = GetDefault<UABCharacterSetting>();
	if (DefaultSetting->CharacterAssets.Num() > 0)
		for (auto CharacterAsset : DefaultSetting->CharacterAssets)
			ABLOG(Warning, TEXT("Character Asset : %s"), *CharacterAsset.ToString());
	*/

	AssetIndex = 4;

	SetActorHiddenInGame(true);
	HPBarWidget->SetHiddenInGame(true);

	// 4.24 에서는 바로 접근 불가능 bCanBeDamaged = false;
	SetCanBeDamaged(false);

	DeadTimer = 5.0f;
}

// Called when the game starts or when spawned
void AABCharacter::BeginPlay()
{
	Super::BeginPlay();

	/*   hand_rSocket 에 무기 장착
	FName WeaponSocket(TEXT("hand_rSocket"));
	auto CurWeapon = GetWorld()->SpawnActor<AABWeapon>(FVector::ZeroVector, FRotator::ZeroRotator);

	if (nullptr != CurWeapon)
		CurWeapon->AttachToComponent(GetMesh(),
			FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSocket);
	*/

	/*     이거 때문에 개 고 생

	UI 시스템은 플레이어 컨트롤러의 BeginPlay() 에서 생성된다.
	따라서 이 클래스의 PostInitializeComponents()은 위젯 생성에 효능이 없어 BeginPlay()에서 작업을 해야한다.

	-> 책에서는 PostInitializeComponents() 에 작성 : 4.21 전에는 가능했음 ---------------------
	4.21 버전때 UI의 초기화가 BeginPlay 시점으로 변경 됨
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
			// 비동기 방식으로 에셋을 로딩하는 함수 : RequestAsyncLoad()
			// FStreamableDelegate 형식의 델리게이트를 넘겨줄 경우, 애셋이 로딩을 완료하면 해당 델리게이트에 연결된
			// 함수롤 호출해준다.
			AssetStreamingHandle = ABGameInstance->StreamableManager.RequestAsyncLoad(CharacterAssetToLoad,
				FStreamableDelegate::CreateUObject(this, &AABCharacter::OnAssetLoadCompleted));
	}
	*/


	// Chapter 14 - 게임 플레이의 제작
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

			// HUD 위젯과 캐릭터 스탯 컴포넌트를 연결
			ABPlayerController->GetHUDWidget()->BindCharacterStat(CharacterStat);

			auto ABPlayerState = Cast<AABPlayerState>(GetPlayerState());
			ABCHECK(nullptr != ABPlayerState);
			CharacterStat->SetNewLevel(ABPlayerState->GetCharacterLevel());
		}
		// 게임 스코어를 기반으로 NPC 능력시 설정
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

// 무기 유무에 따른 공격 범위
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

	// 무기 보유중일 경우 기존 무기 삭제
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

// 흰색 마네킹과 동일한 방식으로 캐릭터를 동작하도록 만들기
void AABCharacter::SetControlMode(EControlMode NewControlMode)
{
	CurrentControlMode = NewControlMode;

	switch (CurrentControlMode)
	{
	case EControlMode::GTA:
		// 카메라 지지대 길이
		//SpringArm->TargetArmLength = 450.0f;
		ArmLengthTo = 450.0f;

		// 캐릭터의 회전
		//SpringArm->SetRelativeRotation(FRotator::ZeroRotator);

		// 폰의 로테이션 제어 설정
		SpringArm->bUsePawnControlRotation = true;

		SpringArm->bInheritPitch = true;
		SpringArm->bInheritRoll = true;
		SpringArm->bInheritYaw = true;

		// 카메라와 캐릭터 사이에 장애물 감지시 캐릭터가 보이도록 카메라를 장애물 앞으로 줌인
		SpringArm->bDoCollisionTest = true;

		// 카메라 회전
		bUseControllerRotationYaw = false;

		// 캐릭터가 움직이는 방향으로 캐릭터를 자동으로 회전 시켜준다.
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

		// true 일경우 45도 단위로 끊겨서 회전
		bUseControllerRotationYaw = false;
		// 아래의 속성을 사용하여 부드럽게 회전
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

	// FInterpTo() 함수를 통해 지정한 속력으로 목표 지점까지 진행하되,
	// 목표 지점까지 도달하면 그 값에서 멈추는 기능
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
			// FRotationMatrix: 회전된 좌표계 정보를 저장하는 행렬
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

	// ABAnimInstance의 OnNextAttackCheck 델리게이트와 등록할 로직
	// C++의 람다식 구문을 사용
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

//  14장에서 게임 플레이 제작으로 변경하면서 해당 기능을 READT 스테이트에서 구현
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
	아래의 공격 받을 때마다 instigator를 검사하는 방식보다 소멸된 때 LastHitBy를 사용해
	처리하는 것이 효율적, 학습을 위해 2가지 방법 사용해 본 것
	*/
	// 데미지를 가했는데 죽었으면
	if (CurrentState == ECharacterState::DEAD)
	{
		// 이벤트의 가해자가 플레이어이면
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

	// ACharacter 클래스에 Jump라는 멤버 함수가 있다.
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

	// 카메라가 보는 방향 기준으로 캐릭터가 이동
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

	// 카메라가 보는 방향 기준으로 캐릭터가 이동
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

// Shift + V 키를 누를 때마다 캐릭터의 컨트롤 변경
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

// 마우스 왼쪽 버튼 눌러졌을 경우 실행
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
OnMontageEnded 델리게이트는 블루프린트와 호환되는 성질 외에도 여러 개의 함수를 받을 수 있어서
행동이 끝나면 등록된 모든 함수들에게 모두 알려주는 기능도 제공한다.
=> 멀티캐스트 델리게이트 라고 한다.
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

	// 충돌된 액터에 관련된 정보를 가질 구조체
	FHitResult HitResult;

	FCollisionQueryParams Params(NAME_None, false, this);

	bool bResult = GetWorld()->SweepSingleByChannel(
		HitResult,	

		// 도형의 탐색 영역
		GetActorLocation(),
		GetActorLocation() + GetActorForwardVector() * FinalAttackRange,
		
		FQuat::Identity,

		// Attack 트레이스 채널
		ECollisionChannel::ECC_GameTraceChannel2,		

		// 탐색할 도형 : 50cm 반지름 구체
		FCollisionShape::MakeSphere(AttackRadius),					

		// 공격 명령을 내리는 자신은 이 탐색에 감지되지 않도록 설정
		Params);

	// 공격 탐지 범위 시각적으로 표현 해주기
#if ENABLE_DRAW_DEBUG
	FVector TraceVec = GetActorForwardVector() * FinalAttackRange;
	FVector Center = GetActorLocation() + TraceVec * 0.5f;
	float HalfHeight = FinalAttackRange * 0.5f + AttackRadius;
	FQuat CapsuleRot = FRotationMatrix::MakeFromZ(TraceVec).ToQuat();

	// 공격 판정이 발생 : 녹색 아니면 빨강
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
	FHitResult의 멤버 변수 Actor의 선언이 일반 참조로 선언된다면
	해당 함수에서의 참조로 인해 제거돼야 할 액터가 메모리에 남아있는 문제가 발생할 수 있다.
	이러한 문제를 방지하기 위해 FHitResult는 참조로부터 자유롭게 포인터 정보를
	전달해주는 약 포인터 방식으로 멤버 변수를 선언했다.

	-> 약 포인터로 지정된 액터에 접근하려면 IsValid 함수를 사용해 사용하려는
	   액터가 유효한지 먼저 점검하고 사용해야한다.
	*/

	if (bResult)
		if (HitResult.Actor.IsValid())
		{
			ABLOG(Warning, TEXT("Hit Actor Name : %s"), *HitResult.Actor->GetName());

			// 대상 액터에 대미지를 전달
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