#pragma once

#include "CoreMinimal.h"
#include "FightingGame/Combat/Hittable.h"
#include "GameFramework/Character.h"
#include "FightingCharacter.generated.h"

class UFSM;
class UMovesBufferComponent;
class UHitboxHandlerComponent;

DECLARE_MULTICAST_DELEGATE( FFacingChanged )
DECLARE_MULTICAST_DELEGATE_OneParam( FHitLanded, AActor* );

UCLASS()
class FIGHTINGGAME_API AFightingCharacter : public ACharacter, public IHittable
{
	GENERATED_BODY()

public:
	AFightingCharacter();

	FFacingChanged m_FacingChangedDelegate;
	FHitLanded m_HitLandedDelegate;

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

	virtual void Tick( float DeltaTime ) override;
	virtual void SetupPlayerInputComponent( class UInputComponent* PlayerInputComponent ) override;

	virtual void OnHitReceived( const HitData& HitData ) override;

protected:
	UPROPERTY( EditAnywhere, BlueprintReadWrite, DisplayName = "FSM" )
	TObjectPtr<UFSM> m_FSM = nullptr;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, DisplayName = "Moves Buffer" )
	TObjectPtr<UMovesBufferComponent> m_MovesBuffer = nullptr;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, DisplayName = "Hitbox Handler" )
	TObjectPtr<UHitboxHandlerComponent> m_HitboxHandler = nullptr;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, DisplayName = "FSM First State" )
	FName m_FirstState = "IDLE";

	UPROPERTY( EditAnywhere, BlueprintReadWrite, DisplayName = "Update Facing When Airborne" )
	bool m_UpdateFacingWhenAirborne = false;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, DisplayName = "Facing Rotation Lerp Multiplier" )
	float m_FacingRotationLerpMultiplier = 1.f;

	virtual void BeginPlay() override;
	virtual void EndPlay( const EEndPlayReason::Type EndPlayReason ) override;

private:
	bool m_FacingRight = true;
	float m_TargetRotatorYaw = 90.f;
	FDelegateHandle m_HitDelegateHandle;

	void UpdateYaw( float DeltaTime );
	void OnHitLanded( AActor* Target );
};
