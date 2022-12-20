// Copyright (c) Giammarco Agazzotti

#include "SphereVisualizer.h"

namespace
{
	constexpr float loc_ScaleToRadiusValue = 50.f;
}

ASphereVisualizer::ASphereVisualizer()
{
	PrimaryActorTick.bCanEverTick = true;

	m_Sphere      = CreateDefaultSubobject<UStaticMeshComponent>( TEXT( "Sphere" ) );
	RootComponent = m_Sphere;
}

void ASphereVisualizer::BeginPlay()
{
	Super::BeginPlay();
}

void ASphereVisualizer::SetRadius( float Radius )
{
	m_Radius = Radius;
	m_Sphere->SetWorldScale3D( (m_Radius / loc_ScaleToRadiusValue) * FVector::OneVector );
}

void ASphereVisualizer::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
}
