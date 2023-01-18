// Copyright (c) Giammarco Agazzotti

#include "HUDBase.h"

AHUDBase::AHUDBase()
{
    PrimaryActorTick.bCanEverTick = true;
}

void AHUDBase::BeginPlay()
{
    Super::BeginPlay();
}

void AHUDBase::Tick( float DeltaTime )
{
    Super::Tick( DeltaTime );
}

void AHUDBase::OnReady()
{
}
