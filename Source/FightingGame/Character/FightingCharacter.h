#pragma once

#include "CoreMinimal.h"
#include "FightingGame/Combat/FacingEntity.h"
#include "FightingGame/Combat/GroundSensitiveEntity.h"
#include "FightingGame/Combat/Hittable.h"
#include "GameFramework/Character.h"
#include "FightingCharacter.generated.h"

class UProjectileSpawnerComponent;
class UBoxComponent;
class UHitStopComponent;
class UFSM;
class UMovesBufferComponent;
class UHitboxHandlerComponent;

DECLARE_MULTICAST_DELEGATE( FFacingChanged )
DECLARE_MULTICAST_DELEGATE_OneParam( FHitLanded, AActor* )
DECLARE_MULTICAST_DELEGATE( FGrounded )
DECLARE_MULTICAST_DELEGATE( FAirborne )

UCLASS()
class FIGHTINGGAME_API AFightingCharacter : public ACharacter,
                                            public IHittable, public IFacingEntity, public IGroundSensitiveEntity
{
	GENERATED_BODY()

public:
	AFightingCharacter();

	FFacingChanged m_FacingChangedDelegate;
	FHitLanded m_HitLandedDelegate;
	FGrounded m_GroundedDelegate;
	FAirborne m_AirborneDelegate;
	int m_PlayerIndex = 0;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, DisplayName = "Current Horizontal Movement" )
	float m_CurrentHorizontalMovement = 0.f;

	UFUNCTION( BlueprintCallable )
	virtual bool IsAirborne() const override;

	UFUNCTION( BlueprintCallable )
	virtual bool IsGrounded() const override;

	UFUNCTION( BlueprintCallable )
	void UpdateHorizontalMovement( float value );

	UFUNCTION( BlueprintCallable )
	virtual bool IsFacingRight() override;

	UFUNCTION( BlueprintCallable )
	virtual void SetFacingRight( bool Right, bool Instant ) override;

	virtual FVector GetLocation() override;

	UFUNCTION( BlueprintCallable )
	UHitboxHandlerComponent* GetHitboxHandler() const { return m_HitboxHandler; }

	UFUNCTION( BlueprintCallable )
	float GetKnockbackMultiplier() const;

	UFUNCTION( BlueprintCallable )
	float GetDamagePercent() const;

	UFUNCTION( BlueprintCallable )
	void SetDamagePercent( float Percent );

	UFUNCTION( BlueprintCallable )
	bool IsAirKnockbackHappening() const;

	UFUNCTION( BlueprintCallable )
	void SetAirKnockbackHappening( bool Value );

	UFUNCTION( BlueprintCallable )
	void PushTimeDilation( float Value );

	UFUNCTION( BlueprintCallable )
	void PopTimeDilation();

	virtual void Tick( float DeltaTime ) override;
	virtual void SetupPlayerInputComponent( class UInputComponent* PlayerInputComponent ) override;

	virtual void OnHitReceived( const HitData& HitData ) override;
	virtual bool IsHittable() override;

	FORCEINLINE TObjectPtr<UMovesBufferComponent> GetMovesBufferComponent() const { return m_MovesBuffer; }
	FORCEINLINE TObjectPtr<UFSM> GetFSM() const { return m_FSM; }
	FORCEINLINE TObjectPtr<UHitStopComponent> GetHitStopComponent() const { return m_HitStopComponent; }
	FORCEINLINE TObjectPtr<UProjectileSpawnerComponent> GetProjectileSpawnerComponent() const { return m_ProjectileSpawnerComponent; }

	FORCEINLINE bool HasJustLandedHit() const { return m_HasJustLandedHit; }
	FORCEINLINE void ResetHasJustLandedHit() { m_HasJustLandedHit = false; }

	FORCEINLINE void SetHittable( bool Hittable ) { m_Hittable = Hittable; }
	FORCEINLINE bool IsHittable() const { return m_Hittable; }

	void UpdateMeshShake();
	void ResetMeshRelativeLocation();

