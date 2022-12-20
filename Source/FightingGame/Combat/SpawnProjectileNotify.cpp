// Copyright (c) Giammarco Agazzotti


#include "SpawnProjectileNotify.h"

#include "FightingGame/Character/FightingCharacter.h"
#include "FightingGame/Projectile/ProjectileSpawnerComponent.h"

void USpawnProjectileNotify::Notify( USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference )
{
	Super::Notify( MeshComp, Animation, EventReference );

	if( auto* character = Cast<AFightingCharacter>( MeshComp->GetOwner() ) )
	{
		FVector rotatedOffset = m_SpawnOffset;
		rotatedOffset.Y *= character->IsFacingRight() ? 1.f : -1.f;

		FVector spawnLocation      = character->GetActorLocation() + rotatedOffset;
		float horizontalMultiplier = character->IsFacingRight() ? 1.f : -1.f;

		character->GetProjectileSpawnerComponent()->SpawnProjectile( m_ProjectileClass, spawnLocation, horizontalMultiplier, m_BaseSpeed, m_Lifetime );
	}
}
