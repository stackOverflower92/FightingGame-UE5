// Copyright (c) Giammarco Agazzotti


#include "MontageEndedTransition.h"

#include "FightingGame/Animation/FightingCharacterAnimInstance.h"

bool UMontageEndedTransition::CanPerformTransition()
{
	return m_CanTransition;
}

void UMontageEndedTransition::OnStateEnter()
{
	Super::OnStateEnter();

	m_CanTransition = false;
}

void UMontageEndedTransition::OnMontageEvent( UAnimMontage* Montage, EMontageEventType EventType )
{
	switch( EventType )
	{
	case EMontageEventType::Ended:
		{
			m_CanTransition = true;
			break;
		}
	}
}
