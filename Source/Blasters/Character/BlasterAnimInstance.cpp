#include "BlasterAnimInstance.h"
#include "BlasterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Blasters/Weapon/Weapon.h"
#include "Blasters/BlasterTypes/CombatState.h"

// This function is called to initialize the animation instance.
void UBlasterAnimInstance::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation(); // Call the parent class's initialization function.

    // Attempt to get a reference to the owning BlasterCharacter.
    BlasterCharacter = Cast<ABlasterCharacter>(TryGetPawnOwner());
}

// This function is called to update the animation instance.
void UBlasterAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
    Super::NativeUpdateAnimation(DeltaTime); // Call the parent class's update function.

    // If the reference to BlasterCharacter is null, attempt to get it again.
    if (BlasterCharacter == nullptr)
    {
        BlasterCharacter = Cast<ABlasterCharacter>(TryGetPawnOwner());
    }
    if (BlasterCharacter == nullptr)
        return; // If still null, return early.

    // Get the character's velocity and calculate its speed.
    FVector Velocity = BlasterCharacter->GetVelocity();
    Velocity.Z = 0.f; // Ignore vertical movement.
    Speed = Velocity.Size();

    // Check if the character is in the air.
    bIsInAir = BlasterCharacter->GetCharacterMovement()->IsFalling();

    // Check if the character is accelerating.
    bIsAccelerating = BlasterCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.0f ? true : false;

    // Check if a weapon is equipped.
    bWeaponEquipped = BlasterCharacter->IsWeaponEquipped();
    EquippedWeapon = BlasterCharacter->GetEquippedWeapon();

    // Check if the character is crouched.
    bIsCrouched = BlasterCharacter->bIsCrouched;

    // Check if the character is aiming.
    bAiming = BlasterCharacter->IsAiming();

    // Get turning in place status.
    TurningInPlace = BlasterCharacter->GetTurningInPlace();

    // Check if root bone should be rotated.
    bRotateRootBone = BlasterCharacter->ShouldRotateRootBone();

    // Check if the character is eliminated.
    bElimmed = BlasterCharacter->IsElimmed();

    // Calculate Yaw offset for aim offset when strafing.
    FRotator AimRotation = BlasterCharacter->GetBaseAimRotation();
    FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(BlasterCharacter->GetVelocity());
    FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation);
    DeltaRotation = FMath::RInterpTo(DeltaRotation, DeltaRot, DeltaTime, 6.f);
    YawOffset = DeltaRotation.Yaw;

    // Calculate lean angle of the character.
    CharacterRotationLastFrame = CharacterRotation;
    CharacterRotation = BlasterCharacter->GetActorRotation();
    const FRotator Delta = UKismetMathLibrary::NormalizedDeltaRotator(CharacterRotation, CharacterRotationLastFrame);
    const float Target = Delta.Yaw / DeltaTime;
    const float Interp = FMath::FInterpTo(Lean, Target, DeltaTime, 6.f);
    Lean = FMath::Clamp(Interp, -90.f, 90.f);

    // Get aim offset yaw and pitch.
    AO_Yaw = BlasterCharacter->GetAO_Yaw();
    AO_Pitch = BlasterCharacter->GetAO_Pitch();

    // Calculate left hand transform if a weapon is equipped.
    if (bWeaponEquipped && EquippedWeapon && EquippedWeapon->GetWeaponMesh() && BlasterCharacter->GetMesh())
    {
        LeftHandTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("LeftHandSocket"), ERelativeTransformSpace::RTS_World);
        FVector OutPosition;
        FRotator OutRotation;
        BlasterCharacter->GetMesh()->TransformToBoneSpace(FName("hand_r"), LeftHandTransform.GetLocation(), FRotator::ZeroRotator, OutPosition, OutRotation);
        LeftHandTransform.SetLocation(OutPosition);
        LeftHandTransform.SetRotation(FQuat(OutRotation));

        // If locally controlled, calculate right hand rotation.
        if (BlasterCharacter->IsLocallyControlled())
        {
            bLocallyControlled = true;
            FTransform RightHandTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("Hand_R"), ERelativeTransformSpace::RTS_World);
            FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(RightHandTransform.GetLocation(), RightHandTransform.GetLocation() + (RightHandTransform.GetLocation() - BlasterCharacter->GetHitTarget()));
            RightHandRotation = FMath::RInterpTo(RightHandRotation, LookAtRotation, DeltaTime, 30.f);
        }
    }

    // Determine whether to use FABRIK based on combat state.
    bUseFABRIK = BlasterCharacter->GetCombatState() == ECombatState::ECS_Unoccupied;

    // Determine whether to use aim offsets based on combat state and gameplay disable state.
    bUseAimOffsets = BlasterCharacter->GetCombatState() == ECombatState::ECS_Unoccupied && !BlasterCharacter->GetDisableGameplay();

    // Determine whether to transform right hand based on combat state and gameplay disable state.
    bTransformRightHand = BlasterCharacter->GetCombatState() == ECombatState::ECS_Unoccupied && !BlasterCharacter->GetDisableGameplay();
}
