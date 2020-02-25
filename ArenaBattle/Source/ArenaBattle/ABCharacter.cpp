// Fill out your copyright notice in the Description page of Project Settings.


#include "ABCharacter.h"
#include "ABAnimInstance.h"

// 디버그 드로잉 : 다양한 그리기 함수들이 선언되있음. -> ex) 공격 범위를 시각적으로 표현
#include "DrawDebugHelpers.h"

// Sets default values
AABCharacter::AABCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArm = CreateDefaultSubobject <USpringArmComponent>(TEXT("SPRINGARM"));
	Camera = CreateDefaultSubobject <UCameraComponent>(TEXT("CAMERA"));

	SpringArm->SetupAttachment(GetCapsuleComponent());
	Camera->SetupAttachment(SpringArm);

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

	AttackRange = 200.0f;
	AttackRadius = 50.0f;
}

// Called when the game starts or when spawned
void AABCharacter::BeginPlay()
{
	Super::BeginPlay();

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
}

float AABCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
	class AController* EventInstigator, AActor* DamageCauser)
{
	float FinalDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	ABLOG(Warning, TEXT("Actor : %s took Damage : %f"), *GetName(), FinalDamage);
	
	if (FinalDamage > 0.0f)
	{
		ABAnim->SetDeadAnim();
		SetActorEnableCollision(false);
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
	// 충돌된 액터에 관련된 정보를 가질 구조체
	FHitResult HitResult;

	FCollisionQueryParams Params(NAME_None, false, this);

	bool bResult = GetWorld()->SweepSingleByChannel(
		HitResult,	

		// 도형의 탐색 영역
		GetActorLocation(),
		GetActorLocation() + GetActorForwardVector() * AttackRange,
		
		FQuat::Identity,

		// Attack 트레이스 채널
		ECollisionChannel::ECC_GameTraceChannel2,		

		// 탐색할 도형 : 50cm 반지름 구체
		FCollisionShape::MakeSphere(AttackRadius),					

		// 공격 명령을 내리는 자신은 이 탐색에 감지되지 않도록 설정
		Params);

	// 공격 탐지 범위 시각적으로 표현 해주기
#if ENABLE_DRAW_DEBUG
	FVector TraceVec = GetActorForwardVector() * AttackRange;
	FVector Center = GetActorLocation() + TraceVec * 0.5f;
	float HalfHeight = AttackRange * 0.5f + AttackRadius;
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
			HitResult.Actor->TakeDamage(50.0f, DamageEvent, GetController(), this);
		}
}