// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BaseCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CombatComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Net/UnrealNetwork.h"
#include "Weapon/BaseWeapon.h"

ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
    bUseControllerRotationYaw = false;
    
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>("CameraBoom");
    CameraBoom->SetupAttachment(GetMesh());
    CameraBoom->TargetArmLength = 600.0f;
    CameraBoom->bUsePawnControlRotation = true;
    
    FollowCamera = CreateDefaultSubobject<UCameraComponent>("FollowCamera");
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    FollowCamera->bUsePawnControlRotation = false;
    
    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
    
    OverheadWidget = CreateDefaultSubobject<UWidgetComponent>("OverheadWidget");
    OverheadWidget->SetupAttachment(GetRootComponent());

    CombatComponent = CreateDefaultSubobject<UCombatComponent>("CombatComponent");
    CombatComponent->SetIsReplicated(true);
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
    
    PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
    PlayerInputComponent->BindAction("Equip", IE_Pressed, this, &ABaseCharacter::EquipButtonPressed);
    PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ABaseCharacter::CrouchButtonPressed);

    PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &ABaseCharacter::AimButtonPressed);
    PlayerInputComponent->BindAction("Aim",IE_Released, this, &ABaseCharacter::AimButtonReleased);
    
    PlayerInputComponent->BindAxis("MoveForward", this, &ABaseCharacter::MoveForward);
    PlayerInputComponent->BindAxis("MoveRight", this, &ABaseCharacter::MoveRight);
    PlayerInputComponent->BindAxis("LookUp", this, &ABaseCharacter::LookUp);
    PlayerInputComponent->BindAxis("Turn", this, &ABaseCharacter::Turn);
}

void ABaseCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME_CONDITION(ABaseCharacter, OverlappingWeapon, COND_OwnerOnly);
}

void ABaseCharacter::MoveForward(float Amount)
{
    if (!Controller || Amount == 0.0f)
    {
        return;
    }

    const FRotator YawRotation(0.0f, Controller->GetControlRotation().Yaw, 0.f);
    const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X));

    AddMovementInput(Direction, Amount);
}

void ABaseCharacter::MoveRight(float Amount)
{
    if (!Controller || Amount == 0.0f)
    {
        return;
    }

    const FRotator YawRotation(0.0f, Controller->GetControlRotation().Yaw, 0.f);
    const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y));

    AddMovementInput(Direction, Amount);
}

void ABaseCharacter::Turn(float Amount)
{
    if (!Controller || Amount == 0.0f)
    {
        return;
    }

    AddControllerYawInput(Amount);
}

void ABaseCharacter::LookUp(float Amount)
{
    if (!Controller || Amount == 0.0f)
    {
        return;
    }

    AddControllerPitchInput(Amount);
}

void ABaseCharacter::EquipButtonPressed()
{
    if (!CombatComponent)
    {
        return;
    }

    if (HasAuthority())
    {
        CombatComponent->EquipWeapon(OverlappingWeapon);
    }
    else
    {
        //client call ServerEquipButtonPressed_Implementation()
       ServerEquipButtonPressed();
    }
}

void ABaseCharacter::CrouchButtonPressed()
{
    bIsCrouched ? UnCrouch() : Crouch();
}

void ABaseCharacter::OnRep_OverlappingWeapon(ABaseWeapon* LastOverlappingWeapon)
{
    if (OverlappingWeapon)
    {
        OverlappingWeapon->ShowPickupWidget(true);
    }
    
    if (LastOverlappingWeapon)
    {
        LastOverlappingWeapon->ShowPickupWidget(false);
    }
}

void ABaseCharacter::SetOverlappingWeapon(ABaseWeapon* PickupWeapon)
{
    const auto PickupWidget = PickupWeapon != nullptr ? PickupWeapon : OverlappingWeapon;

    if (IsLocallyControlled())
    {
        const bool bShowWidget = PickupWeapon != nullptr ? true : false;
        PickupWidget->ShowPickupWidget(bShowWidget);
    } 

    OverlappingWeapon = PickupWeapon;
}

void ABaseCharacter::PostInitializeComponents()
{
    Super::PostInitializeComponents();

    if (CombatComponent)
    {
        CombatComponent->Character = this;
    }
}

void ABaseCharacter::ServerEquipButtonPressed_Implementation()
{
    if (!CombatComponent)
    {
        return;
    }
    
    CombatComponent->EquipWeapon(OverlappingWeapon);
}

bool ABaseCharacter::IsWeaponEquipped()
{
   return (CombatComponent && CombatComponent->EquippedWeapon);
}

void ABaseCharacter::AimButtonPressed()
{
    if (!CombatComponent)
    {
        return;
    }

    CombatComponent->SetAiming(true);
}

void ABaseCharacter::AimButtonReleased()
{
    if (!CombatComponent)
    {
        return;
    }

    CombatComponent->SetAiming(false);
}