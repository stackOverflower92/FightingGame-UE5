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
	if( m_Targets.Num() == 0 )
	{
		return GetActorLocation();
	}

	if( m_Targets.Num() == 1 )
	{
		const FVector MyLocation = GetActorLocation();
		const FVector TargetLocation = m_Targets[0]->GetActorLocation();

		return FVector( MyLocation.X, TargetLocation.Y, TargetLocation.Z );
	}

	TArray<FVector> AllLocations;
	for( const AActor* Target : m_Targets )
	{
		AllLocations.Add( Target->GetActorLocation() );
	}

	FVector Sum = AllLocations[0];
	for( int i = 1; i < AllLocations.Num(); ++i )
	{
		Sum += AllLocations[i];
	}

	Sum /= AllLocations.Num();

	return FVector( GetActorLocation().X, Sum.Y, Sum.Z );
}

void ACharactersSharedCamera::UpdateCameraPosition( float DeltaTime )
{
	FVector TargetPosition = GetCenterPosition() + m_PositionOffset;
	FVector LerpedLocation = GetActorLocation();

	LerpedLocation.X = FMath::Lerp( LerpedLocation.X, TargetPosition.X, m_MovementDamping.X * DeltaTime );
	LerpedLocation.Y = FMath::Lerp( LerpedLocation.Y, TargetPosition.Y, m_MovementDamping.Y * DeltaTime );
	LerpedLocation.Z = FMath::Lerp( LerpedLocation.Z, TargetPosition.Z, m_MovementDamping.Z * DeltaTime );

	SetActorLocation( LerpedLocation );
}
