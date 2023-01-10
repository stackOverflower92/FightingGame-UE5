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
    UPROPERTY( BlueprintReadOnly, DisplayName = "Owner Character" )
    TObjectPtr<AFightingCharacter> m_OwnerCharacter = nullptr;

    UPROPERTY( BlueprintReadOnly, DisplayName = "Anim Instance" )
    TObjectPtr<UFightingCharacterAnimInstance> m_AnimInstance = nullptr;

    UPROPERTY( EditAnywhere, BlueprintReadOnly, DisplayName = "Is Reaction" )
    bool m_IsReaction = false;

    UPROPERTY( EditAnywhere, BlueprintReadOnly, DisplayName = "Move To Execute" )
    TObjectPtr<UMoveDataAsset> m_MoveToExecute = nullptr;

    UPROPERTY( EditAnywhere, BlueprintReadOnly, DisplayName = "Transitions" )
    TMap<FName, TSubclassOf<UFightingCharacterStateTransition>> m_Transitions;

    UPROPERTY( EditAnywhere, BlueprintReadOnly, DisplayName = "Update Movement" )
    bool m_UpdateMovement = false;

    UPROPERTY( EditAnywhere, BlueprintReadOnly, DisplayName = "Update Facing" )
    bool m_UpdateFacing = false;

    UPROPERTY( EditAnywhere, BlueprintReadOnly, DisplayName = "Pretend is Grounded" )
    bool m_PretendIsGrounded = false;

    UPROPERTY( EditAnywhere, BlueprintReadOnly, DisplayName = "Always Listen for Buffered Input Sequence" )
    bool m_AlwaysListenForBufferedInputSequence = false;

    UPROPERTY( EditAnywhere, BlueprintReadOnly, DisplayName = "Inputs Sequence Name To State Map" )
    TMap<FName, FName> m_InputsSequenceNameToStateMap;

    FName GetDesiredFSMStateFromInputsSequence( const FName& InputsSequenceName );

    UFUNCTION()
    void OnMontageEvent( UAnimMontage* Montage, EMontageEventType EventType );

    UFUNCTION( BlueprintNativeEvent )
    void OnMontageEnded( UAnimMontage* Montage );

    UFUNCTION( BlueprintNativeEvent )
    void OnCharacterHitLanded( AActor* Target );

    UFUNCTION( BlueprintNativeEvent )
    void OnCharacterGrounded();

    UFUNCTION( BlueprintNativeEvent )
    void OnCharacterAirborne();

private:
    UPROPERTY()
    TMap<FName, TObjectPtr<UFightingCharacterStateTransition>> m_InstancedTransitions;

    FDelegateHandle m_CharacterHitLandedHandle;
    FDelegateHandle m_CharacterGroundedHandle;
    FDelegateHandle m_CharacterAirborneHandle;

    bool TryExecuteBufferedInputsSequences();
};
