// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/AstroDropAnimInstance.h"
#include "Character/AstroDropCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"


void UAstroDropAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	AstroDropCharacter = Cast<AAstroDropCharacter>(TryGetPawnOwner());
}

void UAstroDropAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);
	if (AstroDropCharacter == nullptr)
	{
		AstroDropCharacter = Cast<AAstroDropCharacter>(TryGetPawnOwner());
	}
	if (AstroDropCharacter == nullptr) return;

	FVector Velocity = AstroDropCharacter->GetVelocity();
	Velocity.Z = 0.f;
	Speed = Velocity.Size();

	bIsInAir = AstroDropCharacter->GetCharacterMovement()->IsFalling();
	bIsAccelerating = AstroDropCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f ? true : false;
	bWeaponEquipped = AstroDropCharacter->IsWeaponEquipped();
	bIsCrouched = AstroDropCharacter->bIsCrouched;
	bAiming = AstroDropCharacter->IsAiming();
	
	// Offset Yaw for Strafing
	FRotator AimRotation = AstroDropCharacter->GetBaseAimRotation();
	FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(AstroDropCharacter->GetVelocity());
	YawOffset = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation).Yaw;

	CharacterRotationLastFrame = CharacterRotation;
	CharacterRotation = AstroDropCharacter->GetActorRotation();
	const FRotator Delta = UKismetMathLibrary::NormalizedDeltaRotator(CharacterRotation, CharacterRotationLastFrame);
	const float Target = Delta.Yaw / DeltaTime;
	const float Interp = FMath::FInterpTo(Lean, Target, DeltaTime, 6.f);
	Lean = FMath::Clamp(Interp, -90.f, 90.f);
}
