// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Blasters/HUD/BlasterHUD.h"
#include "Blasters/Weapon/WeaponTypes.h"
#include "Blasters/BlasterTypes/CombatState.h"
#include "CombatComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class BLASTERS_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCombatComponent();
	friend class ABlasterCharacter;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const override;

	void EquipWeapon(class AWeapon *WeaponToEquip);

	void Reload();

	UFUNCTION(BlueprintCallable)
	void FinishReloading();

	void FireButtonPressed(bool bIsPressed);

	UFUNCTION(BlueprintCallable)
	void ShotgunShellReload();

	void JumpToShotgunEnd();

	UFUNCTION(BlueprintCallable)
	void ThrowGrenadeFinished();

protected:
	virtual void BeginPlay() override;
	void SetAiming(bool bIsAiming);

	UFUNCTION(Server, Reliable)
	void ServerSetAiming(bool bIsAiming);

	UFUNCTION()
	void OnRep_EquippedWeapon();

	void Fire();

	UFUNCTION(Server, Reliable)
	void ServerFire(const FVector_NetQuantize &TraceHitTarget);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastFire(const FVector_NetQuantize &TraceHitTarget);

	UFUNCTION()
	void TraceUnderCrosshairs(FHitResult &TraceHitResult);

	void SetHUDCrosshairs(float DeltaTime);

	UFUNCTION(Server, Reliable)
	void ServerReload();

	void HandleReload();
	int32 AmountToReload();

	void ThrowGrenade();

	UFUNCTION(Server, Reliable)
	void ServerThrowGrenade();

	void DropEquippedWeapon();
	void AttachActorToRightHand(AActor *ActorToAttach);
	void AttachActorToLeftHand(AActor *ActorToAttach);
	void UpdateCarriedAmmo();
	void PlayEquippedWeaponSound();
	void ReloadEmptyWeapon();

private:
	UPROPERTY()
	class ABlasterCharacter *Character;
	UPROPERTY()
	class ABlasterPlayerController *Controller;
	UPROPERTY()
	class ABlasterHUD *HUD;

	UPROPERTY(ReplicatedUsing = OnRep_EquippedWeapon)
	AWeapon *EquippedWeapon;

	UPROPERTY(Replicated)
	bool bAiming;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float BaseWalkSpeed;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float AimWalkSpeed;

	// Fire related variables
	bool bFireButtonPressed;

	// Crosshair and HUD related variables
	float CrosshairVelocityFactor;
	float CrosshairInAirFactor;
	float CrosshairAimFactor;
	float CrosshairShootingFactor;

	FVector HitTarget;

	FHUDPackage HUDPackage;

	// Aiming and FOV related variables
	// Field of view when not aiming; set to the camera's base FOV in BeginPlay
	float DefaultFOV;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float ZoomedFOV = 30.f;

	float CurrentFOV;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float ZoomInterpSpeed = 20.f;

	void InterpFOV(float DeltaTime);

	bool bCanFire = true;
	// Automatic fire related variables
	FTimerHandle FireTimer;
	FTimerHandle ReloadTimer;

	void StartFireTimer();
	void FireTimerFinished();

	void ClearTimers();

	bool CanFire();

	// Carried ammo for the currently equipped weapon
	UPROPERTY(ReplicatedUsing = OnRep_CarriedAmmo)
	int32 CarriedAmmo;

	UFUNCTION()
	void OnRep_CarriedAmmo();

	TMap<EWeaponType, int32> CarriedAmmoMap;

	UPROPERTY(EditAnywhere, Category = "Combat")
	int32 StartingARAmmo = 60;

	UPROPERTY(EditAnywhere, Category = "Combat")
	int32 StartingRocketAmmo = 0;

	UPROPERTY(EditAnywhere, Category = "Combat")
	int32 StartingPistolAmmo = 0;

	UPROPERTY(EditAnywhere, Category = "Combat")
	int32 StartingSMGAmmo = 0;

	UPROPERTY(EditAnywhere, Category = "Combat")
	int32 StartingShotgunAmmo = 0;

	UPROPERTY(EditAnywhere, Category = "Combat")
	int32 StartingSniperAmmo = 0;

	UPROPERTY(EditAnywhere, Category = "Combat")
	int32 StartingGrenadeLauncherAmmo = 0;

	void InitializeCarriedAmmo();

	UPROPERTY(ReplicatedUsing = OnRep_CombatState)
	ECombatState CombatState = ECombatState::ECS_Unoccupied;

	UFUNCTION()
	void OnRep_CombatState();

	void UpdateAmmoValues();
	void UpdateShotgunAmmoValues();

public:
};
