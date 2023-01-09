// Copyright (c) Giammarco Agazzotti

#include "InputsSequenceTransition.h"

#include "FightingGame/Character/FightingCharacter.h"
#include "FightingGame/Input/MovesBufferComponent.h"

bool UInputsSequenceTransition::CanPerformTransition()
{
    if( m_RequireHitLanded )
    {
        if( m_Character->HasJustLandedHit() )
        {
            if( m_Character->GetMovesBufferComponent()->IsInputsSequenceBuffered( m_InputsSequenceName ) )
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
        return m_Character->GetMovesBufferComponent()->IsInputsSequenceBuffered( m_InputsSequenceName );
    }

    return false;
}
