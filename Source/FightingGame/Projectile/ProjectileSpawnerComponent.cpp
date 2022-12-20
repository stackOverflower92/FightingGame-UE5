// Copyright (c) Giammarco Agazzotti

#include "ProjectileSpawnerComponent.h"

#include "Projectile.h"

UProjectileSpawnerComponent::UProjectileSpawnerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UProjectileSpawnerComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UProjectileSpawnerComponent::SpawnProjectile( TSubclassOf<AProjectile> ProjectileClass, FVector SpawnLocation, float HorizontalDirectionMultiplier,
                                                   float BaseSpeed, float Lifetime )
{
	ensureMsgf( ProjectileClass, TEXT("Projectile class is null") );

	TObjectPtr<AProjectile> inst = GetWorld()->SpawnActor<AProjectile>( ProjectileClass, SpawnLocation, FRotator::ZeroRotator );
	inst->Init( GetOwner(), SpawnLocation, HorizontalDirectionMultiplier, BaseSpeed, Lifetime );
	inst->m_DestroyRequestedDelegate.AddUObject( this, &UProjectileSpawnerComponent::OnProjectileDestroyRequested );

	m_AliveProjectiles.Emplace( inst );
}

void UProjectileSpawnerComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );
}

void UProjectileSpawnerComponent::OnProjectileDestroyRequested( TObjectPtr<AProjectile> Projectile )
{
	m_AliveProjectiles.Remove( Projectile );

	// #TODO temp
	GetWorld()->DestroyActor( Projectile );
}
