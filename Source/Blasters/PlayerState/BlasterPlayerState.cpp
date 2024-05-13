// Fill out your copyright notice in the Description page of Project Settings.

#include "BlasterPlayerState.h"
#include "Blasters/Character/BlasterCharacter.h"
#include "Blasters/PlayerController/BlasterPlayerController.h"
#include "Net/UnrealNetwork.h"


void ABlasterPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ABlasterPlayerState, Defeats);
    DOREPLIFETIME(ABlasterPlayerState, ElimMessage);
    DOREPLIFETIME(ABlasterPlayerState, Team);
}

void ABlasterPlayerState::AddToScore(float ScoreAmount)
{
    SetScore(GetScore() + ScoreAmount);
    Character = Character == nullptr ? Cast<ABlasterCharacter>(GetPawn()) : Character;
    if (Character)
    {
        Controller = Controller == nullptr ? Cast<ABlasterPlayerController>(Character->Controller) : Controller;
        if (Controller)
        {
            Controller->SetHUDScore(GetScore());
        }
    }
}

void ABlasterPlayerState::OnRep_Score()
{
    Super::OnRep_Score();

    Character = Character == nullptr ? Cast<ABlasterCharacter>(GetPawn()) : Character;
    if (Character)
    {
        Controller = Controller == nullptr ? Cast<ABlasterPlayerController>(Character->Controller) : Controller;
        if (Controller)
        {
            Controller->SetHUDScore(GetScore());
        }
    }
}

void ABlasterPlayerState::AddToDefeats(int32 DefeatsAmount)
{
    
    Defeats += DefeatsAmount;
    Character = Character == nullptr ? Cast<ABlasterCharacter>(GetPawn()) : Character;
    if (Character)
    {
        Controller = Controller == nullptr ? Cast<ABlasterPlayerController>(Character->Controller) : Controller;
        if (Controller)
        {
            Controller->SetHUDDefeats(Defeats);
        }
    }
}

void ABlasterPlayerState::AddElimText(FString ElimMessageText)
{
    // Set elimination message and update HUD
    Character = Character == nullptr ? Cast<ABlasterCharacter>(GetPawn()) : Character;
    if (Character)
    {
        Controller = Controller == nullptr ? Cast<ABlasterPlayerController>(Character->Controller) : Controller;
        if (Controller)
        {
            // Use this pointer to access the member variable
            this->ElimMessage = ElimMessageText;
            Controller->SetHUDElimText(ElimMessage); // Update HUD locally
        }
    }
}

void ABlasterPlayerState::SetTeam(ETeam TeamToSet)
{
    Team = TeamToSet;

    ABlasterCharacter *BCharacter = Cast<ABlasterCharacter>(GetPawn());
    if (BCharacter)
    {
        BCharacter->SetTeamColor(Team);
    }
}

void ABlasterPlayerState::OnRep_Team()
{
    ABlasterCharacter *BCharacter = Cast<ABlasterCharacter>(GetPawn());
    if (BCharacter)
    {
        BCharacter->SetTeamColor(Team);
    }
}

void ABlasterPlayerState::OnRep_Defeats()
{
    Character = Character == nullptr ? Cast<ABlasterCharacter>(GetPawn()) : Character;
    if (Character)
    {
        Controller = Controller == nullptr ? Cast<ABlasterPlayerController>(Character->Controller) : Controller;
        if (Controller)
        {
            Controller->SetHUDDefeats(Defeats);
        }
    }
}

void ABlasterPlayerState::OnRep_ElimMessage()
{
    // Update HUD elimination message when replicated
    Character = Character == nullptr ? Cast<ABlasterCharacter>(GetPawn()) : Character;
    if (Character)
    {
        Controller = Controller == nullptr ? Cast<ABlasterPlayerController>(Character->Controller) : Controller;
        if (Controller)
        {
            Controller->SetHUDElimText(ElimMessage);
        }
    }
}
