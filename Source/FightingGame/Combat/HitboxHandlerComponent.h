// Copyright (c) Giammarco Agazzotti

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FightingGame/Combat/HitData.h"
#include "HitboxHandlerComponent.generated.h"

UCLASS( ClassGroup = ( Custom ), meta = ( BlueprintSpawnableComponent ) )
class FIGHTINGGAME_API UHitboxHandlerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UHitboxHandlerComponent();

protected:
	virtual void BeginPlay() override;

public:
	void AddHitbox( FHitData HitData );
	void RemoveHitbox( uint32 HitUniqueId );

	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

private:
	TMap<uint32, TArray<uint32>> m_HitActorsMap;
	TArray<FHitData> m_ActiveHitboxes;

	bool TraceHitbox( const FHitData& HitData, FHitResult& OutHit );
	bool WasActorAlreadyHit( AActor* Actor, uint32 HitboxId ) const;
	void RegisterHitActor( AActor* Actor, uint32 HitboxId );
	void UpdateHitbox( const FHitData& HitData );
	void RemovePendingHitboxes();
};
