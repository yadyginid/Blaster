// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/BaseWeapon.h"
#include "Character/BaseCharacter.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Net/UnrealNetwork.h"

ABaseWeapon::ABaseWeapon()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>("WeaponMesh");
	SetRootComponent(WeaponMesh);
	
	WeaponMesh->SetCollisionResponseToAllChannels(ECR_Block);
	WeaponMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	AreaSphere = CreateDefaultSubobject<USphereComponent>("AreaSphere");
	AreaSphere->SetupAttachment(GetRootComponent());
	AreaSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	PickupWidget = CreateDefaultSubobject<UWidgetComponent>("PickupWidget");
	PickupWidget->SetupAttachment(GetRootComponent());
}

void ABaseWeapon::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		AreaSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

		AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &ABaseWeapon::OnOverlapBeginAreaSphere);
		AreaSphere->OnComponentEndOverlap.AddDynamic(this, &ABaseWeapon::OnOverlapEndAreaSphere);
	}

	if (PickupWidget)
	{
		PickupWidget->SetVisibility(false);
	}
}

void ABaseWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABaseWeapon, WeaponState);
}

void ABaseWeapon::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void ABaseWeapon::OnOverlapBeginAreaSphere(UPrimitiveComponent* OverlapComponent, AActor* OtherActor,
                                      UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& HitResult)
{
	ABaseCharacter* Character = Cast<ABaseCharacter>(OtherActor);
	if (!Character)
	{
		return;
	}

	Character->SetOverlappingWeapon(this);
}

void ABaseWeapon::OnOverlapEndAreaSphere(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                         UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ABaseCharacter* Character = Cast<ABaseCharacter>(OtherActor);
	if (!Character)
	{
		return;
	}

	Character->SetOverlappingWeapon(nullptr);
}

void ABaseWeapon::ShowPickupWidget(bool bShow)
{
	if (!PickupWidget)
	{
		return;
	}

	PickupWidget->SetVisibility(bShow);
}

//call on client
void ABaseWeapon::OnRep_WeaponState()
{
	switch (WeaponState)
	{
	case EWeaponState::EWS_Equipped :
		ShowPickupWidget(false);
		break;
	}
}

//call on server
void ABaseWeapon::SetWeaponState(const EWeaponState NewWeaponState)
{
	WeaponState = NewWeaponState;

	switch (WeaponState)
	{
		case EWeaponState::EWS_Equipped :
			ShowPickupWidget(false);
		
			if (AreaSphere)
			{
				AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			}
		break;
	}
};
