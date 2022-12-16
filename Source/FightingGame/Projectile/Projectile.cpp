// Copyright (c) Giammarco Agazzotti

#include "Projectile.h"

#include "Components/SphereComponent.h"

AProjectile::AProjectile()
{
	PrimaryActorTick.bCanEverTick = true;

	m_MainCollision = CreateDefaultSubobject<USphereComponent>( TEXT( "Main Collision" ) );
	RootComponent   = m_MainCollision;
}

void AProjectile::BeginPlay()
{
	Super::BeginPlay();
}

void AProjectile::Init( TObjectPtr<AActor> OwnerActor, FVector Location, float HorizontalDirectionMultiplier, float Lifetime /* = -1.f*/ )
{
	m_Owner                         = OwnerActor;
	m_HorizontalDirectionMultiplier = HorizontalDirectionMultiplier;
	m_Lifetime                      = Lifetime;

	TeleportTo( Location, FRotator::ZeroRotator );

	FVector targetVelocity = FVector::RightVector * HorizontalDirectionMultiplier * m_BaseSpeed;
	m_MainCollision->SetPhysicsLinearVelocity( targetVelocity );

	if( m_Lifetime > 0.f )
	{
		GetWorldTimerManager().SetTimer( m_LifetimeTimerHandle, this, &AProjectile::OnLifetimeTimerEnded, m_Lifetime );
	}
}

void AProjectile::OnHitReceived( const HitData& HitData )
{
}

void AProjectile::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
}

void AProjectile::OnLifetimeTimerEnded()
{
	m_DestroyRequestedDelegate.Broadcast( this );
}
