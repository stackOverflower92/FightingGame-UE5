// Copyright (c) Giammarco Agazzotti

#pragma once

#include "CoreMinimal.h"
#include "FightingGame/Input/InputPhase.h"
#include "MoveInputState.generated.h"

enum class EInputEntry : uint8;

USTRUCT( BlueprintType )
struct FIGHTINGGAME_API FMoveInputState
{
    GENERATED_BODY()

    UPROPERTY( EditAnywhere, BlueprintReadOnly, DisplayName = "Input" )
    EInputEntry m_InputEntry;

    UPROPERTY( EditAnywhere, BlueprintReadOnly, DisplayName = "Phase" )
    EInputPhase m_InputPhase;

    bool operator==( const FMoveInputState& Other ) const
    {
        return m_InputEntry == Other.m_InputEntry && m_InputPhase == Other.m_InputPhase;
    }
};
