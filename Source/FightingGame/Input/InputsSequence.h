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
    FString m_Name;

    UPROPERTY( EditAnywhere, BlueprintReadOnly, DisplayName = "Sequence" )
    TArray<FMoveInputState> m_Inputs;

    UPROPERTY( EditAnywhere, BlueprintReadOnly, DisplayName = "Priority" )
    int32 m_Priority = 0;

    UPROPERTY( EditAnywhere, BlueprintReadOnly, DisplayName = "Is Special" )
    bool m_IsSpecial = false;
};
