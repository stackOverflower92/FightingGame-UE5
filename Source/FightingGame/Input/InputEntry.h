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

    COUNT UMETA( Hidden ),
    INVALID UMETA( Hidden ),
};

FORCEINLINE FString InputEntryToString( EInputEntry InputEntry )
{
    switch( InputEntry )
    {
        case EInputEntry::None: return TEXT( "None" );
        case EInputEntry::StartJump: return TEXT( "StartJump" );
        case EInputEntry::StopJump: return TEXT( "StopJump" );
        case EInputEntry::Attack: return TEXT( "Attack" );
        case EInputEntry::Special: return TEXT( "Special" );
        case EInputEntry::Forward: return TEXT( "Forward" );
        case EInputEntry::ForwardDown: return TEXT( "ForwardDown" );
        case EInputEntry::Down: return TEXT( "Down" );
        case EInputEntry::DownBackward: return TEXT( "DownBackward" );
        case EInputEntry::Backward: return TEXT( "Backward" );
        case EInputEntry::BackwardUp: return TEXT( "BackwardUp" );
        case EInputEntry::Up: return TEXT( "Up" );
        case EInputEntry::UpForward: return TEXT( "UpForward" );
        case EInputEntry::COUNT: return TEXT( "COUNT" );
        case EInputEntry::INVALID: return TEXT( "INVALID" );
        default: break;
    }

    return TEXT( "" );
}

FORCEINLINE EInputEntry GetMirrored( EInputEntry InputEntry )
{
    switch( InputEntry )
    {
        case EInputEntry::None: return EInputEntry::None;
        case EInputEntry::StartJump: return EInputEntry::StartJump;
        case EInputEntry::StopJump: return EInputEntry::StopJump;
        case EInputEntry::Attack: return EInputEntry::Attack;
        case EInputEntry::Special: return EInputEntry::Special;
        case EInputEntry::Forward: return EInputEntry::Backward;
        case EInputEntry::ForwardDown: return EInputEntry::DownBackward;
        case EInputEntry::Down: return EInputEntry::Down;
        case EInputEntry::DownBackward: return EInputEntry::ForwardDown;
        case EInputEntry::Backward: return EInputEntry::Forward;
        case EInputEntry::BackwardUp: return EInputEntry::UpForward;
        case EInputEntry::Up: return EInputEntry::Up;
        case EInputEntry::UpForward: return EInputEntry::BackwardUp;
        case EInputEntry::COUNT: return EInputEntry::COUNT;
        case EInputEntry::INVALID: return EInputEntry::INVALID;
        default: break;
    }

    return EInputEntry::None;
}
