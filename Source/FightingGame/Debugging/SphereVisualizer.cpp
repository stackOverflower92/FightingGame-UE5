// Copyright (c) Giammarco Agazzotti

#include "SphereVisualizer.h"

ASphereVisualizer::ASphereVisualizer()
{
	PrimaryActorTick.bCanEverTick = true;

	m_Sphere = CreateDefaultSubobject<UStaticMeshComponent>( TEXT( "Sphere" ) );
}

void ASphereVisualizer::BeginPlay()
{
	Super::BeginPlay();
}

void ASphereVisualizer::SetRadius( float Radius )
{
	m_Radius = Radius;
	m_Sphere->SetWorldScale3D( (m_Radius / 50.f) * FVector::OneVector );
}

void ASphereVisualizer::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
}
