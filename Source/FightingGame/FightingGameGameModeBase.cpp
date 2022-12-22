// Copyright Epic Games, Inc. All Rights Reserved.

#include "FightingGameGameModeBase.h"

#include "Camera/CameraManager.h"
#include "Common/GameFramework.h"
#include "Kismet/GameplayStatics.h"

void AFightingGameGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	// #TODO spawn instead of getting
	if( auto* CameraManagerActor = UGameplayStatics::GetActorOfClass( GetWorld(), ACameraManager::StaticClass() ) )
	{
		m_CameraManager = Cast<ACameraManager>( CameraManagerActor );
	}

	m_GameFrameworkInstance = GetWorld()->SpawnActor<AGameFramework>();
	ensureMsgf( m_GameFrameworkInstance, TEXT("Could not spawn game framework") );

	m_GameFrameworkInstance->Init();
}

void AFightingGameGameModeBase::InitCameraManager()
{
	if( m_CameraManager )
	{
		m_CameraManager->Init();
	}
}
