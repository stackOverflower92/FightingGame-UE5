// Copyright (c) Giammarco Agazzotti

#pragma once

#include "CoreMinimal.h"
#include "FightingGame/Combat/FacingEntity.h"
#include "FightingGame/Combat/Hittable.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

class UHitboxHandlerComponent;
class USphereComponent;

DECLARE_MULTICAST_DELEGATE_OneParam( FDestroyRequested, TObjectPtr<AProjectile> )

UCLASS()
class FIGHTINGGAME_API AProjectile : public AActor, public IHittable, public IFacingEntity
{
	GENERATED_BODY()

public:
	AProjectile();

protected:
	virtual void BeginPlay() override;

public:
	FDestroyRequested m_DestroyRequestedDelegate;

	void Init( TObjectPtr<AActor> OwnerActor, FVector Location, float HorizontalDirectionMultiplier, float BaseSpeed, float Lifetime = -1.f );

	virtual void OnHitReceived( const HitData& HitData ) override;

	virtual bool IsFacingRight() override;
	virtual void SetFacingRight( bool Right, bool Instant ) override;
	virtual FVector GetLocation() override;

	FORCEINLINE TObjectPtr<UHitboxHandlerComponent> GetHitboxHandlerComponent() const { return m_HitboxHandler; }

	virtual void Tick( float DeltaTime ) override;

protected:
	UPROPERTY( EditAnywhere, BlueprintReadWrite, DisplayName = "Main Collision" )
	TObjectPtr<USphereComponent> m_MainCollision = nullptr;

	UPROPERTY()
	TObjectPtr<AActor> m_Owner = nullptr;

	/*
	 * -1: left, 1: right
	 */
	UPROPERTY( EditAnywhere, BlueprintReadOnly, DisplayName = "Horizontal Direction Multiplier" )
	float m_HorizontalDirectionMultiplier = 0.f;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, DisplayName = "Lifetime" )
	float m_Lifetime = -1.f;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, DisplayName = "Base Speed" )
	float m_BaseSpeed = 10.f;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, DisplayName = "Horizontal Speed Multiplier Curve" )
	TObjectPtr<UCurveFloat> m_HorizontalSpeedMultiplierCurve = nullptr;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, DisplayName = "Hitbox Handler" )
	TObjectPtr<UHitboxHandlerComponent> m_HitboxHandler = nullptr;

private:
	FTimerHandle m_LifetimeTimerHandle;
	void OnLifetimeTimerEnded();

	void OnHitLanded( AActor* Target, const HitData& HitData );
};
