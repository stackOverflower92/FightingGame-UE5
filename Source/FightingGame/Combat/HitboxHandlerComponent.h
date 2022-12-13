// Copyright (c) Giammarco Agazzotti

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FightingGame/Combat/HitData.h"
#include "HitboxHandlerComponent.generated.h"

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
	virtual void BeginPlay() override;

public:
	FHit m_HitDelegate;

	void AddHitbox( HitData HitData );
	void RemoveHitbox( uint32 HitUniqueId );

	void ShowDebugTraces( bool Show );

	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

private:
	TMap<uint32, TArray<FHitGroupPair>> m_ActorGroupsMap;

	TArray<HitData> m_ActiveHitboxes;
	bool m_DebugTraces = true;

	bool TraceHitbox( const HitData& HitData, FHitResult& OutHit );
	bool WasActorAlreadyHit( AActor* Actor, uint32 HitboxId );
	void RegisterHitActor( AActor* Actor, uint32 HitboxId );
	void UpdateHitbox( const HitData& HitData );

	void RemovePendingHitboxes();
};
