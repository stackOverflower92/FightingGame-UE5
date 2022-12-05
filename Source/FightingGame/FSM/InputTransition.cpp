// Copyright (c) Giammarco Agazzotti


#include "InputTransition.h"

#include "FightingGame/Character/FightingCharacter.h"
#include "FightingGame/Input/MovesBufferComponent.h"

bool UInputTransition::CanPerformTransition()
{
	if( m_RequireHitLanded )
	{
		if( m_Character->HasJustLandedHit() )
		{
			return m_Character->GetMovesBufferComponent()->IsInputBuffered( m_InputEntry );
		}
	}
	else
	{
		return m_Character->GetMovesBufferComponent()->IsInputBuffered( m_InputEntry );
	}

	return false;
}
