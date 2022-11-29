// Copyright (c) Giammarco Agazzotti

#include "FSMStatics.h"
#include "FSM.h"

namespace
{
	const FName loc_DefaultStateName = FName( "DEFAULT_STATE" );
}

bool UFSMStatics::Init( UFSM* Fsm, FName FirstStateName )
{
	if( !Fsm )
	{
		UE_LOG( LogTemp, Error, TEXT( "Fsm is null" ) );
		return false;
	}

	Fsm->Start();

	if( !Fsm->DoesStateExist( loc_DefaultStateName ) )
	{
		UE_LOG( LogTemp, Error, TEXT( "Default state does not exist in FSM" ) );
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
		UE_LOG( LogTemp, Error, TEXT( "At this point the fsm should have the default state at the top of the stack." ) );
		return false;
	}

	if( !Fsm->DoesStateExist( StateName ) )
	{
		UE_LOG( LogTemp, Error, TEXT( "Trying to push state [%s] that does not exist." ), *StateName.ToString() );
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
		UE_LOG( LogTemp, Error, TEXT( "Trying to push state [%s] that does not exist." ), *StateName.ToString() );
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
		UE_LOG( LogTemp, Error, TEXT( "Fsm is null" ) );
		return false;
	}

	if( !Fsm->IsMachineRunning() )
	{
		UE_LOG( LogTemp, Error, TEXT( "Fsm must be running to perform this operation" ) );
		return false;
	}

	return true;
}
