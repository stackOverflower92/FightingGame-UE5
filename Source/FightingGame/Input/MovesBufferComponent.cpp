// Copyright (c) Giammarco Agazzotti

#include "MovesBufferComponent.h"
#include "Components/InputComponent.h"
#include "FightingGame/Character/FightingCharacter.h"
#include "FightingGame/Debugging/Debug.h"

namespace
{
	const FString NoInput{""};

	const FString EntryStartJump{"StartJump"};
	const FString EntryStopJump{"StopJump"};
	const FString EntryAttack{"Attack"};
	const FString EntrySpecial{"Special"};

	// CVars
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
			AddMoveToBuffer( NoInput );
		}
	}

	if( loc_ShowInputBuffer )
	{
		if( m_OwnerCharacter && m_OwnerCharacter->m_PlayerIndex == 0 )
		{
			for( int i = 0; i < m_Buffer.size(); ++i )
			{
				FInputBufferEntry& Entry = m_Buffer.at( i );
				const bool IsEmpty       = Entry.Name == NoInput;
				auto Message             = IsEmpty ? TEXT( "Empty" ) : Entry.Name;

				FColor Color = Entry.Used ? FColor::Red : FColor::Green;
				GEngine->AddOnScreenDebugMessage( i, 1.f, Color, FString::Printf( TEXT( "%s" ), *Message ) );
			}
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

		//m_PlayerInput->BindAxis( "MoveHorizontal", this, &UMovesBufferComponent::OnMoveHorizontal );
		m_PlayerInput->BindAxis( TEXT( "MoveHorizontal" ) );
	}

	InitBuffer();
}

bool UMovesBufferComponent::IsInputBuffered( FString Input, bool ConsumeEntry )
{
	for( int i = 0; i < m_Buffer.size(); ++i )
	{
		FInputBufferEntry& Entry = m_Buffer.at( i );
		if( Entry.Name != NoInput && Entry.Name == Input && !Entry.Used )
		{
			if( ConsumeEntry )
			{
				Entry.Used = true;
			}
			return true;
		}
	}

	return false;
}

TArray<FString> UMovesBufferComponent::GetBufferedInputs() const
{
	TArray<FString> BufferedInputs;
	for( const FInputBufferEntry& Input : m_Buffer )
	{
		if( Input.Name != NoInput && !Input.Used )
		{
			BufferedInputs.Emplace( Input.Name );
		}
	}

	return BufferedInputs;
}

float UMovesBufferComponent::GetMovementDirection() const
{
	return m_MovementDirection;
}

void UMovesBufferComponent::AddMoveToBuffer( const FString& MoveName )
{
	m_Buffer.emplace_back( FInputBufferEntry{FString( MoveName ), false} );
	m_Buffer.pop_front();

	m_BufferChanged = true;
}

bool UMovesBufferComponent::BufferContainsConsumableInput( const FString& Input ) const
{
	for( const FInputBufferEntry& CurrentInput : m_Buffer )
	{
		if( CurrentInput.Name == Input && !CurrentInput.Used )
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
		m_Buffer.emplace_back( FInputBufferEntry{NoInput, false} );
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
	AddMoveToBuffer( EntryStartJump );
}

void UMovesBufferComponent::OnStopJump()
{
	AddMoveToBuffer( EntryStopJump );
}

void UMovesBufferComponent::OnAttack()
{
	AddMoveToBuffer( EntryAttack );
}

void UMovesBufferComponent::OnSpecial()
{
	AddMoveToBuffer( EntrySpecial );
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

void UMovesBufferComponent::UseBufferedInput( FString Input )
{
	verify( BufferContainsConsumableInput( Input ) );

	for( FInputBufferEntry& CurrentInput : m_Buffer )
	{
		if( CurrentInput.Name == Input )
		{
			CurrentInput.Used = true;
		}
	}
}
