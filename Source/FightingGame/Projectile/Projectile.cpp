﻿// Copyright (c) Giammarco Agazzotti

#include "Projectile.h"

#include "Components/SphereComponent.h"
#include "FightingGame/Combat/HitboxHandlerComponent.h"
#include "FightingGame/Debugging/Debug.h"
#include "Kismet/KismetSystemLibrary.h"

namespace
{
    int32 loc_ProjectileDebugFacing = 0;
    FG_CVAR_FLAG_DESC( CVarProjectileDebugFacing, TEXT("Projectile.DebugFacing"), loc_ProjectileDebugFacing );
}

AProjectile::AProjectile()
{
    PrimaryActorTick.bCanEverTick = true;

    m_MainCollision = CreateDefaultSubobject<USphereComponent>( TEXT( "Main Collision" ) );
    RootComponent   = m_MainCollision;

    m_HitboxHandler = CreateDefaultSubobject<UHitboxHandlerComponent>( TEXT( "Hitbox Handler" ) );
}

void AProjectile::Init( TObjectPtr<AActor> OwnerActor, FVector Location, float HorizontalDirectionMultiplier, float BaseSpeed, float Lifetime /* = -1.f*/ )
{
    m_Owner                         = OwnerActor;
    m_HorizontalDirectionMultiplier = HorizontalDirectionMultiplier;
    m_BaseSpeed                     = BaseSpeed;
    m_Lifetime                      = Lifetime;

    GetHitboxHandlerComponent()->m_AdditionalActorsToIgnore.Emplace( m_Owner );

    TeleportTo( Location, FRotator::ZeroRotator );

    if( m_Lifetime > 0.f )
    {
        GetWorldTimerManager().SetTimer( m_LifetimeTimerHandle, this, &AProjectile::OnLifetimeTimerEnded, m_Lifetime );
    }

    GetHitboxHandlerComponent()->m_HitDelegate.AddUObject( this, &AProjectile::OnHitLanded );
    GetHitboxHandlerComponent()->SpawnDefaultHitboxes();
}

bool AProjectile::IsFacingRight()
{
    return m_HorizontalDirectionMultiplier > 0.f;
}

void AProjectile::SetFacingRight( bool /*Right*/, bool /*Instant*/ )
{
}

FVector AProjectile::GetLocation()
{
    return GetActorLocation();
}

void AProjectile::Tick( float DeltaTime )
{
    Super::Tick( DeltaTime );

    FVector currentLocation = GetActorLocation();
    currentLocation.Y += (m_HorizontalDirectionMultiplier * m_BaseSpeed * DeltaTime);

    SetActorLocation( currentLocation );

    if( loc_ProjectileDebugFacing )
    {
        FG_TEXT( GetWorld(), GetActorLocation(), FString::Printf( TEXT( "[Facing Right: %s]" ), IsFacingRight() ? TEXT( "TRUE" ) : TEXT( "FALSE" ) ) );
    }
}

void AProjectile::OnLifetimeTimerEnded()
{
    m_DestroyRequestedDelegate.Broadcast( this );
}

void AProjectile::OnHitLanded( TObjectPtr<AActor> /*Target*/, const HitData& /*HitData*/ )
{
    m_DestroyRequestedDelegate.Broadcast( this );
}
