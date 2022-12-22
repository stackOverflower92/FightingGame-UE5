// Copyright (c) Giammarco Agazzotti


#include "GameFramework.h"

#include "Manager.h"
#include "FightingGame/Debugging/Debug.h"

AGameFramework::AGameFramework()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AGameFramework::Init()
{
	for( auto& tuple : m_Managers )
	{
		// #TODO find a way to set name for this actor (tuple.Key)
		TObjectPtr<AManager> manager = Cast<AManager>( GetWorld()->SpawnActor( tuple.Value ) );
		manager->OnRegister( *this );

		m_ManagersInstances.Emplace( manager );
	}
}

void AGameFramework::ShutDown()
{
	for( TObjectPtr<AManager> manager : m_ManagersInstances )
	{
		manager->OnDeregister( *this );
	}

	for( int i = m_ManagersInstances.Num(); i >= 0; --i )
	{
		if( !GetWorld()->DestroyActor( m_ManagersInstances[i] ) )
		{
			FG_SLOG_ERR( FString::Printf(TEXT("Could not destroy manager [%s]"), *m_ManagersInstances[i]->GetName()) );
		}

		m_ManagersInstances.RemoveAt( i );
	}
}
