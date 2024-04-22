// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "Flag.generated.h"

/**
 *
 */
UCLASS()
class BLASTERS_API AFlag : public AWeapon
{
	GENERATED_BODY()

public:
	AFlag();
	virtual void DroppedWeapon() override;

protected:
	void OnEquipped() override;
	void OnDropped() override;

private:
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent *FlagMesh;
};
