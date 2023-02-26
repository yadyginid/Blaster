// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BaseCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Net/UnrealNetwork.h"
#include "Weapon/BaseWeapon.h"

ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

    CameraBoom = CreateDefaultSubobject<USpringArmComponent>("CameraBoom");
    CameraBoom->SetupAttachment(GetMesh());
    CameraBoom->TargetArmLength = 600.0f;
    CameraBoom->bUsePawnControlRotation = true;
    
    FollowCamera = CreateDefaultSubobject<UCameraComponent>("FollowCamera");
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    FollowCamera->bUsePawnControlRotation = false;

    bUseControllerRotationYaw = false;
    GetCharacterMovement()->bOrientRotationToMovement = true;

    OverheadWidget = CreateDefaultSubobject<UWidgetComponent>("OverheadWidget");
    OverheadWidget->SetupAttachment(GetRootComponent());
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

//call only for client
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