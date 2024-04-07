// Fill out your copyright notice in the Description page of Project Settings.

#include "ShieldPickup.h"
#include "Blasters/Character/BlasterCharacter.h"
#include "Blasters/BlasterComponents/BuffComponent.h"

void AShieldPickup::OnSphereOverlap(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
    Super::OnSphereOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

    ABlasterCharacter *BlasterCharacter = Cast<ABlasterCharacter>(OtherActor);
    if (BlasterCharacter)
    {
        UBuffComponent *BuffComponent = BlasterCharacter->GetBuffComponent();
        if (BuffComponent)
        {
            BuffComponent->ReplenishShield(ShieldReplenishAmount, ShieldReplenishTime);
        }
    }

    Destroy();
}