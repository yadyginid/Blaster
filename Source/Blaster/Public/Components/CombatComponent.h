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
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty>& OutLifetimeProps) const override;
	
	friend class ABaseCharacter;

	void EquipWeapon(ABaseWeapon* WeaponToEquip);
	bool IsAiming() const {return bAiming;}
	
protected:
	virtual void BeginPlay() override;

	void SetAiming(bool NewAimValue);

	UFUNCTION(Server, Reliable)
	void ServerSetAiming(bool LastValuebAming);

	UFUNCTION()
	void OnRep_EquippedWeapon();
	
private:
	UPROPERTY()
	ABaseCharacter* Character;
	
	UPROPERTY(ReplicatedUsing = OnRep_EquippedWeapon)
	ABaseWeapon* EquippedWeapon;

	UPROPERTY(EditDefaultsOnly, Category="Combat")
	FName WeaponSocket = "RightHandSocket";

	UPROPERTY(Replicated)
	bool bAiming;
};
