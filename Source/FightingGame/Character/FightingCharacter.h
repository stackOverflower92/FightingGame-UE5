#pragma once

#include "CoreMinimal.h"
#include "FightingGame/Combat/Hittable.h"
#include "GameFramework/Character.h"
#include "FightingCharacter.generated.h"

class UFSM;
class UMovesBufferComponent;
class UHitboxHandlerComponent;

DECLARE_MULTICAST_DELEGATE( FFacingChanged )
DECLARE_MULTICAST_DELEGATE_OneParam( FHitLanded, AActor* )
DECLARE_MULTICAST_DELEGATE( FGrounded )
DECLARE_MULTICAST_DELEGATE( FAirborne )

UENUM( BlueprintType )
enum class EFightingCharacterFacingTechnique : uint8
{
	ChangeRotation,
	MirrorSkeleton,
	MirrorScale,

	COUNT UMETA( Hidden ),
	INVALID UMETA( Hidden )
};

UCLASS()
class FIGHTINGGAME_API AFightingCharacter : public ACharacter, public IHittable
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
	bool IsAirborne() const;

	UFUNCTION( BlueprintCallable )
	void UpdateHorizontalMovement( float value );

	UFUNCTION( BlueprintCallable )
	bool IsFacingRight() const;

	UFUNCTION( BlueprintCallable )
	void SetFacingRight( bool Right, bool Instant = false );

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
	void EnableHitStun( bool Shake );

	UFUNCTION( BlueprintCallable )
	void DisableHitStun();

	virtual void Tick( float DeltaTime ) override;
	virtual void SetupPlayerInputComponent( class UInputComponent* PlayerInputComponent ) override;

	virtual void OnHitReceived( const HitData& HitData ) override;

	FORCEINLINE TObjectPtr<UMovesBufferComponent> GetMovesBufferComponent() const { return m_MovesBuffer; }
	FORCEINLINE TObjectPtr<UFSM> GetFSM() const { return m_FSM; }
	FORCEINLINE bool HasJustLandedHit() const { return m_HasLandedHit; }

	FORCEINLINE void SetHittable( bool Hittable ) { m_Hittable = Hittable; }
	FORCEINLINE bool IsHittable() const { return m_Hittable; }

protected:
	UPROPERTY( EditAnywhere, BlueprintReadWrite, DisplayName = "FSM" )
	TObjectPtr<UFSM> m_FSM = nullptr;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, DisplayName = "Moves Buffer" )
	TObjectPtr<UMovesBufferComponent> m_MovesBuffer = nullptr;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, DisplayName = "Hitbox Handler" )
	TObjectPtr<UHitboxHandlerComponent> m_HitboxHandler = nullptr;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, DisplayName = "Knockback Multiplier Curve" )
	TObjectPtr<UCurveFloat> m_KnockbackMultiplierCurve;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, DisplayName = "FSM First State" )
	FName m_FirstState = "IDLE";

	UPROPERTY( EditAnywhere, BlueprintReadWrite, DisplayName = "Update Facing When Airborne" )
	bool m_UpdateFacingWhenAirborne = false;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, DisplayName = "Facing Rotation Lerp Multiplier" )
	float m_FacingRotationLerpMultiplier = 1.f;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, DisplayName = "Facing Technique" )
	EFightingCharacterFacingTechnique m_FacingTechnique = EFightingCharacterFacingTechnique::MirrorScale;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, DisplayName = "Grounded Reaction State Name" )
	FName m_GroundedReactionStateName = "REACTION_LIGHT_GROUNDED";

	UPROPERTY( EditAnywhere, BlueprintReadWrite, DisplayName = "Ground To Air Reaction State Name" )
	FName m_GroundToAirReactionStateName = "REACTION_LIGHT_AIRBORNE";

	UPROPERTY( EditAnywhere, BlueprintReadWrite, DisplayName = "Hit Landed State Duration" )
	float m_HitLandedStateDuration = .2f;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, DisplayName = "Hit Stun Time Dilation" )
	float m_HitStunTimeDilation = .001f;

	virtual void BeginPlay() override;
	virtual void EndPlay( const EEndPlayReason::Type EndPlayReason ) override;

private:
	bool m_FacingRight = true;
	float m_TargetRotatorYaw = 90.f;
	float m_DamagePercent = 0.f;
	bool m_IsAirKnockbackHappening = false;
	bool m_GroundedDelegateBroadcast = false;
	bool m_AirborneDelegateBroadcast = false;
	bool m_HasLandedHit = false;
	FTimerHandle m_HitLandedStateTimerHandle;
	bool m_Hittable = true;
	FTimerHandle m_HitStunStopTimerHandle;
	FTimerHandle m_HitStunBeginTimerHandle;
	float m_CachedHitStunDuration = 0.f;
	TArray<float> m_TimeDilations;

	FDelegateHandle m_HitDelegateHandle;

	void UpdateYaw( float DeltaTime );
	void UpdateVerticalScale();

	void OnHitLanded( AActor* Target, const HitData& HitData );
	void StartHitLandedTimer();
	void OnHitLandedTimerEnded();

	void CheckGroundedEvent();
	void CheckAirborneEvent();

	void InitTimeDilations();
	void StartBeginHitStunTimer( const HitData& HitData );
	void OnHitStunBeginTimerEnded();

	void StartStopHitStunTimer();
	void OnHitStunStopTimerEnded();
};
