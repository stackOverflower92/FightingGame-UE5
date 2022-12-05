// Copyright (c) Giammarco Agazzotti


#include "InputTransition.h"

#include "FightingGame/Character/FightingCharacter.h"
#include "FightingGame/Input/MovesBufferComponent.h"

bool UInputTransition::CanPerformTransition()
{
	return m_Character->GetMovesBufferComponent()->IsInputBuffered( m_InputEntry );
}
