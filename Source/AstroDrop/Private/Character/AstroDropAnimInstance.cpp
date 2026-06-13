// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/AstroDropAnimInstance.h"
#include "Character/AstroDropCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"


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
}
