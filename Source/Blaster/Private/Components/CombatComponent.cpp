// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/CombatComponent.h"
#include "Character/BaseCharacter.h"
#include "Weapon/BaseWeapon.h"

UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();
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
}

