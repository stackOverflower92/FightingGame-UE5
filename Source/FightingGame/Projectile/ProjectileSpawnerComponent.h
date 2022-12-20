// Copyright (c) Giammarco Agazzotti

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ProjectileSpawnerComponent.generated.h"

class AProjectile;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FIGHTINGGAME_API UProjectileSpawnerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UProjectileSpawnerComponent();

protected:
	virtual void BeginPlay() override;

public:
	void SpawnProjectile( TSubclassOf<AProjectile> ProjectileClass, FVector SpawnLocation, float HorizontalDirectionMultiplier, float BaseSpeed, float Lifetime );

	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

private:
	TArray<TObjectPtr<AProjectile>> m_AliveProjectiles;

	void OnProjectileDestroyRequested( TObjectPtr<AProjectile> Projectile );
};
