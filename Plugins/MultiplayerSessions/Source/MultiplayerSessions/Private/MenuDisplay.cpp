// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuDisplay.h"
#include "Components/Button.h"
#include "MultiplayerSessionsSubsystem.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"

void UMenuDisplay::MenuSetup(int32 NumberOfPublicConnections, FString TypeOfMatch, FString LobbyPath)
{
    PathToLobby = FString::Printf(TEXT("%s?listen"), *LobbyPath);  
    NumPublicConnections = NumberOfPublicConnections;
    MatchType = TypeOfMatch;
    AddToViewport();
    SetVisibility(ESlateVisibility::Visible);
    bIsFocusable = true; 

    UWorld* World = GetWorld();
    if (World)
    {
        APlayerController* PlayerController = World->GetFirstPlayerController();
        if (PlayerController)
        {
            FInputModeUIOnly InputModeData;
            InputModeData.SetWidgetToFocus(TakeWidget());
            InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
            PlayerController->SetInputMode(InputModeData);
            PlayerController->SetShowMouseCursor(true);
        }
    }

    // Get the game instance
    UGameInstance* GameInstance = GetGameInstance();
    if (GameInstance)
    {
        // Get the multiplayer sessions subsystem from the game instance
        MultiplayerSessionsSubsystem = GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();
    }

    if (MultiplayerSessionsSubsystem)
    {
        MultiplayerSessionsSubsystem->MultiplayerOnCreateSessionComplete.AddDynamic(this, &ThisClass::OnCreateSession);
        MultiplayerSessionsSubsystem->MultiplayerOnFindSessionsComplete.AddUObject(this, &ThisClass::OnFindSessions);
        MultiplayerSessionsSubsystem->MultiplayerOnJoinSessionComplete.AddUObject(this, &ThisClass::OnJoinSession);
        MultiplayerSessionsSubsystem->MultiplayerOnDestroySessionComplete.AddDynamic(this, &ThisClass::OnDestroySession);
        MultiplayerSessionsSubsystem->MultiplayerOnStartSessionComplete.AddDynamic(this, &ThisClass::OnStartSession);
    }  

}

bool UMenuDisplay::Initialize()
{
    // Call the parent class Initialize function
    if (!Super::Initialize())
    {
        return false;
    }

    // Bind the HostButton to the HostButtonClicked function
    if (HostButton)
    {
        HostButton->OnClicked.AddDynamic(this, &ThisClass::HostButtonClicked);
    }

    // Bind the JoinButton to the JoinButtonClicked function
    if (JoinButton)
    {
        JoinButton->OnClicked.AddDynamic(this, &ThisClass::JoinButtonClicked);
    }

    return true;
}

void UMenuDisplay::NativeDestruct()
{

    MenuTearDown();
    Super::NativeDestruct();
}

void UMenuDisplay::OnCreateSession(bool bWasSuccesfull)
{
    if (bWasSuccesfull)
    {
        // Display a debug message when the session is created
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Yellow, FString(TEXT("Session Created Successfully")));
        }

        UWorld* World = GetWorld();
        if (World)
        {
            World->ServerTravel(PathToLobby);
        }
    }
    else{
        // Display a debug message when the session is not created
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Yellow, FString(TEXT("Failed to create session")));
        }
        HostButton->SetIsEnabled(true);
    }
}

void UMenuDisplay::OnFindSessions(const TArray<FOnlineSessionSearchResult> &SessionResults, bool bWasSuccesfull)
{
    if (MultiplayerSessionsSubsystem == nullptr)
    {
        return;
    }

    for (auto Result : SessionResults)
    {
        FString SettingsValue;
        Result.Session.SessionSettings.Get(FName("MatchType"), SettingsValue);
        if (SettingsValue == MatchType)
        {
            MultiplayerSessionsSubsystem->JoinSession(Result);
            return;
        }
    }
    if (!bWasSuccesfull || SessionResults.Num() == 0)
    {
        JoinButton->SetIsEnabled(true);
    }
}

void UMenuDisplay::OnJoinSession(EOnJoinSessionCompleteResult::Type Result)
{
    IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
    if (Subsystem)
    {
        IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();
        if (SessionInterface.IsValid())
        {
            FString Address;
            SessionInterface->GetResolvedConnectString("NAME_GameSession", Address);

            APlayerController* PlayerController = GetGameInstance()->GetFirstLocalPlayerController();
            if (PlayerController)
            {
                PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
            }

        }
    }
    if (Result != EOnJoinSessionCompleteResult::Success)
    {
        JoinButton->SetIsEnabled(true);
    }
}

void UMenuDisplay::OnDestroySession(bool bWasSuccesfull)
{
}

void UMenuDisplay::OnStartSession(bool bWasSuccesfull)
{
}

void UMenuDisplay::HostButtonClicked()
{
    HostButton->SetIsEnabled(false);
    // If the multiplayer sessions subsystem is available, create a session
    if (MultiplayerSessionsSubsystem)
    {
        MultiplayerSessionsSubsystem->CreateSession(NumPublicConnections, MatchType);
    }
}

void UMenuDisplay::JoinButtonClicked()
{
    JoinButton->SetIsEnabled(false);
    // Find an active game session
    if (MultiplayerSessionsSubsystem)
    {
        MultiplayerSessionsSubsystem->FindSessions(10000);

    }
}

void UMenuDisplay::MenuTearDown()
{
    RemoveFromParent();
    UWorld* World = GetWorld();
    if (World)
    {
        APlayerController* PlayerController = World->GetFirstPlayerController();
        if (PlayerController)
        {
            FInputModeGameOnly InputModeData;
            PlayerController->SetInputMode(InputModeData);
            PlayerController->SetShowMouseCursor(false);
        }
    }   
}
