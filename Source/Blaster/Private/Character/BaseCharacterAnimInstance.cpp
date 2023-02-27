// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BaseCharacterAnimInstance.h"
#include "Character/BaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

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
}
