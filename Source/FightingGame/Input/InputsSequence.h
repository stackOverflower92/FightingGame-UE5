// Copyright (c) Giammarco Agazzotti

#pragma once

#include "CoreMinimal.h"
#include "FightingGame/Combat/MoveInputState.h"
#include "InputsSequence.generated.h"

UCLASS()
class FIGHTINGGAME_API UInputsSequence : public UDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY( EditAnywhere, BlueprintReadOnly, DisplayName = "Name" )
    FName m_Name;

    UPROPERTY( EditAnywhere, BlueprintReadOnly, DisplayName = "Sequence" )
    TArray<FMoveInputState> m_Inputs;

    UPROPERTY( EditAnywhere, BlueprintReadOnly, DisplayName = "Sequence" )
    int32 m_Priority = 0;
};
