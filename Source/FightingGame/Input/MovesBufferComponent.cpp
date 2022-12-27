// Copyright (c) Giammarco Agazzotti

#include "MovesBufferComponent.h"
#include "Components/InputComponent.h"
#include "FightingGame/Character/FightingCharacter.h"
#include "FightingGame/Debugging/Debug.h"

namespace
{
    int32 loc_ShowInputBuffer = 0;
    FG_CVAR_FLAG_DESC( CVarShowInputBuffer, TEXT( "MovesBufferComponent.ShowInputBuffer" ), loc_ShowInputBuffer );
}

UMovesBufferComponent::UMovesBufferComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UMovesBufferComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
    Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

    m_ElapsedFrameTime += DeltaTime;

    if( m_ElapsedFrameTime >= m_BufferFrameLength )
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
            /*for( int i = 0; i < m_Buffer.size(); ++i )
            {
                FInputBufferEntry& Entry = m_Buffer.at( i );
                const bool IsEmpty       = Entry.m_MoveType == EInputEntry::None;
                auto Message             = IsEmpty ? TEXT( "Empty" ) : Entry.m_MoveType;

                FColor Color = Entry.m_Used ? FColor::Red : FColor::Green;
                GEngine->AddOnScreenDebugMessage( i, 1.f, Color, FString::Printf( TEXT( "%s" ), *Message ) );
            }*/
        }
    }

    m_BufferChanged = false;

    UpdateMovementDirection();

    if( m_PlayerInput )
    {
        float horizontalMovement = m_PlayerInput->GetAxisValue( TEXT( "MoveHorizontal" ) );

        m_InputMovement = horizontalMovement;

        m_MovingRight = horizontalMovement > 0.f;
        m_MovingLeft  = horizontalMovement < 0.f;
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

void UMovesBufferComponent::AddMoveToBuffer( EInputEntry MoveType )
{
    m_Buffer.emplace_back( FInputBufferEntry{MoveType, false} );
    m_Buffer.pop_front();

    m_BufferChanged = true;
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

void UMovesBufferComponent::UseBufferedInput( EInputEntry Input )
{
    verify( BufferContainsConsumableInput( Input ) );

    for( FInputBufferEntry& CurrentInput : m_Buffer )
    {
        if( CurrentInput.m_MoveType == Input )
        {
            CurrentInput.m_Used = true;
        }
    }
}
