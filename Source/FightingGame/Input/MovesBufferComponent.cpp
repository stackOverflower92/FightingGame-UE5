// Copyright (c) Giammarco Agazzotti

#include "MovesBufferComponent.h"
#include "Components/InputComponent.h"

namespace
{
	const FString NoInput{""};

	const FString EntryStartJump{"StartJump"};
	const FString EntryStopJump{"StopJump"};
	const FString EntryStartMove{"StartMove"};
	const FString EntryStartMoveRight{"StartMoveRight"};
	const FString EntryStartMoveLeft{"StartMoveLeft"};
	const FString EntryStopMove{"StopMove"};
	const FString EntryAttack{"Attack"};
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

	for( int i = 0; i < m_Buffer.size(); ++i )
	{
		FInputBufferEntry& Entry = m_Buffer.at( i );
		const bool IsEmpty = Entry.Name == NoInput;
		auto Message = IsEmpty ? "Empty" : Entry.Name;

		GEngine->AddOnScreenDebugMessage( i, 1.f, FColor::White, FString::Printf( TEXT( "%s" ), *Message ) );
	}

	m_BufferChanged = false;

	UpdateMovement();
}

void UMovesBufferComponent::OnSetupPlayerInputComponent( UInputComponent* PlayerInputComponent )
{
	m_PlayerInput = PlayerInputComponent;
	if( m_PlayerInput )
	{
		m_PlayerInput->BindAction( "Jump", IE_Pressed, this, &UMovesBufferComponent::OnStartJump );
		m_PlayerInput->BindAction( "Jump", IE_Released, this, &UMovesBufferComponent::OnStopJump );
		m_PlayerInput->BindAction( "MoveRight", IE_Pressed, this, &UMovesBufferComponent::OnStartMoveRight );
		m_PlayerInput->BindAction( "MoveRight", IE_Released, this, &UMovesBufferComponent::OnStopMoveRight );
		m_PlayerInput->BindAction( "MoveLeft", IE_Pressed, this, &UMovesBufferComponent::OnStartMoveLeft );
		m_PlayerInput->BindAction( "MoveLeft", IE_Released, this, &UMovesBufferComponent::OnStopMoveLeft );
		m_PlayerInput->BindAction( "Attack", IE_Pressed, this, &UMovesBufferComponent::OnAttack );

		m_PlayerInput->BindAxis( "MoveHorizontal", this, &UMovesBufferComponent::OnMoveHorizontal );
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
	m_InputMovement = Value;

	m_MovingRight = Value > 0.f;
	m_MovingLeft = Value < 0.f;
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

void UMovesBufferComponent::OnStartMoveRight()
{
	AddMoveToBuffer( EntryStartMoveRight );

	m_MovingRight = true;
}

void UMovesBufferComponent::OnStopMoveRight()
{
	AddMoveToBuffer( EntryStopMove );

	m_MovingRight = false;
}

void UMovesBufferComponent::OnStartMoveLeft()
{
	AddMoveToBuffer( EntryStartMoveLeft );

	m_MovingLeft = true;
}

void UMovesBufferComponent::OnStopMoveLeft()
{
	AddMoveToBuffer( EntryStopMove );

	m_MovingLeft = false;
}

void UMovesBufferComponent::OnAttack()
{
	AddMoveToBuffer( EntryAttack );
}

void UMovesBufferComponent::UpdateMovement()
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
