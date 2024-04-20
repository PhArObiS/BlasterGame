#include "BlasterGameMode.h"
#include "Blasters/Character/BlasterCharacter.h"
#include "Blasters/PlayerController/BlasterPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "Blasters/PlayerState/BlasterPlayerState.h"
#include "Blasters/GameState/BlasterGameState.h"

// Namespace for match states
namespace MatchState
{
    const FName Cooldown = FName("Cooldown");
}

// Constructor
ABlasterGameMode::ABlasterGameMode()
{
    bDelayedStart = true; // Delayed start enabled
}

// Called when the game mode begins play
void ABlasterGameMode::BeginPlay()
{
    Super::BeginPlay();
    LevelStartingTime = GetWorld()->GetTimeSeconds(); // Record the starting time of the level
}

// Called every frame
void ABlasterGameMode::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Update based on match state
    if (MatchState == MatchState::WaitingToStart)
    {
        CountdownTime = WarmupTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;
        if (CountdownTime <= 0.f)
        {
            StartMatch(); // Start the match if countdown time is up
        }
    }
    else if (MatchState == MatchState::InProgress)
    {
        CountdownTime = WarmupTime + MatchTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;
        if (CountdownTime <= 0.f)
        {
            SetMatchState(MatchState::Cooldown); // Set match state to cooldown if countdown time is up
        }
    }
    else if (MatchState == MatchState::Cooldown)
    {
        CountdownTime = WarmupTime + MatchTime + CooldownTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;
        if (CountdownTime <= 0.f)
        {
            RestartGame(); // Restart the game if cooldown time is up
        }
    }
}

// Called when the match state is set
void ABlasterGameMode::OnMatchStateSet()
{
    Super::OnMatchStateSet();

    // Update match state for all player controllers
    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; It++)
    {
        ABlasterPlayerController *BlasterPlayer = Cast<ABlasterPlayerController>(*It);
        if (BlasterPlayer)
        {
            BlasterPlayer->OnMatchStateSet(MatchState);
        }
    }
}

float ABlasterGameMode::CalculateDamage(AController *Attacker, AController *Victim, float BaseDamage)
{
    return BaseDamage;
}

// Handle player elimination
void ABlasterGameMode::PlayerEliminated(ABlasterCharacter *ElimmedCharacter, ABlasterPlayerController *VictimController, ABlasterPlayerController *AttackerController)
{
    // Ensure valid controllers and player states
    if (AttackerController == nullptr || AttackerController->PlayerState == nullptr)
        return;
    if (VictimController == nullptr || VictimController->PlayerState == nullptr)
        return;

    // Get player states of attacker and victim
    ABlasterPlayerState *AttackerPlayerState = AttackerController ? Cast<ABlasterPlayerState>(AttackerController->PlayerState) : nullptr;
    ABlasterPlayerState *VictimPlayerState = VictimController ? Cast<ABlasterPlayerState>(VictimController->PlayerState) : nullptr;

    // Get game state
    ABlasterGameState *BlasterGameState = GetGameState<ABlasterGameState>();

    // Update scores and defeats
    if (AttackerPlayerState && AttackerPlayerState != VictimPlayerState && BlasterGameState)
    {
        TArray<ABlasterPlayerState *> PlayersCurrentlyInTheLead;
        for (auto LeadPlayer : BlasterGameState->TopScoringPlayers)
        {
            PlayersCurrentlyInTheLead.Add(LeadPlayer);
        }

        AttackerPlayerState->AddToScore(1.f);                  // Increase attacker's score
        BlasterGameState->UpdateTopScore(AttackerPlayerState); // Update top score in game state
        if (BlasterGameState->TopScoringPlayers.Contains(AttackerPlayerState))
        {
            ABlasterCharacter *Leader = Cast<ABlasterCharacter>(AttackerPlayerState->GetPawn());
            if (Leader)
            {
                Leader->MulticastGainedTheLead();
            }
        }

        for (int32 i = 0; i < PlayersCurrentlyInTheLead.Num(); i++)
        {
            if (!BlasterGameState->TopScoringPlayers.Contains(PlayersCurrentlyInTheLead[i]))
            {
                ABlasterCharacter *Loser = Cast<ABlasterCharacter>(PlayersCurrentlyInTheLead[i]->GetPawn());
                if (Loser)
                {
                    Loser->MulticastLostTheLead();
                }
            }
        }
    }
    if (VictimPlayerState)
    {
        VictimPlayerState->AddToDefeats(1); // Increase victim's defeats
    }

    // Eliminate the character
    if (ElimmedCharacter)
    {
        ElimmedCharacter->Elim(false); // Eliminate the character
    }

    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        ABlasterPlayerController *BlasterPlayer = Cast<ABlasterPlayerController>(*It);
        if (BlasterPlayer && AttackerPlayerState && VictimPlayerState)
        {
            BlasterPlayer->BroadcastElim(AttackerPlayerState, VictimPlayerState);
        }
    }
}

// Request respawn for an eliminated character
void ABlasterGameMode::RequestRespawn(ACharacter *ElimmedCharacter, AController *ElimmedController)
{
    // Reset and destroy the eliminated character
    if (ElimmedCharacter)
    {
        ElimmedCharacter->Reset();
        ElimmedCharacter->Destroy();
    }

    // Respawn the character at a random player start
    if (ElimmedController)
    {
        TArray<AActor *> PlayerStarts;
        UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), PlayerStarts);
        int32 PlayerStartIndex = FMath::RandRange(0, PlayerStarts.Num() - 1);
        RestartPlayerAtPlayerStart(ElimmedController, PlayerStarts[PlayerStartIndex]);
    }
}

void ABlasterGameMode::PlayerLeftGame(ABlasterPlayerState *PlayerLeaving)
{
    if (PlayerLeaving == nullptr)
        return;
    ABlasterGameState *BlasterGameState = GetGameState<ABlasterGameState>();
    if (BlasterGameState && BlasterGameState->TopScoringPlayers.Contains(PlayerLeaving))
    {
        BlasterGameState->TopScoringPlayers.Remove(PlayerLeaving);
    }
    ABlasterCharacter *CharacterLeaving = Cast<ABlasterCharacter>(PlayerLeaving->GetPawn());
    if (CharacterLeaving)
    {
        CharacterLeaving->Elim(true);
    }
}
