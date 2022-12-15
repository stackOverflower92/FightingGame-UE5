#include "FightingCharacter.h"
#include "FSM.h"
#include "FightingGame/Common/FSMStatics.h"
#include "FightingGame/Input/MovesBufferComponent.h"
#include "FightingGame/Combat/HitboxHandlerComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include <Runtime/Engine/Classes/Kismet/KismetMathLibrary.h>

#include "Components/BoxComponent.h"
#include "FightingGame/Combat/HitStopComponent.h"
#include "FightingGame/Common/CombatStatics.h"
#include "FightingGame/Debug/Debug.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

namespace
{
	int32 loc_DebugDamageStats = 0;
	FG_CVAR_DESC( CVarDebugDamageStats, TEXT("FightingCharacter.DebugDamageStats"), TEXT("1: enable, 0: disable"), loc_DebugDamageStats );

	int32 loc_DebugFacing = 0;
	FG_CVAR_DESC( CVarDebugFacing, TEXT("FightingCharacter.DebugFacing"), TEXT("1: enable, 0: disable"), loc_DebugFacing );

	int32 loc_DebugEnableShake = 0;
	FG_CVAR_DESC( CVarDebugEnableShake, TEXT("FightingCharacter.DebugEnableShake"), TEXT("1: enable, 0: disable"), loc_DebugEnableShake );
}

AFightingCharacter::AFightingCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	m_FSM              = CreateDefaultSubobject<UFSM>( TEXT( "FSM" ) );
	m_MovesBuffer      = CreateDefaultSubobject<UMovesBufferComponent>( TEXT( "Moves Buffer" ) );
	m_HitboxHandler    = CreateDefaultSubobject<UHitboxHandlerComponent>( TEXT( "Hitbox Handler" ) );
	m_HitStopComponent = CreateDefaultSubobject<UHitStopComponent>( TEXT( "Hit Stop" ) );
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
			float HorizontalMovementSign = FMath::Sign( m_CurrentHorizontalMovement );
			m_TargetRotatorYaw           = HorizontalMovementSign * 90.f;
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

	m_InitialMeshRelativeLocation = GetMesh()->GetRelativeLocation();

	InitPushbox();

	GetHitboxHandler()->SetReferenceComponent( GetMesh() );
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
		TargetRotation.Yaw      = m_TargetRotatorYaw;

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

void AFightingCharacter::PushTimeDilation( float Value )
{
	m_TimeDilations.Push( Value );

	CustomTimeDilation = m_TimeDilations.Last();
}

void AFightingCharacter::PopTimeDilation()
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
	UpdateYaw( DeltaTime );

	if( loc_DebugFacing == 1 )
	{
		UKismetSystemLibrary::DrawDebugString( GetWorld(), GetActorLocation(),
		                                       FString::Printf( TEXT( "[Facing Right: %s]" ), m_FacingRight ? TEXT( "TRUE" ) : TEXT( "FALSE" ) ) );
	}

	CheckGroundedEvent();
	CheckAirborneEvent();

	if( m_CanUpdateMeshShake || loc_DebugEnableShake )
	{
		UpdateMeshShake();
	}

	UpdatePushbox( DeltaTime );

	UpdateGravityScale();
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

	if( HitData.m_HitStopDuration > 0.f )
	{
		GetHitStopComponent()->EnableHitStop( HitData.m_HitStopDuration, HitData.m_Shake );
	}
}

void AFightingCharacter::UpdateYaw( float DeltaTime )
{
	FRotator TargetRotator = GetActorRotation();
	TargetRotator.Yaw      = m_TargetRotatorYaw;

	FRotator UpdatedRotator;
	if( m_FacingRotationLerpMultiplier > 0.f )
	{
		UpdatedRotator = UKismetMathLibrary::RLerp( GetActorRotation(), TargetRotator, m_FacingRotationLerpMultiplier * DeltaTime, true );
	}
	else
	{
		UpdatedRotator = TargetRotator;
	}

	SetActorRotation( UpdatedRotator );
}

void AFightingCharacter::UpdateVerticalScale()
{
	FVector Scale = GetMesh()->GetRelativeScale3D();
	float YScale  = m_FacingRight ? FMath::Abs( Scale.Y ) : -FMath::Abs( Scale.Y );
	Scale.Y       = YScale;

	GetMesh()->SetRelativeScale3D( Scale );
}

void AFightingCharacter::OnHitLanded( AActor* Target, const HitData& HitData )
{
	m_HitLandedDelegate.Broadcast( Target );

	m_HasJustLandedHit = true;

	StartHitLandedTimer();

	if( HitData.m_HitStopDuration > 0.f )
	{
		GetHitStopComponent()->EnableHitStop( HitData.m_HitStopDuration, false );
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
	m_HasJustLandedHit = false;
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

void AFightingCharacter::InitPushbox()
{
	TArray<UActorComponent*> PushBoxes = GetComponentsByTag( UBoxComponent::StaticClass(), TEXT( "Pushbox" ) );
	ensure( PushBoxes.Num() <= 1 );

	if( PushBoxes.Num() > 0 )
	{
		m_Pushbox = Cast<UBoxComponent>( PushBoxes[0] );
		if( !m_Pushbox )
		{
			FG_SLOG_WARN( FString::Printf( TEXT( "Character [%s] has no Pushbox available; will overlap with other characters" ), *GetName() ) );
		}
	}
}

void AFightingCharacter::UpdatePushbox( float DeltaTime )
{
	if( m_Pushbox )
	{
		TArray<UPrimitiveComponent*> OverlappingComponents;
		m_Pushbox->GetOverlappingComponents( OverlappingComponents );

		// TODO: temp, let's start by handling a single other enemy
		if( OverlappingComponents.Num() == 1 )
		{
			AActor* OtherActor         = OverlappingComponents[0]->GetOwner();
			bool OtherOnTheRight       = OtherActor->GetActorLocation().Y > GetActorLocation().Y;
			float MyShiftingMultiplier = OtherOnTheRight ? -1.f : 1.f;

			const FVector CurrentLocation = GetActorLocation();
			const float NextPosition      = CurrentLocation.Y + (m_PushboxShiftRatePerFrame * MyShiftingMultiplier * DeltaTime);
			const FVector NextLocation    = FVector( CurrentLocation.X, NextPosition, CurrentLocation.Z );

			SetActorLocation( NextLocation );
		}
	}
}

void AFightingCharacter::UpdateGravityScale()
{
	float verticalVelocity = GetVelocity().Z;
	if( verticalVelocity < 0.f )
	{
		GetCharacterMovement()->GravityScale = m_FallingGravityScale;
	}
	else
	{
		GetCharacterMovement()->GravityScale = m_RegularGravityScale;
	}
}

void AFightingCharacter::UpdateMeshShake()
{
	float ElapsedTime = UGameplayStatics::GetRealTimeSeconds( GetWorld() );
	float DeltaShake  = FMath::Sin( ElapsedTime * m_MeshShakeFrequency ) * m_MeshShakeAmplitude;

	FVector CurrentPosition = GetMesh()->GetRelativeLocation();
	CurrentPosition.X += DeltaShake;

	if( IsAirborne() )
	{
		CurrentPosition.Z += DeltaShake;
	}

	GetMesh()->SetRelativeLocation( CurrentPosition, false, nullptr, ETeleportType::TeleportPhysics );
}

void AFightingCharacter::ResetMeshRelativeLocation()
{
	GetMesh()->SetRelativeLocation( m_InitialMeshRelativeLocation );
}
