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

    int32 loc_ShowMovesBuffer = 0;
    FG_CVAR_FLAG_DESC( CVarShowMovesBuffer, TEXT( "MovesBufferComponent.ShowMovesBuffer" ), loc_ShowMovesBuffer );

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

    TArray<TObjectPtr<UInputsSequence>> inputs;
    TArray<TTuple<bool, bool>> groundedAirborneFlags;
    for( int i = 0; i < m_MovesList.Num(); ++i )
    {
        inputs.Emplace( m_MovesList[i]->m_InputsSequence );
        groundedAirborneFlags.Emplace( TTuple<bool, bool>( m_MovesList[i]->m_AllowWhenGrounded, m_MovesList[i]->m_AllowWhenAirborne ) );
    }

    m_InputSequenceResolver->Init( inputs, groundedAirborneFlags );
}

void UMovesBufferComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
    Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

    m_IBElapsedFrameTime += DeltaTime;

    static float bufferFrameDuration = 1.f / m_BufferFrameRate;
    if( m_IBElapsedFrameTime >= bufferFrameDuration )
    {
        m_IBElapsedFrameTime = 0.f;

        if( !m_IBBufferChanged )
        {
            AddToInputBuffer( EInputEntry::None );
        }
    }

    m_MBElapsedFrameTime += DeltaTime;

    static float movesBufferFrameDuration = 1.f / m_BufferFrameRate;
    if( m_MBElapsedFrameTime >= movesBufferFrameDuration )
    {
        m_MBElapsedFrameTime = 0.f;
        if( !m_MBBufferChanged )
        {
            AddToMovesBuffer( FMoveBufferEntry::s_MoveNone );
        }
    }

    if( loc_ShowInputBuffer )
    {
        if( m_OwnerCharacter && m_OwnerCharacter->m_PlayerIndex == 0 )
        {
            for( int32 i = 0; i < m_InputsBuffer.size(); ++i )
            {
                FInputBufferEntry& entry = m_InputsBuffer.at( i );
                const bool isEmpty       = entry.m_InputEntry == EInputEntry::None;
                FString message          = isEmpty ? TEXT( "---" ) : InputEntryToString( entry.m_InputEntry );

                FColor color = entry.m_Used ? FColor::Red : FColor::Green;

                GEngine->AddOnScreenDebugMessage( i, 1.f, color, FString::Printf( TEXT( "%s" ), *message ) );
            }
        }
    }

    if( loc_ShowMovesBuffer )
    {
        if( m_OwnerCharacter && m_OwnerCharacter->m_PlayerIndex == 0 )
        {
            for( int32 i = 0; i < m_MovesBuffer.size(); ++i )
            {
                FMoveBufferEntry& entry = m_MovesBuffer.at( i );
                bool isEmpty            = entry.m_MoveName == FMoveBufferEntry::s_MoveNone;
                FString message         = isEmpty ? TEXT( "---" ) : entry.m_MoveName.ToString();

                FColor color = entry.m_Used ? FColor::Red : FColor::Green;

                GEngine->AddOnScreenDebugMessage( i + 20, 1.f, color, FString::Printf( TEXT( "%s" ), *message ) );
            }
        }
    }

    m_IBBufferChanged = false;
    m_MBBufferChanged = false;

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

    InitInputBuffer();

    InitMovesBuffer();
}

bool UMovesBufferComponent::IsInputBuffered( EInputEntry Input, bool ConsumeEntry )
{
    for( int i = 0; i < m_InputsBuffer.size(); ++i )
    {
        FInputBufferEntry& entry = m_InputsBuffer.at( i );
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

void UMovesBufferComponent::OnInputRouteEnded( uint32 InputsSequenceUniqueId )
{
    auto* it = m_MovesList.FindByPredicate( [&InputsSequenceUniqueId]( TObjectPtr<UMoveDataAsset> _move )
    {
        return _move->m_InputsSequence->GetUniqueID() == InputsSequenceUniqueId;
    } );

    if( it )
    {
        AddToMovesBuffer( (*it)->m_Id );
    }
}

void UMovesBufferComponent::AddToInputBuffer( EInputEntry InputEntry )
{
    EInputEntry targetEntry = m_OwnerCharacter->IsFacingRight() ? InputEntry : GetMirrored( InputEntry );

    m_InputsBuffer.emplace_back( FInputBufferEntry{targetEntry, false} );
    m_InputsBuffer.pop_front();

    m_IBBufferChanged = true;

    if( InputEntry != EInputEntry::None )
    {
        m_InputSequenceResolver->RegisterInput( targetEntry );
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

void UMovesBufferComponent::AddToMovesBuffer( const FName& MoveName )
{
    m_MovesBuffer.emplace_back( FMoveBufferEntry{MoveName, false} );
    m_MovesBuffer.pop_front();

    m_MBBufferChanged = true;
}

bool UMovesBufferComponent::MovesBufferContainsConsumable( const FName& MoveName )
{
    for( const FMoveBufferEntry& entry : m_MovesBuffer )
    {
        if( entry.m_MoveName == MoveName && !entry.m_Used )
        {
            return true;
        }
    }

    return false;
}

void UMovesBufferComponent::ClearInputsBuffer()
{
    m_InputsBuffer.clear();
}

void UMovesBufferComponent::InitInputBuffer()
{
    ClearInputsBuffer();

    for( int i = 0; i < m_InputBufferSizeFrames; ++i )
    {
        m_InputsBuffer.emplace_back( FInputBufferEntry{EInputEntry::None, false} );
    }
}

void UMovesBufferComponent::UseBufferedMove( const FName& MoveName )
{
    verify( MovesBufferContainsConsumable( MoveName ) );

    for( FMoveBufferEntry& entry : m_MovesBuffer )
    {
        if( entry.m_MoveName == MoveName )
        {
            entry.m_Used = true;
        }
    }
}

void UMovesBufferComponent::ClearMovesBuffer()
{
    m_MovesBuffer.clear();
}

void UMovesBufferComponent::InitMovesBuffer()
{
    ClearMovesBuffer();

    for( int i = 0; i < m_InputBufferSizeFrames; ++i )
    {
        m_MovesBuffer.emplace_back( FMoveBufferEntry{FMoveBufferEntry::s_MoveNone, false} );
    }
}

bool UMovesBufferComponent::IsMoveBuffered( const FName& MoveName, bool ConsumeEntry /*= true*/ )
{
    for( int i = 0; i < m_MovesBuffer.size(); ++i )
    {
        FMoveBufferEntry& entry = m_MovesBuffer.at( i );
        if( entry.m_MoveName != FMoveBufferEntry::s_MoveNone && entry.m_MoveName == MoveName && !entry.m_Used )
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
            UKismetSystemLibrary::DrawDebugString( GetWorld(), GetOwner()->GetActorLocation(),
                                                   FString::Printf( TEXT( "[Input: %s]" ), *InputEntryToString( entry ) ) );
        }

        if( entry != m_LastDirectionalInputEntry )
        {
            m_LastDirectionalInputEntry = entry;

            AddToInputBuffer( entry );
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
