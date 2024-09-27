// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "GameFramework/Pawn.h"
#include "GuildRunnerPawn.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

UCLASS()
class GUILDRUNNER_API AGuildRunnerPawn : public APawn
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GuildRunner|Camera", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* MainCamera;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GuildRunner|Camera", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* SpringArm;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GuildRunner|Input", meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* CombatCameraMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GuildRunner|Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* CombatCameraZoomAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GuildRunner|Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* CombatCameraMoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GuildRunner|Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* CombatCameraFixedRotateAction;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GuildRunner|Camera", meta = (AllowPrivateAccess = "true"))
	//float SpringArmTargetLength = 700.f;

	//Minimum spring arm length
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GuildRunner|Camera", meta = (AllowPrivateAccess = "true"))
	float CameraZoomMin = 100.f;

	//Maximum spring arm length
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GuildRunner|Camera", meta = (AllowPrivateAccess = "true"))
	float CameraZoomMax = 5000.f;

	//Speed of camera zoom input response 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GuildRunner|Camera", meta = (AllowPrivateAccess = "true"))
	float CameraZoomSpeed = 40.f;

	//Camera zoom interpolation speed
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GuildRunner|Camera", meta = (AllowPrivateAccess = "true"))
	float ZoomInterpolationSpeed = 10.f;
	
	//Speed of camera movement input response
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GuildRunner|Camera", meta = (AllowPrivateAccess = "true"))
	float CameraMoveSpeed = 40.f;

	//Camera movement interpolation speed
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GuildRunner|Camera", meta = (AllowPrivateAccess = "true"))
	float MoveInterpolationSpeed = 10.f;

	//Degrees of camera movement for single button press
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GuildRunner|Camera", meta = (AllowPrivateAccess = "true"))
	float CameraRotationAngle = 45.f;

	//Camera rotation interpolation speed
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GuildRunner|Camera", meta = (AllowPrivateAccess = "true"))
	float RotateInterpolationSpeed = 10.f;

public:
	// Sets default values for this pawn's properties
	AGuildRunnerPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	void CameraZoom(const FInputActionValue& Value);
	void CameraMove(const FInputActionValue& Value);
	void CameraFixedRotate(const FInputActionValue& Value);

	float DesiredZoom;
	FVector DesiredLocation;
	FRotator DesiredRotation;

	bool bInterpolating_CameraZoom = false;
	bool bInterpolating_CameraMove = false;
	bool bInterpolating_CameraRotation = false;
};
