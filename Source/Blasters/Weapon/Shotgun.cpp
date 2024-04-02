// Fill out your copyright notice in the Description page of Project Settings.

#include "Shotgun.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Blasters/Character/BlasterCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"
#include "Kismet/KismetMathLibrary.h"
#include "WeaponTypes.h"

void AShotgun::Fire(const FVector &HitTarget)
{
    AWeapon::Fire(HitTarget);
    // Get the owner pawn
    APawn *OwnerPawn = Cast<APawn>(GetOwner());
    if (OwnerPawn == nullptr)
    {
        return; // If there is no owner pawn, return
    }
    // Get the instigator controller
    AController *InstigatorController = OwnerPawn->GetController();
    // Get the muzzle flash socket
    const USkeletalMeshSocket *MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName("MuzzleFlash");
    if (MuzzleFlashSocket)
    {
        // Get the transform of the muzzle flash socket
        FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
        FVector Start = SocketTransform.GetLocation();

        TMap<ABlasterCharacter *, uint32> HitMap;
        for (uint32 i = 0; i < NumberOfPellets; i++)
        {
            FHitResult FireHit;
            WeaponTraceHit(Start, HitTarget, FireHit);

            ABlasterCharacter *BlasterCharacter = Cast<ABlasterCharacter>(FireHit.GetActor());
            if (BlasterCharacter && HasAuthority() && InstigatorController)
            {
                if (HitMap.Contains(BlasterCharacter))
                {
                    HitMap[BlasterCharacter]++;
                }
                else
                {
                    HitMap.Emplace(BlasterCharacter, 1);
                }
            }
            if (ImpactParticles)
            {
                UGameplayStatics::SpawnEmitterAtLocation(
                    GetWorld(),
                    ImpactParticles,
                    FireHit.ImpactPoint,
                    FireHit.ImpactNormal.Rotation());
            }
            if (HitSound)
            {
                UGameplayStatics::PlaySoundAtLocation(
                    this,
                    HitSound,
                    FireHit.ImpactPoint,
                    .5f,
                    FMath::FRandRange(-.5f, .5f));
            }
        }

        for (auto HitPair : HitMap)
        {
            if (HitPair.Key && HasAuthority() && InstigatorController)
            {
                UGameplayStatics::ApplyDamage(
                    HitPair.Key,
                    Damage * HitPair.Value,
                    InstigatorController,
                    this,
                    UDamageType::StaticClass());
            }
        }
    }
}
