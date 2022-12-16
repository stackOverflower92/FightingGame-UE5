// Copyright (c) Giammarco Agazzotti

#pragma once

#include "CoreMinimal.h"
#include "FightingGame/Combat/Hittable.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

class USphereComponent;

DECLARE_MULTICAST_DELEGATE_OneParam( FDestroyRequested, TObjectPtr<AProjectile> )

UCLASS()
class FIGHTINGGAME_API AProjectile : public AActor, public IHittable
{
	GENERATED_BODY()

public:
	AProjectile();

protected:
	virtual void BeginPlay() override;

public:
	FDestroyRequested m_DestroyRequestedDelegate;

	void Init( TObjectPtr<AActor> OwnerActor, FVector Location, float HorizontalDirectionMultiplier, float Lifetime = -1.f );

#pragma region IHITTABLE
	virtual void OnHitReceived( const HitData& HitData ) override;
#pragma endregion

	virtual void Tick( float DeltaTime ) override;

protected:
	UPROPERTY()
	TObjectPtr<USphereComponent> m_MainCollision = nullptr;

	UPROPERTY()
	TObjectPtr<AActor> m_Owner = nullptr;

	float m_HorizontalDirectionMultiplier = 0.f;
	float m_Lifetime                      = -1.f;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, DisplayName = "Base Speed" )
	float m_BaseSpeed = 10.f;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, DisplayName = "Horizontal Speed Multiplier Curve" )
	TObjectPtr<UCurveFloat> m_HorizontalSpeedMultiplierCurve = nullptr;

private:
	FTimerHandle m_LifetimeTimerHandle;
	void OnLifetimeTimerEnded();
};
