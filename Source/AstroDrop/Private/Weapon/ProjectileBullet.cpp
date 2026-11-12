// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/ProjectileBullet.h"

#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"


void AProjectileBullet::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                              FVector NormalImpulse, const FHitResult& Hit)
{
	ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
	if (OwnerCharacter)
	{
		AController* OwnerController = OwnerCharacter->Controller;
		if (OwnerController)
		{
			// UE_LOG(LogTemp, Warning, TEXT("Osso atingido: %s"), *Hit.BoneName.ToString());
			//
			// if (GEngine)
			// {
			// 	GEngine->AddOnScreenDebugMessage(
			// 		-1, 5.f, FColor::Yellow,
			// 		FString::Printf(TEXT("Osso atingido: %s"), *Hit.BoneName.ToString())
			// 	);
			// }
			UGameplayStatics::ApplyDamage(OtherActor, Damage, OwnerController, this, UDamageType::StaticClass());
		}
	}

	Super::OnHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);
}
