// Copyright (c) Giammarco Agazzotti

#pragma once

#include "CoreMinimal.h"
#include "StateBase.h"
#include "FightingGame/Character/FightingCharacter.h"
#include "FightingCharacterState.generated.h"

class UFightingCharacterStateTransition;
class UMoveDataAsset;
class AFightingCharacter;
class UFightingCharacterAnimInstance;
enum class EMontageEventType : uint8;

UCLASS()
class FIGHTINGGAME_API UFightingCharacterState : public UStateBase
{
	GENERATED_BODY()

public:
	virtual void Init_Implementation() override;
	virtual void Enter_Implementation() override;
	virtual void Exit_Implementation() override;
	virtual void Update_Implementation( float DeltaTime ) override;

protected:
	UPROPERTY( EditAnywhere, BlueprintReadOnly, DisplayName = "Owner Character" )
	TObjectPtr<AFightingCharacter> m_OwnerCharacter = nullptr;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, DisplayName = "Anim Instance" )
	TObjectPtr<UFightingCharacterAnimInstance> m_AnimInstance = nullptr;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, DisplayName = "Is Reaction" )
	bool m_IsReaction = false;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, DisplayName = "Move To Execute" )
	TObjectPtr<UMoveDataAsset> m_MoveToExecute = nullptr;

	// #TODO add AND/OR conditions for transitions? So that they can be combined to create more complex behaviours
	UPROPERTY( EditAnywhere, BlueprintReadOnly, DisplayName = "Transitions" )
	TMap<TObjectPtr<UFightingCharacterStateTransition>, FName> m_Transitions;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, DisplayName = "Update Movement" )
	bool m_UpdateMovement = false;

	UFUNCTION()
	void OnMontageEvent( UAnimMontage* Montage, EMontageEventType EventType );

	UFUNCTION( BlueprintNativeEvent )
	void OnMontageEnded( UAnimMontage* Montage );

	UFUNCTION( BlueprintNativeEvent )
	void OnCharacterHitLanded( AActor* Target );

	UFUNCTION( BlueprintNativeEvent )
	void OnCharacterGrounded();

private:
	FDelegateHandle m_CharacterHitLandedHandle;
	FDelegateHandle m_CharacterGroundedHandle;
};
