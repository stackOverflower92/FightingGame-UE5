// Copyright (c) Giammarco Agazzotti

#include "FSMStatics.h"
#include "FSM.h"
#include "FightingGame/Debugging/Debug.h"

namespace
{
	const FName loc_DefaultStateName = TEXT( "DEFAULT_STATE" );
}

bool UFSMStatics::Init( UFSM* Fsm, FName FirstStateName )
{
	if( !Fsm )
	{
		FG_SLOG_ERR( TEXT( "Fsm is null" ) );
		return false;
	}

	Fsm->Start();

	if( !Fsm->DoesStateExist( loc_DefaultStateName ) )
	{
		FG_SLOG_ERR( TEXT( "Default state does not exist in FSM" ) );
		return false;
	}

	Fsm->PushState( FirstStateName );

	return true;
}

bool UFSMStatics::SetState( UFSM* Fsm, FName StateName )
{
	if( !IsFSMValid( Fsm ) ) return false;

	Fsm->PopActiveState();
	if( Fsm->GetActiveStateName() != loc_DefaultStateName )
	{
		FG_SLOG_ERR( FString::Printf( TEXT( "At this point the fsm should have the default state at the top of the stack." ) ) );
		return false;
	}

	if( !Fsm->DoesStateExist( StateName ) )
	{
		FG_SLOG_ERR( FString::Printf( TEXT( "Trying to push state [%s] that does not exist." ), *StateName.ToString()) );
		return false;
	}

	Fsm->PushState( StateName );

	return true;
}

bool UFSMStatics::PushState( UFSM* Fsm, FName StateName )
{
	if( !IsFSMValid( Fsm ) ) return false;

	if( !Fsm->DoesStateExist( StateName ) )
	{
		FG_SLOG_ERR( FString::Printf(TEXT( "Trying to push state [%s] that does not exist." ), *StateName.ToString() ) );
		return false;
	}

	Fsm->PushState( StateName );

	return true;
}

bool UFSMStatics::PopState( UFSM* Fsm )
{
	if( !IsFSMValid( Fsm ) ) return false;

	Fsm->PopActiveState();

	return true;
}

bool UFSMStatics::IsFSMValid( UFSM* Fsm )
{
	if( !Fsm )
	{
		FG_SLOG_ERR( TEXT( "Fsm is null" ) );
		return false;
	}

	if( !Fsm->IsMachineRunning() )
	{
		FG_SLOG_ERR( TEXT( "Fsm must be running to perform this operation" ) );
		return false;
	}

	return true;
}
