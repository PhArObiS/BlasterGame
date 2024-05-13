// Fill out your copyright notice in the Description page of Project Settings.


#include "OverheadWidget.h"
#include "Components/TextBlock.h"
#include "Blasters/PlayerState/BlasterPlayerState.h"
#

void UOverheadWidget::SetDisplayText(FString TextToDisplay)
{
    if (DisplayText)
    {
        DisplayText->SetText(FText::FromString(TextToDisplay));
    }
}

void UOverheadWidget::ShowPlayerNetRole(APawn* InPawn)
{
    ENetRole RemoteRole = InPawn->GetRemoteRole();
    FString Role;
    switch (RemoteRole)
    {
        case ENetRole::ROLE_Authority:
            Role = FString("Authority");
            break;
        case ENetRole::ROLE_AutonomousProxy:
            Role = FString("Autonomous Proxy");
            break;
        case ENetRole::ROLE_SimulatedProxy:
            Role = FString("Simulated Proxy");
            break;
        case ENetRole::ROLE_None:
            Role = FString("None");
            break;
    }
    FString RemoteRoleString = FString::Printf(TEXT("Remote Role: %s"), *Role);
    SetDisplayText(RemoteRoleString);
}

void UOverheadWidget::DisplayPlayerName(APawn* NewPlayer)
{
    if (NewPlayer)
    {
        APlayerState* PlayerState = NewPlayer->GetPlayerState();
        if (PlayerState)
        {
            FString PlayerName = FString::Printf(TEXT("%s"), *PlayerState->GetPlayerName());
            SetDisplayText(PlayerName);

            //Clear the timer if player status obtained
            GetWorld()->GetTimerManager().ClearTimer(PlayerNameTimerHandle);
        }
        else if (!PlayerNameTimerHandle.IsValid())
        {
            // Start the timer if it's not already running
            GetWorld()->GetTimerManager().SetTimer(
                PlayerNameTimerHandle,
                [this, NewPlayer]()
            {
                DisplayPlayerName(NewPlayer);
            }, 0.5f, true);
        }
    }
}

void UOverheadWidget::NativeDestruct()
{
    RemoveFromParent();
    Super::NativeDestruct();
}
