// Copyright (c) Giammarco Agazzotti

#include "MovesBufferComponent.h"
#include "Components/InputComponent.h"
#include "FightingGame/Character/FightingCharacter.h"
#include "FightingGame/Combat/InputSequenceResolver.h"
#include "FightingGame/Common/ConversionStatics.h"
#include "FightingGame/Common/MathStatics.h"
#include "FightingGame/Debugging/Debug.h"
#include "Kismet/KismetSystemLibrary.h"

namespace
{
    int32 loc_ShowInputBuffer = 0;
    FG_CVAR_FLAG_DESC( CVarShowInputBuffer, TEXT( "MovesBufferComponent.ShowInputBuffer" ), loc_ShowInputBuffer );

    int32 loc_ShowInputsSequenceBuffer = 0;
    FG_CVAR_FLAG_DESC( CVarShowMovesBuffer, TEXT( "MovesBufferComponent.ShowInputsSequenceBuffer" ), loc_ShowInputsSequenceBuffer );

    int32 loc_ShowDirectionalAngle = 0;
    FG_CVAR_FLAG_DESC( CVarShowDirectionalAngle, TEXT("MovesBufferComponent.ShowDirectionalAngle"), loc_ShowDirectionalAngle );

    FName loc_JumpAction           = TEXT( "Jump" );
    FName loc_AttackAction         = TEXT( "Attack" );
    FName loc_SpecialAction        = TEXT( "Special" );
    FName loc_MoveHorizontalAction = TEXT( "MoveHorizontal" );
    FName loc_MoveVerticalAction   = TEXT( "MoveVertical" );
}

UMovesBufferComponent::UMovesBufferComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UMovesBufferComponent::BeginPlay()
{
    Super::BeginPlay();

    m_InputSequenceResolver = m_InputSequenceResolverClass
                                  ? NewObject<UInputSequenceResolver>( GetOwner(), m_InputSequenceResolverClass )
                                  : NewObject<UInputSequenceResolver>( GetOwner() );

    m_InputSequenceResolver->m_InputRouteEndedDelegate.AddUObject( this, &UMovesBufferComponent::OnInputRouteEnded );

    TArray<TObjectPtr<UInputsSequence>> inputs;
    TArray<TTuple<bool, bool>> groundedAirborneFlags;
    for( int32 i = 0; i < m_InputsList.Num(); ++i )
    {
        inputs.Emplace( m_InputsList[i] );

        // #TODO find a way to link these inputs to the actual moves
        groundedAirborneFlags.Emplace( TTuple<bool, bool>( true, true ) );
    }

    m_InputSequenceResolver->Init( inputs, groundedAirborneFlags );
}

void UMovesBufferComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
    Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

    m_IBElapsedFrameTime += DeltaTime;

    static float bufferFrameDuration = 1.f / m_InputBufferFrameRate;
    if( m_IBElapsedFrameTime >= bufferFrameDuration )
    {
        m_IBElapsedFrameTime = 0.f;

        if( !m_IBBufferChanged )
        {
            AddToInputBuffer( EInputEntry::None );
        }
    }

    m_ISBElapsedFrameTime += DeltaTime;

    static float movesBufferFrameDuration = 1.f / m_InputsSequencesBufferFrameRate;
    if( m_ISBElapsedFrameTime >= movesBufferFrameDuration )
    {
        m_ISBElapsedFrameTime = 0.f;
        if( !m_ISBBufferChanged )
        {
            AddToInputsSequenceBuffer( FInputsSequenceBufferEntry::s_SequenceNone, 0 );
        }
    }

    if( loc_ShowInputBuffer )
    {
        if( m_OwnerCharacter && m_OwnerCharacter->m_PlayerIndex == 0 )
        {
            for( int32 i = 0; i < m_InputsBuffer.Num(); ++i )
            {
                FInputBufferEntry& entry = m_InputsBuffer[i];
                FColor color             = entry.m_Used ? FColor::Red : FColor::Green;

                FG_SLOG_KEY( i, FString::Printf( TEXT( "%s [%u]" ), *entry.ToString(), entry.m_UniqueId ), color );
            }
        }
    }

    if( loc_ShowInputsSequenceBuffer )
    {
        if( m_OwnerCharacter && m_OwnerCharacter->m_PlayerIndex == 0 )
        {
            for( int32 i = 0; i < m_InputsSequenceBuffer.Num(); ++i )
            {
                FInputsSequenceBufferEntry& entry = m_InputsSequenceBuffer[i];
                FColor color                      = entry.m_Used ? FColor::Red : FColor::Green;

                FG_SLOG_KEY( i + 20, FString::Printf( TEXT( "%s [%u]" ), *entry.ToString(), entry.m_UniqueId ), color );
            }
        }
    }

    m_IBBufferChanged  = false;
    m_ISBBufferChanged = false;

    UpdateMovementDirection();

    if( m_PlayerInput )
    {
        float horizontalMovement = m_PlayerInput->GetAxisValue( loc_MoveHorizontalAction );

        m_InputMovement = horizontalMovement;

        m_MovingRight = horizontalMovement > m_AnalogMovementDeadzone;
        m_MovingLeft  = horizontalMovement < -m_AnalogMovementDeadzone;

        UpdateDirectionalInputs( m_PlayerInput );
    }
}

