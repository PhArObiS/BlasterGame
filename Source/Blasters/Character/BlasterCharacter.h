// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Blasters/BlasterTypes/TurningInPlace.h"
#include "Blasters/Interfaces/InteractWithCrosshairsInterface.h"
#include "Components/TimelineComponent.h"
#include "Blasters/BlasterTypes/CombatState.h"
#include "BlasterCharacter.generated.h"

UCLASS()
class BLASTERS_API ABlasterCharacter : public ACharacter, public IInteractWithCrosshairsInterface
{
	GENERATED_BODY()

public:
	ABlasterCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent *PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const override;
	virtual void PostInitializeComponents() override;
	void PlayFireMontage(bool bIsAiming);
	void PlayReloadMontage();
	void PlayElimMontage();
	void PlayThrowGrenadeMontage();
	virtual void OnRep_ReplicatedMovement() override;
	void Elim();
	UFUNCTION(NetMulticast, Reliable)
	void MulticastElim();
	virtual void Destroyed() override;

	UPROPERTY(Replicated)
	bool bDisableGameplay = false;

	UFUNCTION(BlueprintImplementableEvent, Category = "Crosshairs")
	void ShowSniperScopeWidget(bool bShowScope);

	void UpdateHUDHealth();
	void UpdateHUDShield();
	void UpdateHUDAmmo();

	void SpawnDefaultWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void MoveForward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);
	void LookUp(float Value);
	void EquipButtonPressed();
	void CrouchButtonPressed();
	void ReloadButtonPressed();
	void AimButtonPressed();
	void AimButtonReleased();
	void AimOffset(float DeltaTime);
	void CalculatedAO_Pitch();
	void SimProxiesTurn();
	virtual void Jump() override;
	void FireButtonPressed();
	void FireButtonReleased();
	void PlayHitReactMontage();
	void GrenadeButtonPressed();
	void DropOrDestroyedWeapon(AWeapon *Weapon);
	void DropOrDestroyWeapons();

	UFUNCTION()
	void RecieveDamage(AActor *DamagedActor, float Damage, const UDamageType *DamageType, class AController *InstigatorController, AActor *DamageCauser);

	// Poll for any relevant classes and intialize our HUD
	void PollInit();
	void RotateInPlace(float DeltaTime);

