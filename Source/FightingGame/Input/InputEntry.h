#pragma once

UENUM( BlueprintType )
enum class EInputEntry : uint8
{
    None,
    StartJump,
    StopJump,
    Attack,
    Special,
    Forward,
    ForwardDown,
    Down,
    DownBackward,
    Backward,
    BackwardUp,
    Up,
    UpForward,
    Neutral,
    Counter,

    COUNT UMETA( Hidden ),
    INVALID UMETA( Hidden ),
};

FORCEINLINE EInputEntry MirrorInputEntry( EInputEntry InputEntry )
{
    switch( InputEntry )
    {
        case EInputEntry::Forward: return EInputEntry::Backward;
        case EInputEntry::Backward: return EInputEntry::Forward;

        case EInputEntry::ForwardDown: return EInputEntry::DownBackward;
        case EInputEntry::DownBackward: return EInputEntry::ForwardDown;

        case EInputEntry::UpForward: return EInputEntry::BackwardUp;
        case EInputEntry::BackwardUp: return EInputEntry::UpForward;

        default: return InputEntry;
    }
}
