#pragma once

UENUM( BlueprintType )
enum class EInputEntry : uint8
{
    None,
    StartJump,
    StopJump,
    Attack,
    Special,

    COUNT,
    INVALID
};
