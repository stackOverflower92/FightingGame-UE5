// Copyright (c) Giammarco Agazzotti

#include "CharactersSharedCamera.h"

#include "FightingGame/Character/FightingCharacter.h"
#include "Kismet/GameplayStatics.h"

ACharactersSharedCamera::ACharactersSharedCamera()
{
    PrimaryActorTick.bCanEverTick = true;
}

void ACharactersSharedCamera::BeginPlay()
{
    Super::BeginPlay();

    if( m_AutoAddTargetsOnBeginPlay )
    {
        AddAvailableTargets();
    }
}

void ACharactersSharedCamera::Tick( float DeltaTime )
{
    Super::Tick( DeltaTime );

    UpdateCameraPosition( DeltaTime );
}

void ACharactersSharedCamera::OnViewSet()
{
    Super::OnViewSet();

    AddAvailableTargets();
}

bool ACharactersSharedCamera::AddTarget( AActor* Target )
{
    if( m_Targets.Contains( Target ) )
    {
        return false;
    }

    m_Targets.Add( Target );
    return true;
}

bool ACharactersSharedCamera::RemoveTarget( AActor* Target )
{
    if( !m_Targets.Contains( Target ) )
    {
        return false;
    }

    m_Targets.Remove( Target );
    return true;
}

void ACharactersSharedCamera::AddAvailableTargets()
{
    UGameplayStatics::GetAllActorsOfClass( GetWorld(), AFightingCharacter::StaticClass(), m_Targets );
}

FVector ACharactersSharedCamera::GetCenterPosition() const
{
    return UGameplayStatics::GetActorArrayAverageLocation( m_Targets );
}

void ACharactersSharedCamera::UpdateCameraPosition( float DeltaTime )
{
    FVector TargetPosition = GetCenterPosition() + m_PositionOffset;
    FVector LerpedLocation = GetActorLocation();

    float maxDist     = GetMaxActorsDistance();
    float clampedDist = FMath::Max( maxDist, m_MinZoomDistance );

    LerpedLocation.X = FMath::Lerp( LerpedLocation.X, -clampedDist, m_MovementDamping.X * DeltaTime );
    LerpedLocation.Y = FMath::Lerp( LerpedLocation.Y, TargetPosition.Y, m_MovementDamping.Y * DeltaTime );
    LerpedLocation.Z = FMath::Lerp( LerpedLocation.Z, TargetPosition.Z, m_MovementDamping.Z * DeltaTime );

    SetActorLocation( LerpedLocation );
}

float ACharactersSharedCamera::GetMaxActorsDistance() const
{
    float maxDistance = 0.f;

    for( int32 i = 1; i < m_Targets.Num(); ++i )
    {
        for( int32 j = 0; j < m_Targets.Num(); ++j )
        {
            AActor* A = m_Targets[i];
            AActor* B = m_Targets[j];

            float currentDistance = FVector::Distance( A->GetActorLocation(), B->GetActorLocation() );
            if( currentDistance > maxDistance )
            {
                maxDistance = currentDistance;
            }
        }
    }

    return maxDistance;
}
