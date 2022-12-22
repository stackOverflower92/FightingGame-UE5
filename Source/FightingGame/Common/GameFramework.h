// Copyright (c) Giammarco Agazzotti

#pragma once

#include "CoreMinimal.h"
#include "Manager.h"
#include "FightingGame/Debugging/Debug.h"
#include "GameFramework/Actor.h"
#include "GameFramework.generated.h"

UCLASS()
class FIGHTINGGAME_API AGameFramework : public AActor
{
	GENERATED_BODY()

public:
	AGameFramework();

	void Init();
	void ShutDown();

	TObjectPtr<AManager> GetManager( UClass* Class ) const
	{
		ensureMsgf( Class, TEXT("Class is null") );

		auto* it = m_ManagersInstances.FindByPredicate( [&Class]( const TObjectPtr<AManager> _manager )
		{
			return _manager->IsA( Class );
		} );

		if( it )
		{
			return *it;
		}

		FG_SLOG_ERR( FString::Printf(TEXT("Cannot get manager of type [%s]: it does not exist"), *Class->GetName()) );
		return nullptr;
	}

protected:
	UPROPERTY( EditAnywhere, BlueprintReadWrite, DisplayName = "Managers" )
	TMap<FName, TSubclassOf<AManager>> m_Managers;

	TArray<TObjectPtr<AManager>> m_ManagersInstances;
};
