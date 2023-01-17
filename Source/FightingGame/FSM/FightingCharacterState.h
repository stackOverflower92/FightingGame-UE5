// Copyright (c) Giammarco Agazzotti

#pragma once

#include "CoreMinimal.h"
#include "StateMachineState.h"
#include "FightingGame/Character/FightingCharacter.h"
#include "FightingGame/Input/InputsSequenceStateMappingRow.h"
#include "FightingCharacterState.generated.h"

class UFightingCharacterStateTransition;
class UMoveDataAsset;
class AFightingCharacter;
class UFightingCharacterAnimInstance;

enum class EMontageEventType : uint8;

USTRUCT( BlueprintType )
struct FInputsSequenceOverrideData
{
    GENERATED_BODY()

    UPROPERTY( EditAnywhere, BlueprintReadOnly, DisplayName = "Target State" )
    FName m_TargetState;

    UPROPERTY( EditAnywhere, BlueprintReadOnly, DisplayName = "Allow As Cancel On Hit" )
    bool m_AllowAsOnHitCancel = false;
};

UCLASS()
class FIGHTINGGAME_API UFightingCharacterState : public UStateMachineState
{
    GENERATED_BODY()

public:
    virtual void OnInit() override;
    virtual void OnEnter() override;
    virtual void OnExit() override;
    virtual void OnTick( float DeltaTime ) override;

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

    UPROPERTY( EditAnywhere, BlueprintReadOnly, DisplayName = "Inputs Sequences Override" )
    TMap<FString, FInputsSequenceOverrideData> m_InputsSequencesOverrides;

    UPROPERTY( EditAnywhere, BlueprintReadOnly, DisplayName = "Inputs Sequences Blocks" )
    TArray<FString> m_BlockedInputsSequences;

    UPROPERTY( EditAnywhere, BlueprintReadOnly, DisplayName = "Blocks All Inputs Sequences" )
    bool m_BlocksAllInputsSequences = false;

    bool ThisStateOverridesInputsSequenceMapping( const FString& InputsSequenceName ) const;

    FName GetFSMStateFromInputsSequence( const FString& InputsSequenceName );

    FInputsSequenceStateMappingRow* GetStateMappingRowFromInputsSequence( const FString& InputsSequenceName );

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

    bool EvaluateInputsSequenceBufferedTransition( bool WasUsedDuringHit );
};