void UMovesBufferComponent::OnSetupPlayerInputComponent( UInputComponent* PlayerInputComponent )
{
    m_PlayerInput = PlayerInputComponent;
    if( m_PlayerInput )
    {
        m_PlayerInput->BindAction( loc_JumpAction, IE_Pressed, this, &UMovesBufferComponent::OnStartJump );
        m_PlayerInput->BindAction( loc_JumpAction, IE_Released, this, &UMovesBufferComponent::OnStopJump );
        m_PlayerInput->BindAction( loc_AttackAction, IE_Pressed, this, &UMovesBufferComponent::OnAttack );
        m_PlayerInput->BindAction( loc_SpecialAction, IE_Pressed, this, &UMovesBufferComponent::OnSpecial );

        m_PlayerInput->BindAxis( loc_MoveHorizontalAction );
        m_PlayerInput->BindAxis( loc_MoveVerticalAction );
    }

    InitInputBuffer();

    InitInputsSequenceBuffer();
}

bool UMovesBufferComponent::IsInputBuffered( EInputEntry Input, bool ConsumeEntry )
{
    for( int32 i = 0; i < m_InputsBuffer.Num(); ++i )
    {
        FInputBufferEntry& entry = m_InputsBuffer[i];
        if( entry.m_InputEntry != EInputEntry::None && entry.m_InputEntry == Input && !entry.m_Used )
        {
            if( ConsumeEntry )
            {
                entry.m_Used = true;
            }
            return true;
        }
    }

    return false;
}

float UMovesBufferComponent::GetMovementDirection() const
{
    return m_MovementDirection;
}

EInputEntry UMovesBufferComponent::GetDirectionalInputEntryFromAngle( float Angle ) const
{
    // Assuming the character is facing right

    static float forwardAngle = 90.f;
    static float downAngle    = 180.f;
    static float backAngle    = -90.f;
    static float upAngle      = 0.f;

    // Up
    if( Angle > upAngle - m_DirectionalChangeRotationEpsilon && Angle < upAngle + m_DirectionalChangeRotationEpsilon )
    {
        return EInputEntry::Up;
    }

    // Up-forward
    if( Angle >= upAngle + m_DirectionalChangeRotationEpsilon && Angle < forwardAngle - m_DirectionalChangeRotationEpsilon )
    {
        return EInputEntry::UpForward;
    }

    // Forward
    if( Angle > forwardAngle - m_DirectionalChangeRotationEpsilon && Angle < forwardAngle + m_DirectionalChangeRotationEpsilon )
    {
        return EInputEntry::Forward;
    }

    // Forward-down
    if( Angle >= forwardAngle + m_DirectionalChangeRotationEpsilon && Angle < downAngle - m_DirectionalChangeRotationEpsilon )
    {
        return EInputEntry::ForwardDown;
    }

    // Down
    if( (Angle >= downAngle && Angle >= downAngle - m_DirectionalChangeRotationEpsilon) ||
        (Angle > -downAngle && Angle < -downAngle + m_DirectionalChangeRotationEpsilon) )
    {
        return EInputEntry::Down;
    }

    // Down-back
    if( Angle > -downAngle + m_DirectionalChangeRotationEpsilon && Angle < backAngle - m_DirectionalChangeRotationEpsilon )
    {
        return EInputEntry::DownBackward;
    }

    // Back
    if( Angle > backAngle - m_DirectionalChangeRotationEpsilon && Angle < backAngle + m_DirectionalChangeRotationEpsilon )
    {
        return EInputEntry::Backward;
    }

    // Back-Up
    if( Angle >= backAngle + m_DirectionalChangeRotationEpsilon && Angle < upAngle - m_DirectionalChangeRotationEpsilon )
    {
        return EInputEntry::BackwardUp;
    }

    return EInputEntry::None;
}

