// Copyright (c) Giammarco Agazzotti


#include "MoveTransition.h"

#include "FightingGame/Character/FightingCharacter.h"
#include "FightingGame/Input/MovesBufferComponent.h"

bool UMoveTransition::CanPerformTransition()
{
    if( m_RequireHitLanded )
    {
        if( m_Character->HasJustLandedHit() )
        {
            if( m_Character->GetMovesBufferComponent()->IsMoveBuffered( m_MoveName ) )
            {
                // #TODO is this correct? can this transition have the ownership of that value?
                m_Character->ResetHasJustLandedHit();
                return true;
            }

            return false;
        }
    }
    else
    {
        return m_Character->GetMovesBufferComponent()->IsMoveBuffered( m_MoveName );
    }

    return false;
}
