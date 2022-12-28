// Copyright (c) Giammarco Agazzotti

#pragma once

#include "CoreMinimal.h"
#include "MoveInputState.generated.h"

enum class EInputEntry : uint8;

USTRUCT( BlueprintType )
struct FIGHTINGGAME_API FMoveInputState
{
    GENERATED_BODY()

    UPROPERTY( EditAnywhere, BlueprintReadOnly, DisplayName = "Input" )
    EInputEntry m_InputEntry;

    UPROPERTY( EditAnywhere, BlueprintReadOnly, DisplayName = "Event" )
    TEnumAsByte<EInputEvent> m_InputEvent;

    bool operator==( const FMoveInputState& Other ) const
    {
        return m_InputEntry == Other.m_InputEntry && m_InputEvent == Other.m_InputEvent;
    }
};
