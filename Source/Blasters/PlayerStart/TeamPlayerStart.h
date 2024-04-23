// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerStart.h"
#include "Blasters/BlasterTypes/Team.h"
#include "TeamPlayerStart.generated.h"

/**
 *
 */
UCLASS()
class BLASTERS_API ATeamPlayerStart : public APlayerStart
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	ETeam Team;
};