void UMovesBufferComponent::OnInputRouteEnded( TObjectPtr<UInputsSequence> InputsSequence )
{
    AddToInputsSequenceBuffer( InputsSequence->m_Name, InputsSequence->m_Priority );
}

void UMovesBufferComponent::AddToInputBuffer( EInputEntry InputEntry )
{
    EInputEntry targetEntry = m_OwnerCharacter->IsFacingRight() ? InputEntry : MirrorInputEntry( InputEntry );

    m_InputsBuffer.Emplace( FInputBufferEntry( targetEntry, false ) );
    m_InputsBuffer.RemoveAt( 0 );

    m_IBBufferChanged = true;

    if( InputEntry != EInputEntry::None )
    {
        auto result = m_InputSequenceResolver->RegisterInput( targetEntry );
        if( result == EInputRegistrationResult::InputNotFound )
        {
            m_InputSequenceResolver->RegisterInput( targetEntry );
        }
    }
}

bool UMovesBufferComponent::InputBufferContainsConsumable( EInputEntry InputEntry ) const
{
    for( const FInputBufferEntry& entry : m_InputsBuffer )
    {
        if( entry.m_InputEntry == InputEntry && !entry.m_Used )
        {
            return true;
        }
    }

    return false;
}

void UMovesBufferComponent::AddToInputsSequenceBuffer( const FString& InputsSequenceName, int32 Priority )
{
    m_InputsSequenceBuffer.Emplace( FInputsSequenceBufferEntry( InputsSequenceName, Priority, false ) );
    m_InputsSequenceBuffer.RemoveAt( 0 );

    m_ISBBufferChanged = true;
}

bool UMovesBufferComponent::InputsSequenceBufferContainsConsumable( const FString& MoveName )
{
    for( const FInputsSequenceBufferEntry& entry : m_InputsSequenceBuffer )
    {
        if( entry.m_InputsSequenceName == MoveName && !entry.m_Used )
        {
            return true;
        }
    }

    return false;
}

void UMovesBufferComponent::ClearInputsBuffer()
{
    m_InputsBuffer.Empty();
}

void UMovesBufferComponent::InitInputBuffer()
{
    ClearInputsBuffer();

    for( int32 i = 0; i < m_InputBufferSizeFrames; ++i )
    {
        m_InputsBuffer.Emplace( FInputBufferEntry( EInputEntry::None, false ) );
    }
}

void UMovesBufferComponent::UseBufferedInputsSequence( const FString& InputsSequenceName )
{
    verify( InputsSequenceBufferContainsConsumable( InputsSequenceName ) );

    for( FInputsSequenceBufferEntry& entry : m_InputsSequenceBuffer )
    {
        if( entry.m_InputsSequenceName == InputsSequenceName )
        {
            entry.m_Used = true;
        }
    }
}

void UMovesBufferComponent::UseBufferedInputsSequence( int32 UniqueId )
{
    for( FInputsSequenceBufferEntry& entry : m_InputsSequenceBuffer )
    {
        if( entry.m_UniqueId == UniqueId )
        {
            entry.m_Used = true;
        }
    }
}

void UMovesBufferComponent::ClearInputsSequenceBuffer()
{
    m_InputsSequenceBuffer.Empty();
}

void UMovesBufferComponent::InitInputsSequenceBuffer()
{
    ClearInputsSequenceBuffer();

    for( int i = 0; i < m_InputsSequencesBufferSizeFrames; ++i )
    {
        m_InputsSequenceBuffer.Emplace( FInputsSequenceBufferEntry( FInputsSequenceBufferEntry::s_SequenceNone, 0, false ) );
    }
}

