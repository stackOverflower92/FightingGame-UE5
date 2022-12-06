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

void AProp::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
}
