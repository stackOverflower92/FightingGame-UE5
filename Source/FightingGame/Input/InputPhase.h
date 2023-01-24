#pragma once

#include "CoreMinimal.h"

UENUM( BlueprintType )
enum class EInputPhase : uint8
{
    Pressed,
    Released,
    Any,

    COUNT UMETA( Hidden ),
    INVALID UMETA( Hidden ),
};
