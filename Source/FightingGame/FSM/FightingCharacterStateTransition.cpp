// Copyright (c) Giammarco Agazzotti


#include "FightingCharacterStateTransition.h"

void UFightingCharacterStateTransition::OnStateEnter( TObjectPtr<AFightingCharacter> Character )
{
	m_Character = Character;
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

void UFightingCharacterStateTransition::OnAirborne()
{
}

bool UFightingCharacterStateTransition::CanPerformTransition()
{
	return false;
}
