#include "FightingCharacter.h"
#include "FSM.h"
#include "FightingGame/Common/FSMStatics.h"
#include "FightingGame/Input/MovesBufferComponent.h"
#include "FightingGame/Combat/HitboxHandlerComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include <Runtime/Engine/Classes/Kismet/KismetMathLibrary.h>

#include "FightingGame/Common/CombatStatics.h"

AFightingCharacter::AFightingCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	m_FSM = CreateDefaultSubobject<UFSM>( TEXT( "FSM" ) );
	m_MovesBuffer = CreateDefaultSubobject<UMovesBufferComponent>( TEXT( "Moves Buffer" ) );
	m_HitboxHandler = CreateDefaultSubobject<UHitboxHandlerComponent>( TEXT( "Hitbox Handler" ) );
}

bool AFightingCharacter::IsAirborne() const
{
	return GetCharacterMovement()->IsFalling();
}

void AFightingCharacter::UpdateHorizontalMovement( float value )
{
	m_CurrentHorizontalMovement = value;
	AddMovementInput( FVector( 0.f, -1.f, 0.f ), m_CurrentHorizontalMovement );

	if( !FMath::IsNearlyZero( m_CurrentHorizontalMovement ) )
	{
		m_TargetRotatorYaw = -FMath::Sign( m_CurrentHorizontalMovement ) * 90.f;
	}
}

void AFightingCharacter::BeginPlay()
{
	Super::BeginPlay();

	UFSMStatics::Init( m_FSM, m_FirstState );
}

bool AFightingCharacter::IsFacingRight() const
{
	return m_FacingRight;
}

void AFightingCharacter::SetFacingRight( bool Right, bool Instant /*= false*/ )
{
	m_TargetRotatorYaw = Right ? 90.f : -90.f;

	if( Instant )
	{
		FRotator TargetRotation = GetActorRotation();
		TargetRotation.Yaw = m_TargetRotatorYaw;
		SetActorRotation( TargetRotation );
	}
}

float AFightingCharacter::GetKnockbackMultiplier() const
{
	// #TODO implement
	return 1.f;
}

void AFightingCharacter::ShowHitTraces( bool Show )
{
	if( m_HitboxHandler )
	{
		m_HitboxHandler->ShowDebugTraces( Show );
	}
}

void AFightingCharacter::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	m_FacingRight = m_TargetRotatorYaw < 0.f && m_TargetRotatorYaw > -180.f;

	if( !IsAirborne() || m_UpdateFacingWhenAirborne )
	{
		UpdateYaw( DeltaTime );
	}
}

void AFightingCharacter::SetupPlayerInputComponent( UInputComponent* PlayerInputComponent )
{
	Super::SetupPlayerInputComponent( PlayerInputComponent );

	ensureMsgf( m_MovesBuffer, TEXT( "Moves buffer is null" ) );
	m_MovesBuffer->OnSetupPlayerInputComponent( PlayerInputComponent );
}

void AFightingCharacter::OnHit( const FHitData& HitData )
{
	if( HitData.m_ForceOpponentFacing )
	{
		UCombatStatics::FaceOther( this, HitData.m_Owner, true );
	}

	UFSMStatics::SetState( m_FSM, TEXT( "REACTION_LIGHT_GROUNDED" ) );
}

void AFightingCharacter::UpdateYaw( float DeltaTime )
{
	FRotator TargetRotator = GetActorRotation();
	TargetRotator.Yaw = m_TargetRotatorYaw;

	FRotator LerpRotator = UKismetMathLibrary::RLerp( GetActorRotation(), TargetRotator, m_FacingRotationLerpMultiplier * DeltaTime, true );

	SetActorRotation( LerpRotator );
}
