// Copyright (c) Giammarco Agazzotti

#include "HitStopComponent.h"

#include "FightingGame/Character/FightingCharacter.h"
#include "FightingGame/Common/CombatStatics.h"
#include "FightingGame/Debugging/Debug.h"
#include "Kismet/KismetSystemLibrary.h"

namespace
{
	int32 loc_ShowHitStopState = 0;
	FG_CVAR_DESC( CVarShowHitStopState, TEXT( "HitStopComponent.ShowHitStopState" ), TEXT("1: Enable, 0: Disable"), loc_ShowHitStopState );
}

UHitStopComponent::UHitStopComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UHitStopComponent::BeginPlay()
{
	Super::BeginPlay();

	m_Character = Cast<AFightingCharacter>( GetOwner() );
	if( !m_Character )
	{
		FG_SLOG_ERR( TEXT("Cast to AFightingCharacter failed.") );
	}
}

void UHitStopComponent::EnableHitStop( float Duration, bool Shake )
{
	StartBeginHitStopTimer( Duration, Shake );
}

void UHitStopComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	if( m_UpdateMeshShake )
	{
		m_Character->UpdateMeshShake();
	}

	if( loc_ShowHitStopState )
	{
		UKismetSystemLibrary::DrawDebugString( GetWorld(), GetOwner()->GetActorLocation(),
		                                       FString::Printf( TEXT( "[Hit Stop: %s]" ), m_HitStopRunning ? TEXT( "TRUE" ) : TEXT( "FALSE" ) ) );
	}
}

void UHitStopComponent::StartBeginHitStopTimer( float Duration, bool Shake )
{
	FTimerManager& TimerManager = GetOwner()->GetWorldTimerManager();

	if( TimerManager.IsTimerActive( m_HitStopBeginTimerHandle ) )
	{
		TimerManager.ClearTimer( m_HitStopBeginTimerHandle );
	}

	m_CachedHitStopDuration = Duration;
	m_CachedDoMeshShake     = Shake;

	// #TODO instead of using GetHitStunInitialDelay, interpolate directly to the next reaction animation to ensure the correcto pose is always visible
	float hitStunInitialDelay = UCombatStatics::GetHitStunInitialDelay();
	if( hitStunInitialDelay > 0.f )
	{
		TimerManager.SetTimer( m_HitStopBeginTimerHandle, this, &UHitStopComponent::OnHitStopBeginTimerEnded, UCombatStatics::GetHitStunInitialDelay() );
	}
	else
	{
		OnHitStopBeginTimerEnded();
	}
}

void UHitStopComponent::OnHitStopBeginTimerEnded()
{
	StartStopHitStopTimer();

	if( m_CachedDoMeshShake )
	{
		m_UpdateMeshShake = true;
	}
}

void UHitStopComponent::StartStopHitStopTimer()
{
	FTimerManager& TimerManager = GetOwner()->GetWorldTimerManager();

	if( TimerManager.IsTimerActive( m_HitStopStopTimerHandle ) )
	{
		TimerManager.ClearTimer( m_HitStopStopTimerHandle );
		m_Character->PopTimeDilation();
	}

	m_HitStopRunning = true;

	// #TODO pass shake from hitdata
	m_Character->PushTimeDilation( UCombatStatics::GetMinCustomTimeDilation() );
	TimerManager.SetTimer( m_HitStopStopTimerHandle, this, &UHitStopComponent::OnHitStopStopTimerEnded, m_CachedHitStopDuration );
}

void UHitStopComponent::OnHitStopStopTimerEnded()
{
	m_Character->PopTimeDilation();
	m_UpdateMeshShake = false;

	m_Character->ResetMeshRelativeLocation();

	m_HitStopRunning = false;
}
