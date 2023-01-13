// Copyright (c) Giammarco Agazzotti


#include "StateMachineState.h"

void UStateMachineState::OnInit()
{
    ReceiveOnInit();
}

void UStateMachineState::OnEnter()
{
    ReceiveOnEnter();
}

void UStateMachineState::OnExit()
{
    ReceiveOnExit();
}

void UStateMachineState::OnTick( float DeltaTime )
{
    // #TODO re-enable
    ReceiveOnTick( DeltaTime );
}
