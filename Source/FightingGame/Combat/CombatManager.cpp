// Copyright (c) Giammarco Agazzotti

#include "CombatManager.h"

ACombatManager::ACombatManager()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ACombatManager::BeginPlay()
{
	Super::BeginPlay();
}

void ACombatManager::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
}
