// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BaseCharacterAnimInstance.h"
#include "Character/BaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UBaseCharacterAnimInstance::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();

    BaseCharacter = Cast<ABaseCharacter>(TryGetPawnOwner());
}

void UBaseCharacterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{ 
    Super::NativeUpdateAnimation(DeltaSeconds);

    if (!BaseCharacter)
    {
        return;
    }

    FVector Velocity = BaseCharacter->GetVelocity();
    Velocity.Z = 0.0f;
    Speed = Velocity.Size();

    bIsAir = BaseCharacter->GetCharacterMovement()->IsFalling();
    bIsAccelerating = BaseCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.0f ? true : false;

    bWeaponEquip = BaseCharacter->IsWeaponEquipped();
    bIsCrouch = BaseCharacter->bIsCrouched;
    bIsAiming = BaseCharacter->GetIsAiming();

    auto AimRotation = BaseCharacter->GetBaseAimRotation();
    auto MovementRotation = UKismetMathLibrary::MakeRotFromX(BaseCharacter->GetVelocity());
    auto DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation);
    DeltaRotation = FMath::RInterpTo(DeltaRotation, DeltaRot, DeltaSeconds, 5.f);
    YawOffset = DeltaRotation.Yaw;
    
    CharacterRotationLastFrame = CharacterRotation;
    CharacterRotation = BaseCharacter->GetActorRotation();
    const auto Delta = UKismetMathLibrary::NormalizedDeltaRotator(CharacterRotation, CharacterRotationLastFrame);
    const auto Target = Delta.Yaw / DeltaSeconds;
    const float Interp = FMath::FInterpTo(Lean, Target, DeltaSeconds, 6.f);
    Lean = FMath::Clamp(Interp, -90.f, 90.f);
}
