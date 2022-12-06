// Copyright (c) Giammarco Agazzotti

#include "FightingCharacterState.h"

#include "FightingCharacterStateTransition.h"
#include "FightingGame/Character/FightingCharacter.h"
#include "FightingGame/Animation/FightingCharacterAnimInstance.h"
#include "FightingGame/Common/CombatStatics.h"
#include "FightingGame/Common/FSMStatics.h"
#include "FightingGame/Input/MovesBufferComponent.h"

void UFightingCharacterState::Init_Implementation()
{
	Super::Init_Implementation();

	m_OwnerCharacter = Cast<AFightingCharacter>( ActorOwner );
	ensure( m_OwnerCharacter );

	m_AnimInstance = Cast<UFightingCharacterAnimInstance>( m_OwnerCharacter->GetMesh()->GetAnimInstance() );
	ensure( m_AnimInstance );
}

void UFightingCharacterState::Enter_Implementation()
{
	Super::Enter_Implementation();

	m_AnimInstance->m_MontageEvent.AddDynamic( this, &UFightingCharacterState::OnMontageEvent );

	m_CharacterHitLandedHandle = m_OwnerCharacter->m_HitLandedDelegate.AddUObject( this, &UFightingCharacterState::OnCharacterHitLanded );
	m_CharacterGroundedHandle = m_OwnerCharacter->m_GroundedDelegate.AddUObject( this, &UFightingCharacterState::OnCharacterGrounded );
	m_CharacterAirborneHandle = m_OwnerCharacter->m_AirborneDelegate.AddUObject( this, &UFightingCharacterState::OnCharacterAirborne );

	if( m_MoveToExecute )
	{
		UCombatStatics::ExecuteMove( m_OwnerCharacter, m_MoveToExecute );
	}

	// #TODO what happens with AI?
	if( m_IsReaction && m_OwnerCharacter->IsPlayerControlled() )
	{
		m_OwnerCharacter->DisableInput( Cast<APlayerController>( m_OwnerCharacter->GetController() ) );
	}

	for( auto Pair : m_Transitions )
	{
		Pair.Value->OnStateEnter( m_OwnerCharacter );
	}
}

void UFightingCharacterState::Exit_Implementation()
{
	Super::Exit_Implementation();

	if( m_AnimInstance )
	{
		m_AnimInstance->m_MontageEvent.RemoveDynamic( this, &UFightingCharacterState::OnMontageEvent );
	}

	m_OwnerCharacter->m_HitLandedDelegate.Remove( m_CharacterHitLandedHandle );
	m_OwnerCharacter->m_GroundedDelegate.Remove( m_CharacterGroundedHandle );
	m_OwnerCharacter->m_AirborneDelegate.Remove( m_CharacterAirborneHandle );

	// #TODO what happens with AI?
	if( m_IsReaction && m_OwnerCharacter->IsPlayerControlled() )
	{
		m_OwnerCharacter->EnableInput( Cast<APlayerController>( m_OwnerCharacter->GetController() ) );
	}

	for( auto Pair : m_Transitions )
	{
		Pair.Value->OnStateExit();
	}
}

void UFightingCharacterState::Update_Implementation( float DeltaTime )
{
	Super::Update_Implementation( DeltaTime );

	for( auto Pair : m_Transitions )
	{
		if( Pair.Value->CanPerformTransition() )
		{
			UFSMStatics::SetState( FSMOwner, Pair.Key );
		}
	}

	if( m_UpdateMovement )
	{
		m_OwnerCharacter->UpdateHorizontalMovement( m_OwnerCharacter->GetMovesBufferComponent()->m_InputMovement );
	}
}

void UFightingCharacterState::OnMontageEvent( UAnimMontage* Montage, EMontageEventType EventType )
{
	for( auto Pair : m_Transitions )
	{
		Pair.Value->OnMontageEvent( Montage, EventType );
	}

	// #TODO handle other cases
	switch( EventType )
	{
	case EMontageEventType::Ended:
		{
			OnMontageEnded( Montage );
			break;
		}
	}
}

void UFightingCharacterState::OnCharacterAirborne_Implementation()
{
	for( auto Pair : m_Transitions )
	{
		Pair.Value->OnAirborne();
	}
}

void UFightingCharacterState::OnCharacterGrounded_Implementation()
{
	for( auto Pair : m_Transitions )
	{
		Pair.Value->OnGrounded();
	}
}

void UFightingCharacterState::OnCharacterHitLanded_Implementation( AActor* Target )
{
}

void UFightingCharacterState::OnMontageEnded_Implementation( UAnimMontage* Montage )
{
}
