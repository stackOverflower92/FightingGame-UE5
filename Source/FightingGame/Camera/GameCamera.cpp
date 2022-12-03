// Copyright (c) Giammarco Agazzotti


#include "GameCamera.h"


AGameCamera::AGameCamera()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AGameCamera::BeginPlay()
{
	Super::BeginPlay();
}

void AGameCamera::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
}

void AGameCamera::OnViewSet()
{
}

FString AGameCamera::GetId() const
{
	return m_Id;
}
