// Include necessary headers
#include "HitScanWeapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Blasters/Character/BlasterCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"
#include "Kismet/KismetMathLibrary.h"
#include "WeaponTypes.h"

#include "DrawDebugHelpers.h"

// Fire function implementation
void AHitScanWeapon::Fire(const FVector &HitTarget)
{
    // Call the parent class's Fire function
    Super::Fire(HitTarget);

    // Get the owner pawn
    APawn *OwnerPawn = Cast<APawn>(GetOwner());
    if (OwnerPawn == nullptr)
    {
        return; // If there is no owner pawn, return
    }
    // Get the instigator controller
    AController *InstigatorController = OwnerPawn->GetController();

    // Get the muzzle flash socket ----------------------------------------------
    const USkeletalMeshSocket *MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName("MuzzleFlash");
    if (MuzzleFlashSocket)
    {
        // Get the transform of the muzzle flash socket
        FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
        FVector Start = SocketTransform.GetLocation();

        // Perform line trace
        FHitResult FireHit;
        WeaponTraceHit(Start, HitTarget, FireHit);

        // Apply damage if hit a valid character
        ABlasterCharacter *BlasterCharacter = Cast<ABlasterCharacter>(FireHit.GetActor());
        if (BlasterCharacter && HasAuthority() && InstigatorController)
        {
            UGameplayStatics::ApplyDamage(
                BlasterCharacter,
                Damage,
                InstigatorController,
                this,
                UDamageType::StaticClass());
        }
        // Spawn impact particles at the hit point
        if (ImpactParticles)
        {
            UGameplayStatics::SpawnEmitterAtLocation(
                GetWorld(),
                ImpactParticles,
                FireHit.ImpactPoint,
                FireHit.ImpactNormal.Rotation());
        }
        // Play hit sound at the hit point ----------------------------------------
        if (HitSound)
        {
            UGameplayStatics::PlaySoundAtLocation(
                this,
                HitSound,
                FireHit.ImpactPoint);
        }

        if (MuzzleFlash)
        {
            UGameplayStatics::SpawnEmitterAtLocation(
                GetWorld(),
                MuzzleFlash,
                SocketTransform);
        }
        if (FireSound)
        {
            UGameplayStatics::PlaySoundAtLocation(
                this,
                FireSound,
                GetActorLocation());
        }
    }
}

void AHitScanWeapon::WeaponTraceHit(const FVector &TraceStart, const FVector &HitTarget, FHitResult &OutHit)
{
    UWorld *World = GetWorld();
    if (World)
    {
        FVector End = bUseScatter ? TraceEndWithScatter(TraceStart, HitTarget) : TraceStart + (HitTarget - TraceStart) * 1.25f;

        World->LineTraceSingleByChannel(
            OutHit,
            TraceStart,
            End,
            ECollisionChannel::ECC_Visibility);
        FVector BeamEnd = End;
        if (OutHit.bBlockingHit)
        {
            BeamEnd = OutHit.ImpactPoint;
        }
        else
        {
            OutHit.ImpactPoint = End;
        }

        if (BeamParticles)
        {
            UParticleSystemComponent *Beam = UGameplayStatics::SpawnEmitterAtLocation(
                World,
                BeamParticles,
                TraceStart,
                FRotator::ZeroRotator,
                true);
            if (Beam)
            {
                Beam->SetVectorParameter(FName("Target"), BeamEnd);
            }
        }
    }
}

FVector AHitScanWeapon::TraceEndWithScatter(const FVector &TraceStart, const FVector &HitTarget)
{
    FVector ToTargetNormalized = (HitTarget - TraceStart).GetSafeNormal();
    FVector SphereCenter = TraceStart + ToTargetNormalized * DistanceToSphere;
    FVector RandVec = UKismetMathLibrary::RandomUnitVector() * FMath::FRandRange(0.f, SphereRadius);
    FVector EndLoc = SphereCenter + RandVec;
    FVector ToEndLoc = EndLoc - TraceStart;
    /*
    DrawDebugSphere(
        GetWorld(),
        SphereCenter,
        SphereRadius,
        12,
        FColor::Red,
        true);

    DrawDebugSphere(
        GetWorld(),
        EndLoc,
        5.f,
        12,
        FColor::Cyan,
        true);

    DrawDebugLine(
        GetWorld(),
        TraceStart,
        FVector(TraceStart + ToEndLoc * TRACE_LENGTH / ToEndLoc.Size()),
        FColor::Orange,
        true);
*/
    return FVector(TraceStart + ToEndLoc * TRACE_LENGTH / ToEndLoc.Size());
}
