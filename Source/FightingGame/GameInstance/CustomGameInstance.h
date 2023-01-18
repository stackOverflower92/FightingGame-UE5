// Copyright (c) Giammarco Agazzotti

#pragma once

#include "CoreMinimal.h"
#include "CustomGameState.h"
#include "CustomGameInstance.generated.h"

UCLASS()
class FIGHTINGGAME_API UCustomGameInstance : public UGameInstance
{
    GENERATED_BODY()

public:
    UPROPERTY( EditAnywhere, BlueprintReadOnly, DisplayName = "Game States" )
    TArray<TSubclassOf<UCustomGameState>> m_GameStates;

    UPROPERTY( EditAnywhere, BlueprintReadOnly, DisplayName = "First State Name" )
    FName m_FirstStateName;
};