protected:
	UPROPERTY( EditAnywhere, BlueprintReadWrite, DisplayName = "FSM" )
	TObjectPtr<UFSM> m_FSM = nullptr;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, DisplayName = "Moves Buffer" )
	TObjectPtr<UMovesBufferComponent> m_MovesBuffer = nullptr;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, DisplayName = "Hitbox Handler" )
	TObjectPtr<UHitboxHandlerComponent> m_HitboxHandler = nullptr;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, DisplayName = "Knockback Multiplier Curve" )
	TObjectPtr<UCurveFloat> m_KnockbackMultiplierCurve = nullptr;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, DisplayName = "Hit Stop Component" )
	TObjectPtr<UHitStopComponent> m_HitStopComponent = nullptr;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, DisplayName = "Projectile Spawner Component" )
	TObjectPtr<UProjectileSpawnerComponent> m_ProjectileSpawnerComponent = nullptr;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, DisplayName = "FSM First State" )
	FName m_FirstState = "IDLE";

	UPROPERTY( EditAnywhere, BlueprintReadWrite, DisplayName = "Update Facing When Airborne" )
	bool m_UpdateFacingWhenAirborne = false;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, DisplayName = "Facing Rotation Lerp Multiplier" )
	float m_FacingRotationLerpMultiplier = 1.f;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, DisplayName = "Grounded Reaction State Name" )
	FName m_GroundedReactionStateName = "REACTION_LIGHT_GROUNDED";

	UPROPERTY( EditAnywhere, BlueprintReadWrite, DisplayName = "Ground To Air Reaction State Name" )
	FName m_GroundToAirReactionStateName = "REACTION_LIGHT_AIRBORNE";

	UPROPERTY( EditAnywhere, BlueprintReadWrite, DisplayName = "Hit Landed State Duration" )
	float m_HitLandedStateDuration = .2f;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, DisplayName = "Hit Stun Time Dilation" )
	float m_HitStunTimeDilation = .001f;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, DisplayName = "Mesh Shake Frequency" )
	float m_MeshShakeFrequency = 2.5f;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, DisplayName = "Mesh Shake Amplitude" )
	float m_MeshShakeAmplitude = 60.f;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, DisplayName = "Push Box" )
	TObjectPtr<UBoxComponent> m_Pushbox = nullptr;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, DisplayName = "Push Box Shift Rate (Per Frame)" )
	float m_PushboxShiftRatePerFrame = 0.1f;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, DisplayName = "Use Velocity for Pushbox Force" )
	bool m_UseVelocityForPushboxForce = false;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, DisplayName = "Gravity Scale (Regular)" )
	float m_RegularGravityScale = 1.f;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, DisplayName = "Gravity Scale (Falling)" )
	float m_FallingGravityScale = 2.f;

	virtual void BeginPlay() override;
	virtual void EndPlay( const EEndPlayReason::Type EndPlayReason ) override;

private:
	bool m_FacingRight               = true;
	float m_TargetRotatorYaw         = 90.f;
	float m_DamagePercent            = 0.f;
	bool m_IsAirKnockbackHappening   = false;
	bool m_GroundedDelegateBroadcast = false;
	bool m_AirborneDelegateBroadcast = false;
	bool m_HasJustLandedHit          = false;
	FTimerHandle m_HitLandedStateTimerHandle;
	bool m_Hittable               = true;
	float m_CachedHitStopDuration = 0.f;
	bool m_CachedDoMeshShake      = false;
	bool m_CachedConsiderShake    = false;
	TArray<float> m_TimeDilations;
	bool m_CanUpdateMeshShake = false;
	FVector m_InitialMeshRelativeLocation;

	FDelegateHandle m_HitDelegateHandle;

	void UpdateYaw( float DeltaTime );
	void UpdateVerticalScale();

	void OnHitLanded( TObjectPtr<AActor> Target, const HitData& HitData );
	void StartHitLandedTimer();
	void OnHitLandedTimerEnded();

	void CheckGroundedEvent();
	void CheckAirborneEvent();

	void InitTimeDilations();
	void InitPushbox();
	void UpdatePushbox( float DeltaTime );

	void UpdateGravityScale();
};
