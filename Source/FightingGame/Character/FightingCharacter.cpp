#include "FightingCharacter.h"
#include "FSM.h"
#include "FightingGame/Common/FSMStatics.h"
#include "FightingGame/Input/MovesBufferComponent.h"
#include "FightingGame/Combat/HitboxHandlerComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include <Runtime/Engine/Classes/Kismet/KismetMathLibrary.h>

#include "FightingGame/Common/CombatStatics.h"
#include "FightingGame/Debug/Debug.h"
#include "Kismet/KismetSystemLibrary.h"

namespace
{
	int32 loc_DebugDamageStats = 0;
	FG_CVAR_DESC( CVarDebugDamageStats, TEXT("FightingCharacter.DebugDamageStats"), TEXT("1: enable, 0: disable"), loc_DebugDamageStats );

	int32 loc_DebugFacing = 0;
	FG_CVAR_DESC( CVarDebugFacing, TEXT("FightingCharacter.DebugFacing"), TEXT("1: enable, 0: disable"), loc_DebugFacing );
}

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
	AddMovementInput( FVector( 0.f, 1.f, 0.f ), m_CurrentHorizontalMovement );

	if( !IsAirborne() || m_UpdateFacingWhenAirborne )
	{
		if( !FMath::IsNearlyZero( m_CurrentHorizontalMovement ) )
		{
			m_TargetRotatorYaw = FMath::Sign( m_CurrentHorizontalMovement ) * 90.f;
		}
	}
}

void AFightingCharacter::BeginPlay()
{
	Super::BeginPlay();

	m_MovesBuffer->m_OwnerCharacter = this;

	UFSMStatics::Init( m_FSM, m_FirstState );

	m_HitDelegateHandle = m_HitboxHandler->m_HitDelegate.AddUObject( this, &AFightingCharacter::OnHitLanded );

	InitTimeDilations();
}

void AFightingCharacter::EndPlay( const EEndPlayReason::Type EndPlayReason )
{
	Super::EndPlay( EndPlayReason );

	m_HitboxHandler->m_HitDelegate.Remove( m_HitDelegateHandle );
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
	return m_KnockbackMultiplierCurve->GetFloatValue( m_DamagePercent );
}

float AFightingCharacter::GetDamagePercent() const
{
	return m_DamagePercent;
}

void AFightingCharacter::SetDamagePercent( float Percent )
{
	m_DamagePercent = FMath::Max( 0.f, Percent );
}

bool AFightingCharacter::IsAirKnockbackHappening() const
{
	return m_IsAirKnockbackHappening;
}

void AFightingCharacter::SetAirKnockbackHappening( bool Value )
{
	m_IsAirKnockbackHappening = Value;
}

void AFightingCharacter::EnableHitStun( bool Shake )
{
	m_TimeDilations.Push( UCombatStatics::GetMinCustomTimeDilation() );

	CustomTimeDilation = m_TimeDilations.Last();
}

void AFightingCharacter::DisableHitStun()
{
	m_TimeDilations.Pop();
	ensureMsgf( m_TimeDilations.Num() > 0, TEXT("Time dilations stack cannot be empty") );

	CustomTimeDilation = m_TimeDilations.Last();
}

void AFightingCharacter::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	if( loc_DebugDamageStats == 1 )
	{
		UKismetSystemLibrary::DrawDebugString( GetWorld(), GetActorLocation(),
		                                       FString::Printf( TEXT( "[DP: %.1f][KM: %.1f]" ), m_DamagePercent, GetKnockbackMultiplier() ) );
	}

	m_FacingRight = m_TargetRotatorYaw > 0.f && m_TargetRotatorYaw < 180.f;
	if( loc_DebugFacing == 1 )
	{
		UKismetSystemLibrary::DrawDebugString( GetWorld(), GetActorLocation(),
		                                       FString::Printf( TEXT( "[Facing Right: %s]" ), m_FacingRight ? TEXT( "TRUE" ) : TEXT( "FALSE" ) ) );
	}

	UpdateYaw( DeltaTime );

	CheckGroundedEvent();
	CheckAirborneEvent();
}

void AFightingCharacter::SetupPlayerInputComponent( UInputComponent* PlayerInputComponent )
{
	Super::SetupPlayerInputComponent( PlayerInputComponent );

	ensureMsgf( m_MovesBuffer, TEXT( "Moves buffer is null" ) );
	m_MovesBuffer->OnSetupPlayerInputComponent( PlayerInputComponent );
}

