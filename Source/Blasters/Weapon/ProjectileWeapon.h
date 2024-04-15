// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "ProjectileWeapon.generated.h"

/**
 *
 */
UCLASS()
class BLASTERS_API AProjectileWeapon : public AWeapon
{
	GENERATED_BODY()

public:
	virtual void Fire(const FVector &HitTarget) override;

private:
	UPROPERTY(EditAnywhere, Category = "Projectile Weapon")
	TSubclassOf<class AProjectile> ProjectileClass;

	UPROPERTY(EditAnywhere, Category = "Projectile Weapon")
	TSubclassOf<AProjectile> ServerSideRewindProjectileClass;
};
