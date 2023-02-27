// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/CombatComponent.h"
#include "GameFramework/Character.h"
#include "BaseCharacter.generated.h"

class UCombatComponent;
class ABaseWeapon;
class UWidgetComponent;
class UCameraComponent;
class USpringArmComponent;

UCLASS()
class BLASTER_API ABaseCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    ABaseCharacter();
    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty>& OutLifetimeProps) const override;

    void SetOverlappingWeapon(ABaseWeapon* PickupWeapon);
    virtual void PostInitializeComponents() override;

    UFUNCTION(Server, Reliable)
    void ServerEquipButtonPressed();

    bool IsWeaponEquipped();
    bool GetIsAiming() const {return CombatComponent && CombatComponent->IsAiming();}
    
protected:
    virtual void BeginPlay() override;
    
    void MoveForward(float Amount);
    void MoveRight(float Amount);
    void Turn(float Amount);
    void LookUp(float Amount);

    void EquipButtonPressed();
    void CrouchButtonPressed();

    void AimButtonPressed();
    void AimButtonReleased();

private:
    UPROPERTY(VisibleAnywhere, Category = Camera)
    USpringArmComponent* CameraBoom;

    UPROPERTY(VisibleAnywhere, Category = Camera)
    UCameraComponent* FollowCamera;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    UWidgetComponent* OverheadWidget;

    UPROPERTY(ReplicatedUsing = OnRep_OverlappingWeapon)
    ABaseWeapon* OverlappingWeapon;

    UPROPERTY(VisibleAnywhere)
    UCombatComponent* CombatComponent;
    
    UFUNCTION()
    void OnRep_OverlappingWeapon(ABaseWeapon* LastOverlappingWeapon);
};
