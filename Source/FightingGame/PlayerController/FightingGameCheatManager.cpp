// Copyright (c) Giammarco Agazzotti


#include "FightingGameCheatManager.h"

#include "FightingGame/Character/FightingCharacter.h"

void UFightingGameCheatManager::ShowHitTraces( int32 Show )
{
	GEngine->AddOnScreenDebugMessage( -1, 1.f, FColor::Magenta, TEXT( "ShowHitTraces" ) );

	if( auto* Pawn = GetOuterAPlayerController()->GetPawn() )
	{
		if( auto* FC = Cast<AFightingCharacter>( Pawn ) )
		{
			FC->ShowHitTraces( Show != 0 );
		}
	}
}