void UMovesBufferComponent::GetInputsSequenceBufferSnapshot( TArray<FInputsSequenceBufferEntry>& OutEntries, bool SkipEmptyEntries, bool SkipUsedEntries )
{
    OutEntries.Reset();

    for( const auto& entry : m_InputsSequenceBuffer )
    {
        bool currentEntryIsNone = entry.m_InputsSequenceName == FInputsSequenceBufferEntry::s_SequenceNone;

        if( SkipEmptyEntries && currentEntryIsNone )
        {
            continue;
        }

        if( SkipUsedEntries && entry.m_Used )
        {
            continue;
        }

        OutEntries.Emplace( entry );
    }
}

bool UMovesBufferComponent::IsInputsSequenceBuffered( const FString& InputsSequenceName, bool ConsumeEntry /*= true*/ )
{
    for( int32 i = 0; i < m_InputsSequenceBuffer.Num(); ++i )
    {
        FInputsSequenceBufferEntry& entry = m_InputsSequenceBuffer[i];

        bool isValid     = entry.m_InputsSequenceName != FInputsSequenceBufferEntry::s_SequenceNone;
        bool hasSameName = entry.m_InputsSequenceName == InputsSequenceName;
        bool canBeUsed   = !entry.m_Used;

        if( isValid && hasSameName && canBeUsed )
        {
            if( ConsumeEntry )
            {
                entry.m_Used = true;
            }

            return true;
        }
    }

    return false;
}

void UMovesBufferComponent::OnMoveHorizontal( float Value )
{
    if( FMath::Abs( Value ) > m_AnalogMovementDeadzone )
    {
        m_InputMovement = Value;

        m_MovingRight = FMath::Sign( Value ) > 0;
        m_MovingLeft  = FMath::Sign( Value ) < 0;
    }
}

// TODO: these are all the same, make it generic maybe?
void UMovesBufferComponent::OnStartJump()
{
    AddToInputBuffer( EInputEntry::StartJump );
}

void UMovesBufferComponent::OnStopJump()
{
    AddToInputBuffer( EInputEntry::StopJump );
}

void UMovesBufferComponent::OnAttack()
{
    AddToInputBuffer( EInputEntry::Attack );
}

void UMovesBufferComponent::OnSpecial()
{
    AddToInputBuffer( EInputEntry::Special );
}

void UMovesBufferComponent::UpdateMovementDirection()
{
    if( (m_MovingRight && m_MovingLeft) || (!m_MovingRight && !m_MovingLeft) )
    {
        m_MovementDirection = 0.f;
    }
    else
    {
        if( m_MovingRight )
        {
            m_MovementDirection = 1.f;
        }
        else if( m_MovingLeft )
        {
            m_MovementDirection = -1.f;
        }
    }
}

void UMovesBufferComponent::UpdateDirectionalInputs( UInputComponent* InputComponent )
{
    m_DirectionalInputVector.X = InputComponent->GetAxisValue( "MoveHorizontal" );
    m_DirectionalInputVector.Y = InputComponent->GetAxisValue( "MoveVertical" );

    if( m_DirectionalInputVector.Length() > m_MinDirectionalInputVectorLength )
    {
        float angle       = UMathStatics::GetSignedAngle( m_DirectionalInputVector, FVector2d( 0.f, 1.f ) );
        EInputEntry entry = GetDirectionalInputEntryFromAngle( angle );

        if( loc_ShowDirectionalAngle )
        {
            FG_TEXT( GetWorld(), GetOwner()->GetActorLocation(),
                     FString::Printf( TEXT( "[Input: %s]" ), *UConversionStatics::ConvertEnumValueToString( entry, false ) ) );
        }

        if( entry != m_LastDirectionalInputEntry )
        {
            m_LastDirectionalInputEntry = entry;

            AddToInputBuffer( entry );
        }
    }
    else
    {
        if( m_LastDirectionalInputEntry != EInputEntry::Neutral )
        {
            m_LastDirectionalInputEntry = EInputEntry::Neutral;

            AddToInputBuffer( EInputEntry::Neutral );
        }
    }

    m_LastDirectionalInputVector = m_DirectionalInputVector;
}

void UMovesBufferComponent::UseBufferedInput( EInputEntry Input )
{
    verify( InputBufferContainsConsumable( Input ) );

    for( FInputBufferEntry& entry : m_InputsBuffer )
    {
        if( entry.m_InputEntry == Input )
        {
            entry.m_Used = true;
        }
    }
}

void UMovesBufferComponent::UseBufferedInput( int32 UniqueId )
{
}
