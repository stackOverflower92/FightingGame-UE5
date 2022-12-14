// Copyright (c) Giammarco Agazzotti

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FightingGame/Combat/HitData.h"
#include "HitboxHandlerComponent.generated.h"

class ASphereVisualizer;
DECLARE_MULTICAST_DELEGATE_TwoParams( FHit, AActor*, const HitData& )

struct FHitGroupPair
{
	uint32 m_Id   = -1;
	int m_GroupId = -1;

	friend bool operator==( const FHitGroupPair& Lhs, const FHitGroupPair& Rhs )
	{
		return Lhs.m_Id == Rhs.m_Id && Lhs.m_GroupId == Rhs.m_GroupId;
	}

	friend bool operator!=( const FHitGroupPair& Lhs, const FHitGroupPair& Rhs )
	{
		return !(Lhs == Rhs);
	}
};

UCLASS( ClassGroup = ( Custom ), meta = ( BlueprintSpawnableComponent ) )
class FIGHTINGGAME_API UHitboxHandlerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UHitboxHandlerComponent();

protected:
	UPROPERTY( EditAnywhere, BlueprintReadWrite, DisplayName = "Hitbox Visualizer" )
	TSubclassOf<ASphereVisualizer> m_HitboxVisualizer = nullptr;

	UPROPERTY()
	TObjectPtr<USceneComponent> m_ReferenceComponent = nullptr;

	virtual void BeginPlay() override;

public:
	FHit m_HitDelegate;

	void SetReferenceComponent( TObjectPtr<USceneComponent> Component );

	void AddHitbox( HitData Hit );
	void RemoveHitbox( uint32 HitUniqueId );

	void ShowDebugTraces( bool Show );

	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

private:
	TMap<uint32, TArray<FHitGroupPair>> m_ActorGroupsMap;

	TArray<HitData> m_ActiveHitboxes;
	TArray<TObjectPtr<ASphereVisualizer>> m_HitboxVisualizers;

	bool m_DebugTraces = true;

	bool TraceHitbox( const HitData& HitData, FHitResult& OutHit );
	bool WasActorAlreadyHit( AActor* Actor, uint32 HitboxId );
	void RegisterHitActor( AActor* Actor, uint32 HitboxId );
	void UpdateHitbox( const HitData& HitData );

	void RemovePendingHitboxes();

	void DEBUG_SpawnDebugSphere( const HitData& Hit );
	void DEBUG_DestroyDebugSphere( int HitboxId );
};
