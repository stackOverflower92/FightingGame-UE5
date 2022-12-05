// Copyright (c) Giammarco Agazzotti


#include "FightingCharacterStateTransition.h"

void UFightingCharacterStateTransition::Init( TObjectPtr<AFightingCharacter> Character )
{
	m_Character = Character;
}

void UFightingCharacterStateTransition::OnStateEnter()
{
}

void UFightingCharacterStateTransition::OnStateExit()
{
}

void UFightingCharacterStateTransition::OnMontageEvent( UAnimMontage* Montage, EMontageEventType MontageEvent )
{
}

void UFightingCharacterStateTransition::OnGrounded()
{
}

bool UFightingCharacterStateTransition::CanPerformTransition()
{
	return false;
}
