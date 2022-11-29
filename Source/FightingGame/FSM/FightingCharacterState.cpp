// Copyright (c) Giammarco Agazzotti

#include "FightingCharacterState.h"
#include "FightingGame/Character/FightingCharacter.h"
#include "FightingGame/Animation/FightingCharacterAnimInstance.h"

void UFightingCharacterState::Enter_Implementation()
{
	Super::Enter_Implementation();

	m_OwnerCharacter = Cast<AFightingCharacter>( ActorOwner );
	ensure( m_OwnerCharacter );

	m_AnimInstance = Cast<UFightingCharacterAnimInstance>( m_OwnerCharacter->GetMesh()->GetAnimInstance() );
	ensure( m_AnimInstance );

	m_AnimInstance->m_MontageEvent.AddDynamic( this, &UFightingCharacterState::OnMontageEvent );

	m_CharacterHitLandedHandle = m_OwnerCharacter->m_HitLandedDelegate.AddUObject( this, &UFightingCharacterState::OnCharacterHitLanded );
}

void UFightingCharacterState::Exit_Implementation()
{
	Super::Exit_Implementation();

	if( m_AnimInstance )
	{
		m_AnimInstance->m_MontageEvent.RemoveDynamic( this, &UFightingCharacterState::OnMontageEvent );
	}

	m_OwnerCharacter->m_HitLandedDelegate.Remove( m_CharacterHitLandedHandle );
}

void UFightingCharacterState::OnMontageEvent( UAnimMontage* Montage, EMontageEventType EventType )
{
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

void UFightingCharacterState::OnCharacterHitLanded_Implementation( AActor* Target )
{
}

void UFightingCharacterState::OnMontageEnded_Implementation( UAnimMontage* Montage )
{
}
