// Copyright (c) Giammarco Agazzotti

#include "MovesBufferComponent.h"
#include "Components/InputComponent.h"
#include "FightingGame/Character/FightingCharacter.h"
#include "FightingGame/Combat/InputSequenceResolver.h"
#include "FightingGame/Common/MathStatics.h"
#include "FightingGame/Debugging/Debug.h"
#include "Kismet/KismetSystemLibrary.h"

namespace
{
    int32 loc_ShowInputBuffer = 0;
    FG_CVAR_FLAG_DESC( CVarShowInputBuffer, TEXT( "MovesBufferComponent.ShowInputBuffer" ), loc_ShowInputBuffer );

    int32 loc_ShowDirectionalAngle = 0;
    FG_CVAR_FLAG_DESC( CVarShowDirectionalAngle, TEXT("MovesBufferComponent.ShowDirectionalAngle"), loc_ShowDirectionalAngle );
}

UMovesBufferComponent::UMovesBufferComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UMovesBufferComponent::BeginPlay()
{
    Super::BeginPlay();

    if( !m_InputSequenceResolverClass )
    {
        m_InputSequenceResolver = NewObject<UInputSequenceResolver>( GetOwner() );
    }
    else
    {
        m_InputSequenceResolver = NewObject<UInputSequenceResolver>( GetOwner(), m_InputSequenceResolverClass );
    }

    m_InputSequenceResolver->m_InputRouteEndedDelegate.AddUObject( this, &UMovesBufferComponent::OnInputRouteEnded );
    m_InputSequenceResolver->Init( m_MovesList );
}

void UMovesBufferComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
    Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

    m_ElapsedFrameTime += DeltaTime;

    static float bufferFrameDuration = 1.f / m_BufferFrameRate;
    if( m_ElapsedFrameTime >= bufferFrameDuration )
    {
        m_ElapsedFrameTime = 0.f;

        if( !m_BufferChanged )
        {
            AddMoveToBuffer( EInputEntry::None );
        }
    }

    if( loc_ShowInputBuffer )
    {
        if( m_OwnerCharacter && m_OwnerCharacter->m_PlayerIndex == 0 )
        {
            for( int i = 0; i < m_Buffer.size(); ++i )
            {
                FInputBufferEntry& bufferEntry = m_Buffer.at( i );
                const bool isEmpty             = bufferEntry.m_MoveType == EInputEntry::None;
                FString message                = isEmpty ? TEXT( "Empty" ) : InputEntryToString( bufferEntry.m_MoveType );

                FColor Color = bufferEntry.m_Used ? FColor::Red : FColor::Green;

                GEngine->AddOnScreenDebugMessage( i, 1.f, Color, FString::Printf( TEXT( "%s" ), *message ) );
            }
        }
    }

    m_BufferChanged = false;

    UpdateMovementDirection();

    if( m_PlayerInput )
    {
        float horizontalMovement = m_PlayerInput->GetAxisValue( TEXT( "MoveHorizontal" ) );

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
        m_PlayerInput->BindAction( TEXT( "Jump" ), IE_Pressed, this, &UMovesBufferComponent::OnStartJump );
        m_PlayerInput->BindAction( TEXT( "Jump" ), IE_Released, this, &UMovesBufferComponent::OnStopJump );
        m_PlayerInput->BindAction( TEXT( "Attack" ), IE_Pressed, this, &UMovesBufferComponent::OnAttack );
        m_PlayerInput->BindAction( TEXT( "Special" ), IE_Pressed, this, &UMovesBufferComponent::OnSpecial );

        m_PlayerInput->BindAxis( TEXT( "MoveHorizontal" ) );
        m_PlayerInput->BindAxis( TEXT( "MoveVertical" ) );
    }

    InitBuffer();
}

