// Fill out your copyright notice in the Description page of Project Settings.

#include "SpeedPickup.h"
#include "Blasters/Character/BlasterCharacter.h"
#include "Blasters/BlasterComponents/BuffComponent.h"

void ASpeedPickup::OnSphereOverlap(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
    Super::OnSphereOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

    ABlasterCharacter *BlasterCharacter = Cast<ABlasterCharacter>(OtherActor);
    if (BlasterCharacter)
    {
        UBuffComponent *BuffComponent = BlasterCharacter->GetBuffComponent();
        if (BuffComponent)
        {
            BuffComponent->BuffSpeed(BaseSpeedBuff, CrouchSpeedBuff, SpeedBuffTime);
        }
    }

    Destroy();
}
