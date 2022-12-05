// Copyright (c) Giammarco Agazzotti

#include "MovementTransition.h"

#include "FightingGame/Character/FightingCharacter.h"
#include "FightingGame/Input/MovesBufferComponent.h"

bool UMovementTransition::CanPerformTransition()
{
	return FMath::Abs( m_Character->GetMovesBufferComponent()->m_InputMovement ) > m_Threshold;
}
