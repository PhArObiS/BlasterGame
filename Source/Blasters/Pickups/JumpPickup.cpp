// Fill out your copyright notice in the Description page of Project Settings.

#include "JumpPickup.h"
#include "Blasters/Character/BlasterCharacter.h"
#include "Blasters/BlasterComponents/BuffComponent.h"

void AJumpPickup::OnSphereOverlap(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
    Super::OnSphereOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

    ABlasterCharacter *BlasterCharacter = Cast<ABlasterCharacter>(OtherActor);
    if (BlasterCharacter)
    {
        UBuffComponent *BuffComponent = BlasterCharacter->GetBuffComponent();
        if (BuffComponent)
        {
            BuffComponent->BuffJump(JumpZVelocityBuff, JumpBuffTime);
        }
    }

    Destroy();
}