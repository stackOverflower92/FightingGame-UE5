// Copyright (c) Giammarco Agazzotti

#include "CustomGameState.h"

void UCustomGameState::OnInit()
{
    ReceiveOnInit();
}

void UCustomGameState::OnEnter()
{
    ReceiveOnEnter();
}

void UCustomGameState::OnExit()
{
    ReceiveOnExit();
}

void UCustomGameState::OnTick( float DeltaTime )
{
    ReceiveOnTick( DeltaTime );
}
