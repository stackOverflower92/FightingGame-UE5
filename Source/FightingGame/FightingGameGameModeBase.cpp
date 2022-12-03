// Copyright Epic Games, Inc. All Rights Reserved.

#include "FightingGameGameModeBase.h"

#include "Camera/CameraManager.h"
#include "Kismet/GameplayStatics.h"

void AFightingGameGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	if( auto* CameraManagerActor = UGameplayStatics::GetActorOfClass( GetWorld(), ACameraManager::StaticClass() ) )
	{
		m_CameraManager = Cast<ACameraManager>( CameraManagerActor );
	}
}
