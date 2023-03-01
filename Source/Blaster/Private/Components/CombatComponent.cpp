// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/CombatComponent.h"
#include "Character/BaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "Weapon/BaseWeapon.h"

UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCombatComponent, EquippedWeapon);
	DOREPLIFETIME(UCombatComponent, bAiming);
}

void UCombatComponent::EquipWeapon(ABaseWeapon* WeaponToEquip)
{
	const auto OwnerCharacter = Cast<ABaseCharacter>(GetOwner());
	if (!WeaponToEquip || !OwnerCharacter)
	{
		return;
	}

	EquippedWeapon = WeaponToEquip;
	EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);

	const FAttachmentTransformRules AttachmentTransformRules(EAttachmentRule::SnapToTarget, false);
	EquippedWeapon->AttachToComponent(OwnerCharacter->GetMesh(), AttachmentTransformRules, WeaponSocket);
	EquippedWeapon->SetOwner(OwnerCharacter);
	OwnerCharacter->GetCharacterMovement()->bOrientRotationToMovement = false;
	OwnerCharacter->bUseControllerRotationYaw = true;
}


void UCombatComponent::SetAiming(bool NewAimValue)
{
	bAiming = NewAimValue;
	ServerSetAiming(bAiming);
}

void UCombatComponent::ServerSetAiming_Implementation(bool LastValuebAming)
{
	bAiming = LastValuebAming;
}

void UCombatComponent::OnRep_EquippedWeapon()
{
	if (EquippedWeapon && Character)
	{
		Character->GetCharacterMovement()->bOrientRotationToMovement = false;
		Character->bUseControllerRotationYaw = true;
	}
}