// Fill out your copyright notice in the Description page of Project Settings.


#include "Menu.h"
#include "Components/Button.h"
#include "MultiplayerSessionsSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"

bool UMenu::Initialize()
{
	if (!Super::Initialize())
	{
		return false;
	}
	
	if (HostButton)
	{
		HostButton->OnClicked.AddDynamic(this, &UMenu::HostButtonClick);
	}

	if (JoinButton)
	{
		JoinButton->OnClicked.AddDynamic(this, &UMenu::JoinButtonClick);
	}

	if (MatchTypeText)
	{
		MatchTypeText->OnTextChanged.AddDynamic(this, &UMenu::ChangeMatchTypeText);
	}

	MatchType = MatchTypeText->Text.ToString();
	
	return true;
}

void UMenu::MenuSetup(int32 NumPublicConnectionsValue, FString LobbyPath)
{
	NumPublicConnections = NumPublicConnectionsValue;
	PathToLobby = FString::Printf(TEXT("%s?listen"), *LobbyPath);
	
	AddToViewport();
	SetVisibility(ESlateVisibility::Visible);
	bIsFocusable = true;

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	APlayerController* PlayerController = World->GetFirstPlayerController();
	if (!PlayerController)
	{
		return;
	}

	FInputModeUIOnly InputMode;
	InputMode.SetWidgetToFocus(TakeWidget());
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	
	PlayerController->SetInputMode(InputMode);
	PlayerController->SetShowMouseCursor(true);

	const UGameInstance* GameInstance = GetGameInstance();
	if (!GameInstance)
	{
		return;
	}

	MultiplayerSessionsSubsystem = GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();

	if (!MultiplayerSessionsSubsystem)
	{
		return;
	}

	MultiplayerSessionsSubsystem->MultiplayerOnCreateSessionComplete.AddDynamic(this, &UMenu::UMenu::OnCreateSession);
	MultiplayerSessionsSubsystem->MultiplayerOnFindSessionsComplete.AddUObject(this, &UMenu::OnFindSessions);
	MultiplayerSessionsSubsystem->MultiplayerOnJoinSessionComplete.AddUObject(this, &UMenu::OnJoinSession);
	MultiplayerSessionsSubsystem->MultiplayerOnDestroySessionComplete.AddDynamic(this, &UMenu::OnDestroySession);
	MultiplayerSessionsSubsystem->MultiplayerOnStartSessionComplete.AddDynamic(this, &UMenu::OnStartSession);
}

void UMenu::OnLevelRemovedFromWorld(ULevel* InLevel, UWorld* InWorld)
{
	MenuTearDown();
	Super::OnLevelRemovedFromWorld(InLevel, InWorld);
}

void UMenu::OnCreateSession(bool bWasSuccessful)
{
	GetWorld()->ServerTravel(PathToLobby);

	if (!bWasSuccessful)
	{
		HostButton->SetIsEnabled(true);
	}
}

void UMenu::OnFindSessions(const TArray<FOnlineSessionSearchResult>& SessionsResult, bool bWasSuccessful)
{
	if (!MultiplayerSessionsSubsystem)
	{
		return;
	}
	
	for (auto Result : SessionsResult)
	{
		FString SettingsValue;
		Result.Session.SessionSettings.Get(FName("MatchType"),  SettingsValue);

		if (SettingsValue == MatchType)
		{
			MultiplayerSessionsSubsystem->JoinSession(Result);
			return;
		}
	}

	if (!bWasSuccessful || SessionsResult.Num() == 0)
	{
		JoinButton->SetIsEnabled(true);
	}
}

void UMenu::OnJoinSession(EOnJoinSessionCompleteResult::Type Result)
{
	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
	if (OnlineSubsystem)
	{
		auto SessionInterface = OnlineSubsystem->GetSessionInterface();
		if (!SessionInterface)
		{
			return;
		}

		FString Address;
		SessionInterface->GetResolvedConnectString(NAME_GameSession, Address);

		APlayerController* PlayerController = GetGameInstance()->GetFirstLocalPlayerController();
		if (!PlayerController)
		{
			return;
		}
	
		PlayerController->ClientTravel(Address, TRAVEL_Absolute);
	}

	if (Result != EOnJoinSessionCompleteResult::Success)
	{
		JoinButton->SetIsEnabled(true);
	}
}

void UMenu::OnDestroySession(bool bWasSuccessful)
{
}

void UMenu::OnStartSession(bool bWasSuccessful)
{
}

void UMenu::HostButtonClick()
{
	if (!MultiplayerSessionsSubsystem || !GetWorld())
	{
		return;
	}

	HostButton->SetIsEnabled(false);
	MultiplayerSessionsSubsystem->CreateSession(NumPublicConnections, MatchType);
}

void UMenu::JoinButtonClick()
{
	if (!MultiplayerSessionsSubsystem)
	{
		return;
	}

	JoinButton->SetIsEnabled(false);
	MultiplayerSessionsSubsystem->FindSessions(10000, MatchType);
}

void UMenu::MenuTearDown()
{
	RemoveFromParent();

	const auto World = GetWorld();
	if (!World)
	{
		return;
	}

	const auto PlayerController = World->GetFirstPlayerController();
	if(!PlayerController)
	{
		return;
	}

	FInputModeGameOnly InputMode;
	PlayerController->SetInputMode(InputMode);
	PlayerController->SetShowMouseCursor(false);
}

void UMenu::ChangeMatchTypeText(const FText& Text)
{
	MatchType = MatchTypeText->GetText().ToString();
}