bool UMovesBufferComponent::IsInputBuffered( EInputEntry Input, bool ConsumeEntry )
{
    for( int i = 0; i < m_Buffer.size(); ++i )
    {
        FInputBufferEntry& Entry = m_Buffer.at( i );
        if( Entry.m_MoveType != EInputEntry::None && Entry.m_MoveType == Input && !Entry.m_Used )
        {
            if( ConsumeEntry )
            {
                Entry.m_Used = true;
            }
            return true;
        }
    }

    return false;
}

TArray<EInputEntry> UMovesBufferComponent::GetBufferedInputs() const
{
    TArray<EInputEntry> BufferedInputs;
    for( const FInputBufferEntry& Input : m_Buffer )
    {
        if( Input.m_MoveType != EInputEntry::None && !Input.m_Used )
        {
            BufferedInputs.Emplace( Input.m_MoveType );
        }
    }

    return BufferedInputs;
}

float UMovesBufferComponent::GetMovementDirection() const
{
    return m_MovementDirection;
}

EInputEntry UMovesBufferComponent::GetDirectionalInputEntryFromAngle( float Angle ) const
{
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

void UMovesBufferComponent::OnInputRouteEnded( uint32 MoveUniqueId )
{
    auto* it = m_MovesList.FindByPredicate( [&MoveUniqueId]( TObjectPtr<UMoveDataAsset> _move )
    {
        return _move->GetUniqueID() == MoveUniqueId;
    } );

    if( it )
    {
        FG_SLOG_INFO( FString::Printf(TEXT("Route: %s"), *((*it)->m_Id)) );
    }
}

void UMovesBufferComponent::AddMoveToBuffer( EInputEntry InputEntry )
{
    EInputEntry targetEntry = m_OwnerCharacter->IsFacingRight() ? InputEntry : GetMirrored( InputEntry );

    m_Buffer.emplace_back( FInputBufferEntry{targetEntry, false} );
    m_Buffer.pop_front();

    m_BufferChanged = true;

    if( InputEntry != EInputEntry::None )
    {
        m_InputSequenceResolver->RegisterInput( targetEntry );
    }
}

bool UMovesBufferComponent::BufferContainsConsumableInput( EInputEntry MoveType ) const
{
    for( const FInputBufferEntry& CurrentInput : m_Buffer )
    {
        if( CurrentInput.m_MoveType == MoveType && !CurrentInput.m_Used )
        {
            return true;
        }
    }

    return false;
}

void UMovesBufferComponent::ClearBuffer()
{
    m_Buffer.clear();
}

void UMovesBufferComponent::InitBuffer()
{
    ClearBuffer();

    for( int i = 0; i < m_BufferSizeFrames; ++i )
    {
        m_Buffer.emplace_back( FInputBufferEntry{EInputEntry::None, false} );
    }
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
    AddMoveToBuffer( EInputEntry::StartJump );
}

void UMovesBufferComponent::OnStopJump()
{
    AddMoveToBuffer( EInputEntry::StopJump );
}

void UMovesBufferComponent::OnAttack()
{
    AddMoveToBuffer( EInputEntry::Attack );
}

void UMovesBufferComponent::OnSpecial()
{
    AddMoveToBuffer( EInputEntry::Special );
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
            UKismetSystemLibrary::DrawDebugString( GetWorld(), GetOwner()->GetActorLocation(),
                                                   FString::Printf( TEXT( "[Input: %s]" ), *InputEntryToString( entry ) ) );
        }

        if( entry != m_LastDirectionalInputEntry )
        {
            m_LastDirectionalInputEntry = entry;

            AddMoveToBuffer( entry );
        }
    }

    m_LastDirectionalInputVector = m_DirectionalInputVector;
}

void UMovesBufferComponent::UseBufferedInput( EInputEntry Input )
{
    verify( BufferContainsConsumableInput( Input ) );

    for( FInputBufferEntry& currentInput : m_Buffer )
    {
        if( currentInput.m_MoveType == Input )
        {
            currentInput.m_Used = true;
        }
    }
}