private:
	UPROPERTY(VisibleAnywhere, Category = Camera)
	class USpringArmComponent *CameraBoom;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	class UCameraComponent *FollowCamera;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent *OverheadWidget;

	UPROPERTY(ReplicatedUsing = OnRep_OverlappingWeapon)
	class AWeapon *OverlappingWeapon;

	UFUNCTION()
	void OnRep_OverlappingWeapon(AWeapon *LastWeapon);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UCombatComponent *CombatComponent;

	UPROPERTY(VisibleAnywhere)
	class UBuffComponent *BuffComponent;

	UFUNCTION(Server, Reliable)
	void ServerEquipWeaponButtonPressed();

	float AO_Yaw;
	float InterpAO_Yaw;
	float AO_Pitch;
	FRotator StartingAimRotation;

	ETurningInPlace TurningInPlace;
	void TurnInPlace(float DeltaTime);

	/**
	 * Animation montages for combat
	 */

	UPROPERTY(EditAnywhere, Category = "Combat")
	class UAnimMontage *FireWeaponMontage;

	UPROPERTY(EditAnywhere, Category = "Combat")
	class UAnimMontage *ReloadMontage;

	UPROPERTY(EditAnywhere, Category = "Combat")
	class UAnimMontage *HitReactMontage;

	UPROPERTY(EditAnywhere, Category = "Combat")
	class UAnimMontage *ElimMontage;

	UPROPERTY(EditAnywhere, Category = "Combat")
	class UAnimMontage *ThrowGrenadeMontage;

	void HideCameraIfCharacterClose();

	UPROPERTY(EditAnywhere, Category = "Combat")
	float CameraThreshold = 200.f;

	bool bRotateRootBone;
	float TurnThreshold = 0.5f;
	FRotator ProxyRotationLastFrame;
	FRotator ProxyRotation;
	float ProxyYaw;
	float TimeSinceLastMovementReplication;
	float CalculatedSpeed();

	/**
	 * Player Health
	 */

	UPROPERTY(EditAnywhere, Category = "Player Stats")
	float MaxHealth = 100.f;

	UPROPERTY(ReplicatedUsing = OnRep_Health, VisibleAnywhere, Category = "Player Stats")
	float Health = 100.f;

	UFUNCTION()
	void OnRep_Health(float LastHealth);

	/**
	 * Player Shield
	 */

	UPROPERTY(EditAnywhere, Category = "Player Stats")
	float MaxShield = 100.f;

	UPROPERTY(ReplicatedUsing = OnRep_Shield, EditAnywhere, Category = "Player Stats")
	float Shield = 0.f;

	UFUNCTION()
	void OnRep_Shield(float LastShield);

	UPROPERTY()
	class ABlasterPlayerController *BlasterPlayerController;

	bool bElimmed = false;

	FTimerHandle ElimTimer;

	UPROPERTY(EditDefaultsOnly)
	float ElimDelay = 3.f;

	void ElimTimerFinished();

	// Dissolve effects
	UPROPERTY(VisibleAnywhere)
	UTimelineComponent *DissolveTimeline;
	FOnTimelineFloat DissolveTrack;

	UPROPERTY(EditAnywhere)
	UCurveFloat *DissolveCurve;

	UFUNCTION()
	void UpdateDissolveMaterial(float DissolveValue);
	void StartDissolve();

	// Dyanamic instance that we can change at runtime
	UPROPERTY(VisibleAnywhere, category = Elim)
	UMaterialInstanceDynamic *DynamicDissolveMaterialInstance;

	// Material instance set on the blueprint, used with the dynamic instance
	UPROPERTY(EditAnywhere, category = Elim)
	UMaterialInstance *DissolveMaterialInstance;

	// Elim bot
	UPROPERTY(EditAnywhere)
	UParticleSystem *ElimBotEffect;

	UPROPERTY(VisibleAnywhere)
	UParticleSystemComponent *ElimBotComponent;

	UPROPERTY(EditAnywhere)
	class USoundCue *ElimBotSound;

	UPROPERTY()
	class ABlasterPlayerState *BlasterPlayerState;

	/**
	 * Grenade
	 */

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent *AttachedGrenade;

	/**
	 * Default Weapon
	 */

	UPROPERTY(EditAnywhere)
	TSubclassOf<AWeapon> DefaultWeaponClass;

public:
	void SetOverlappingWeapon(AWeapon *Weapon);
	bool IsWeaponEquipped();
	bool IsAiming();

	FORCEINLINE float GetAO_Yaw() const
	{
		return AO_Yaw;
	}
	FORCEINLINE float GetAO_Pitch() const
	{
		return AO_Pitch;
	}
	AWeapon *GetEquippedWeapon();
	FORCEINLINE ETurningInPlace GetTurningInPlace() const
	{
		return TurningInPlace;
	}
	FVector GetHitTarget() const;
	FORCEINLINE UCameraComponent *GetFollowCamera() const
	{
		return FollowCamera;
	}
	FORCEINLINE bool ShouldRotateRootBone() const
	{
		return bRotateRootBone;
	}
	FORCEINLINE bool IsElimmed() const
	{
		return bElimmed;
	}
	FORCEINLINE float GetHealth() const
	{
		return Health;
	}
	FORCEINLINE void SetHealth(float Amount)
	{
		Health = Amount;
	}
	FORCEINLINE float GetMaxHealth() const
	{
		return MaxHealth;
	}
	FORCEINLINE float GetShield() const
	{
		return Shield;
	}
	FORCEINLINE void SetShield(float Amount)
	{
		Shield = Amount;
	}
	FORCEINLINE float GetMaxShield() const
	{
		return MaxShield;
	}
	ECombatState GetCombatState() const;
	FORCEINLINE UCombatComponent *GetCombatComponent() const { return CombatComponent; }
	FORCEINLINE bool GetDisableGameplay() const
	{
		return bDisableGameplay;
	}
	FORCEINLINE UAnimMontage *GetReloadMontage() const
	{
		return ReloadMontage;
	}
	FORCEINLINE UStaticMeshComponent *GetAttachedGrenade() const
	{
		return AttachedGrenade;
	}
	FORCEINLINE UBuffComponent *GetBuffComponent() const
	{
		return BuffComponent;
	}
};
