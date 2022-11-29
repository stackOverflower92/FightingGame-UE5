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
}

void UFightingCharacterState::Exit_Implementation()
{
	Super::Exit_Implementation();

	if( m_AnimInstance )
	{
		m_AnimInstance->m_MontageEvent.RemoveDynamic( this, &UFightingCharacterState::OnMontageEvent );
	}
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

void UFightingCharacterState::OnMontageEnded_Implementation( UAnimMontage* Montage )
{

}