// Fill out your copyright notice in the Description page of Project Settings.


#include "GuildRunnerPawn.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AGuildRunnerPawn::AGuildRunnerPawn()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	auto* DefaultRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Default Root"));
	MainCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Main Camera"));
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Main Camera Spring Arm"));

	RootComponent = DefaultRoot;
	SpringArm->SetupAttachment(RootComponent);
	MainCamera->SetupAttachment(SpringArm);

	SpringArm->TargetArmLength = SpringArmTargetLength;
	SpringArm->bDoCollisionTest = false;
	SpringArm->SetRelativeRotation(FRotator(-60, 0, 0));
}

// Called when the game starts or when spawned
void AGuildRunnerPawn::BeginPlay()
{
	Super::BeginPlay();

	if (const auto PC = Cast<APlayerController>(Controller))
	{
		if (const auto Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(CombatCameraMappingContext, 0);
		}
	}

	SpringArm->TargetArmLength = SpringArmTargetLength;
	DesiredZoom = SpringArm->TargetArmLength;
	DesiredLocation = GetActorLocation();
	DesiredRotation = GetActorRotation();
}

// Called every frame
void AGuildRunnerPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bInterpolating_CameraZoom)
	{
		SpringArm->TargetArmLength = FMath::FInterpTo(SpringArm->TargetArmLength, DesiredZoom, DeltaTime,
		                                              ZoomInterpolationSpeed);
		if (FMath::IsNearlyEqual(SpringArm->TargetArmLength, DesiredZoom))
		{
			SpringArm->TargetArmLength = DesiredZoom;
			bInterpolating_CameraZoom = false;
		}
	}

	if (bInterpolating_CameraMove)
	{
		const auto NewCameraLocation = FMath::VInterpTo(GetActorLocation(), DesiredLocation, DeltaTime,
		                                                MoveInterpolationSpeed);
		SetActorLocation(NewCameraLocation);
		if (FMath::IsNearlyZero(FVector::DistSquared2D(GetActorLocation(), DesiredLocation)))
		{
			SetActorLocation(DesiredLocation);
			bInterpolating_CameraMove = false;
		}
	}

	if (bInterpolating_CameraRotation)
	{
		const auto NewCameraRotation = FMath::RInterpTo(GetActorRotation(), DesiredRotation, DeltaTime,
		                                                RotateInterpolationSpeed);
		SetActorRotation(NewCameraRotation);
		if (FMath::IsNearlyEqual(GetActorRotation().Yaw, DesiredRotation.Yaw))
		{
			SetActorRotation(DesiredRotation);
			bInterpolating_CameraRotation = false;
		}
	}
}

// Called to bind functionality to input
void AGuildRunnerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (const auto EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(CombatCameraZoomAction, ETriggerEvent::Triggered, this,
		                                   &AGuildRunnerPawn::CameraZoom);
		EnhancedInputComponent->BindAction(CombatCameraMoveAction, ETriggerEvent::Triggered, this,
		                                   &AGuildRunnerPawn::CameraMove);
		EnhancedInputComponent->BindAction(CombatCameraFixedRotateAction, ETriggerEvent::Triggered, this,
		                                   &AGuildRunnerPawn::CameraFixedRotate);
	}
}

void AGuildRunnerPawn::CameraZoom(const FInputActionValue& Value)
{
	const auto ZoomValue = Value.Get<float>();

	bInterpolating_CameraZoom = true;
	DesiredZoom = FMath::Clamp(DesiredZoom + ZoomValue * CameraZoomSpeed, CameraZoomMin, CameraZoomMax);
}

void AGuildRunnerPawn::CameraMove(const FInputActionValue& Value)
{
	auto MoveAxis = Value.Get<FVector2D>();

	bInterpolating_CameraMove = true;
	MoveAxis *= CameraMoveSpeed;

	const auto ForwardVector = GetActorForwardVector() * -MoveAxis.Y;
	const auto RightVector = GetActorRightVector() * MoveAxis.X;
	const auto MoveDirection = ForwardVector + RightVector;
	DesiredLocation = DesiredLocation + MoveDirection;
}

void AGuildRunnerPawn::CameraFixedRotate(const FInputActionValue& Value)
{
	auto RotateValue = Value.Get<float>();
	RotateValue = RotateValue < 0 ? -1 : 1;

	bInterpolating_CameraRotation = true;
	DesiredRotation = UKismetMathLibrary::ComposeRotators(DesiredRotation,
	                                                      FRotator(0, CameraRotationAngle * RotateValue, 0));
}
