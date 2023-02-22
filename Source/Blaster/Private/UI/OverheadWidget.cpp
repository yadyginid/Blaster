// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/OverheadWidget.h"
#include "Components/TextBlock.h"

void UOverheadWidget::SetDisplayText(FString TextToDisplay)
{
	if (!DisplayText)
	{
		return;
	}

	DisplayText->SetText(FText::FromString(TextToDisplay));
}

void UOverheadWidget::ShowPlayerNetRole(const APawn* InPawn)
{
	const ENetRole LocalRole = InPawn->GetLocalRole();
	FString Role;
	
	switch (LocalRole)
	{
	case ENetRole::ROLE_Authority:
		Role = FString("Authority");
		break;

	case ENetRole::ROLE_AutonomousProxy:
		Role = FString("AutonomousProxy");
		break;

	case ENetRole::ROLE_SimulatedProxy:
		Role = FString("SimulatedProxy");
		break;

	case ENetRole::ROLE_None:
		Role = FString("ROLE_None");
		break;
		
	default:
	Role = FString("ROLE Unknown");
		break;;
	}

	const FString LocalRoleString = FString::Printf(TEXT("local role %s "), *Role);
	SetDisplayText(LocalRoleString);
}

void UOverheadWidget::OnLevelRemovedFromWorld(ULevel* InLevel, UWorld* InWorld)
{
	RemoveFromParent();
	Super::OnLevelRemovedFromWorld(InLevel, InWorld);
}
