// Copyright (c) Giammarco Agazzotti

#pragma once

#include "CoreMinimal.h"
#include "MoveInputState.generated.h"

enum class EInputEntry : uint8;

UENUM( BlueprintType )
enum class EInputType : uint8
{
    Pressed,
    Released,

    COUNT UMETA( Hidden ),
    INVALID UMETA( Hidden ),
};

USTRUCT( BlueprintType )
struct FIGHTINGGAME_API FMoveInputState
{
    GENERATED_BODY()

    UPROPERTY( EditAnywhere, BlueprintReadOnly, DisplayName = "Input" )
    EInputEntry m_InputEntry;

    UPROPERTY( EditAnywhere, BlueprintReadOnly, DisplayName = "Type" )
    EInputType m_InputType;

    bool operator==( const FMoveInputState& Other ) const
    {
        return m_InputEntry == Other.m_InputEntry && m_InputType == Other.m_InputType;
    }
};
