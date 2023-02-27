// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatComponent.generated.h"

class ABaseWeapon;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BLASTER_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCombatComponent();
	friend class ABaseCharacter;

	void EquipWeapon(ABaseWeapon* WeaponToEquip);
	
protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	ABaseCharacter* Character;
	
	UPROPERTY()
	ABaseWeapon* EquippedWeapon;

	UPROPERTY(EditDefaultsOnly, Category="Combat")
	FName WeaponSocket = "RightHandSocket"; 
};
