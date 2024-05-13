// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Blasters/BlasterTypes/Team.h"
#include "BlasterPlayerState.generated.h"

/**
 *
 */
UCLASS()
class BLASTERS_API ABlasterPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	// Override GetLifetimeReplicatedProps to specify replicated properties
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const override;

	// Replication notifies
	virtual void OnRep_Score() override; // Called when score is replicated
	UFUNCTION() // Function to be called when defeats are replicated
	virtual void OnRep_Defeats();
	
	UFUNCTION() // Function to be called when elimination message is replicated
	virtual void OnRep_ElimMessage(); // *******************
	

	// Functions to modify player state
	void AddToScore(float ScoreAmount); // Add score
	void AddToDefeats(int32 DefeatsAmount); // Add defeats
	void AddElimText(FString ElimMessage); // Set elimination message
	
private:
	UPROPERTY()
	class ABlasterCharacter* Character;
	
	UPROPERTY()
	class ABlasterPlayerController* Controller;

	UPROPERTY(ReplicatedUsing = OnRep_Defeats)// Replicated variable for keeping track of defeats
	int32 Defeats;
	
	UPROPERTY(ReplicatedUsing = OnRep_ElimMessage) // Replicated variable for elimination message
	FString ElimMessage;

	UPROPERTY(ReplicatedUsing = OnRep_Team) // Replicated variable for team
	ETeam Team = ETeam::ET_NoTeam; // Default value is ET_NoTeam

	// Function to be called when team is replicated
	UFUNCTION()
	void OnRep_Team();

public:
	FORCEINLINE ETeam GetTeam() const { return Team; }
	void SetTeam(ETeam TeamToSet);
};
