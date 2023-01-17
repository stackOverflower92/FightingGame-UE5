// Copyright (c) Giammarco Agazzotti


#include "Prop.h"


AProp::AProp()
{
    PrimaryActorTick.bCanEverTick = true;
}

void AProp::BeginPlay()
{
    Super::BeginPlay();
}

void AProp::OnHitReceived( const HitData& HitData )
{
}

bool AProp::IsHittable()
{
    return true;
}

bool AProp::IsBlocking()
{
    return false;
}

void AProp::Tick( float DeltaTime )
{
    Super::Tick( DeltaTime );
}
