// Copyright (c) Giammarco Agazzotti

#include "HitboxVisualizer.h"

AHitboxVisualizer::AHitboxVisualizer()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AHitboxVisualizer::BeginPlay()
{
	Super::BeginPlay();
}

void AHitboxVisualizer::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	if( m_Knockback.IsSet() )
	{
		FVector lineEnd = GetActorLocation() + m_Knockback.GetValue().GetSafeNormal() * m_Radius;
		DrawDebugLine( GetWorld(), GetActorLocation(), lineEnd, FColor::White, false, -1, SDPG_Foreground, m_KnockbackLineThickness );
	}
}
