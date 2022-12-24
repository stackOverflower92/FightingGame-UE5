#pragma once

UENUM( BlueprintType )
enum class EMoveType : uint8
{
    None,
    StartJump,
    StopJump,
    Attack,
    Special,

    COUNT,
    INVALID
};