void AFightingCharacter::OnHitReceived( const HitData& HitData )
{
	if( !m_Hittable )
	{
		return;
	}

	if( HitData.m_ForceOpponentFacing )
	{
		UCombatStatics::FaceOther( this, HitData.m_Owner, true );
	}

	m_DamagePercent += HitData.m_DamagePercent;
	UCombatStatics::ApplyKnockbackTo( HitData.m_ProcessedKnockback, HitData.m_ProcessedKnockback.Length(), this );

	float DotAbs = FMath::Abs( FVector::DotProduct( GetActorForwardVector(), HitData.m_ProcessedKnockback.GetSafeNormal() ) );
	if( DotAbs < .9f && HitData.m_ProcessedKnockback.Length() >= 500.f )
	{
		UFSMStatics::SetState( m_FSM, m_GroundToAirReactionStateName );
	}
	else
	{
		UFSMStatics::SetState( m_FSM, m_GroundedReactionStateName );
	}

	if( HitData.m_HitStunDuration > 0.f )
	{
		StartBeginHitStunTimer( HitData );
	}
}

void AFightingCharacter::UpdateYaw( float DeltaTime )
{
	FRotator TargetRotator = GetActorRotation();
	TargetRotator.Yaw = m_TargetRotatorYaw;

	FRotator LerpRotator = UKismetMathLibrary::RLerp( GetActorRotation(), TargetRotator, m_FacingRotationLerpMultiplier * DeltaTime, true );

	SetActorRotation( LerpRotator );
}

void AFightingCharacter::OnHitLanded( AActor* Target, const HitData& HitData )
{
	m_HitLandedDelegate.Broadcast( Target );

	m_HasLandedHit = true;

	StartHitLandedTimer();

	if( HitData.m_HitStunDuration > 0.f )
	{
		StartBeginHitStunTimer( HitData );
	}
}

void AFightingCharacter::StartHitLandedTimer()
{
	if( GetWorldTimerManager().IsTimerActive( m_HitLandedStateTimerHandle ) )
	{
		GetWorldTimerManager().ClearTimer( m_HitLandedStateTimerHandle );
	}

	GetWorldTimerManager().SetTimer( m_HitLandedStateTimerHandle, this, &AFightingCharacter::OnHitLandedTimerEnded, m_HitLandedStateDuration );
}

void AFightingCharacter::OnHitLandedTimerEnded()
{
	m_HasLandedHit = false;
}

void AFightingCharacter::CheckGroundedEvent()
{
	if( IsAirborne() )
	{
		m_GroundedDelegateBroadcast = false;
	}
	else
	{
		if( !m_GroundedDelegateBroadcast )
		{
			m_GroundedDelegateBroadcast = true;
			m_GroundedDelegate.Broadcast();
		}
	}
}

void AFightingCharacter::CheckAirborneEvent()
{
	if( !IsAirborne() )
	{
		m_AirborneDelegateBroadcast = false;
	}
	else
	{
		if( !m_AirborneDelegateBroadcast )
		{
			m_AirborneDelegateBroadcast = true;
			m_AirborneDelegate.Broadcast();
		}
	}
}

void AFightingCharacter::InitTimeDilations()
{
	m_TimeDilations.Empty();
	m_TimeDilations.Push( CustomTimeDilation );
}

void AFightingCharacter::StartBeginHitStunTimer( const HitData& HitData )
{
	if( GetWorldTimerManager().IsTimerActive( m_HitStunBeginTimerHandle ) )
	{
		GetWorldTimerManager().ClearTimer( m_HitStunBeginTimerHandle );
	}

	m_CachedHitStunDuration = HitData.m_HitStunDuration;
	GetWorldTimerManager().SetTimer( m_HitStunBeginTimerHandle, this, &AFightingCharacter::OnHitStunBeginTimerEnded, UCombatStatics::GetHitStunInitialDelay() );
}

void AFightingCharacter::OnHitStunBeginTimerEnded()
{
	StartStopHitStunTimer();
}

void AFightingCharacter::OnHitStunStopTimerEnded()
{
	DisableHitStun();
}

void AFightingCharacter::StartStopHitStunTimer()
{
	if( GetWorldTimerManager().IsTimerActive( m_HitStunStopTimerHandle ) )
	{
		GetWorldTimerManager().ClearTimer( m_HitStunStopTimerHandle );
		DisableHitStun();
	}

	// #TODO pass shake from hitdata
	EnableHitStun( false );
	GetWorldTimerManager().SetTimer( m_HitStunStopTimerHandle, this, &AFightingCharacter::OnHitStunStopTimerEnded, m_CachedHitStunDuration );
}
