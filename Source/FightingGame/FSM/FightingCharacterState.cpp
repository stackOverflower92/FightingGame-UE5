// Copyright (c) Giammarco Agazzotti

#include "FightingCharacterState.h"

#include "FightingCharacterStateTransition.h"
#include "StateMachineComponent.h"
#include "FightingGame/Character/FightingCharacter.h"
#include "FightingGame/Animation/FightingCharacterAnimInstance.h"
#include "FightingGame/Common/CombatStatics.h"
#include "FightingGame/Debugging/Debug.h"
#include "FightingGame/Input/InputsSequenceStateMappingRow.h"
#include "FightingGame/Input/MovesBufferComponent.h"

void UFightingCharacterState::OnInit()
{
    Super::OnInit();

    m_OwnerCharacter = Cast<AFightingCharacter>( m_FSM->GetOwner() );
    ensure( m_OwnerCharacter );

    m_AnimInstance = Cast<UFightingCharacterAnimInstance>( m_OwnerCharacter->GetMesh()->GetAnimInstance() );
    ensure( m_AnimInstance );

    for( auto Pair : m_Transitions )
    {
        TObjectPtr<UFightingCharacterStateTransition> Instance = NewObject<UFightingCharacterStateTransition>(
            m_OwnerCharacter, Pair.Value, Pair.Value->GetFName(), RF_NoFlags, Pair.Value.GetDefaultObject(), true );

        Instance->OnInit( m_OwnerCharacter );
        m_InstancedTransitions.Emplace( Pair.Key, Instance );
    }
}

void UFightingCharacterState::OnEnter()
{
    Super::OnEnter();

    m_AnimInstance->m_MontageEvent.AddDynamic( this, &UFightingCharacterState::OnMontageEvent );

    m_CharacterHitLandedHandle = m_OwnerCharacter->m_HitLandedDelegate.AddUObject( this, &UFightingCharacterState::OnCharacterHitLanded );
    m_CharacterGroundedHandle  = m_OwnerCharacter->m_GroundedDelegate.AddUObject( this, &UFightingCharacterState::OnCharacterGrounded );
    m_CharacterAirborneHandle  = m_OwnerCharacter->m_AirborneDelegate.AddUObject( this, &UFightingCharacterState::OnCharacterAirborne );

    if( m_MoveToExecute )
    {
        UCombatStatics::ExecuteMove( m_OwnerCharacter, m_MoveToExecute );
    }

    if( m_IsReaction )
    {
        m_OwnerCharacter->m_IsReacting = true;

        // #TODO what happens with AI?
        if( m_OwnerCharacter->IsPlayerControlled() )
        {
            m_OwnerCharacter->DisableInput( Cast<APlayerController>( m_OwnerCharacter->GetController() ) );
        }
    }

    m_OwnerCharacter->m_PretendIsGrounded = m_PretendIsGrounded;

    for( auto Pair : m_InstancedTransitions )
    {
        Pair.Value->OnStateEnter();
    }
}

void UFightingCharacterState::OnExit()
{
    Super::OnExit();

    if( m_AnimInstance )
    {
        m_AnimInstance->m_MontageEvent.RemoveDynamic( this, &UFightingCharacterState::OnMontageEvent );
    }

    m_OwnerCharacter->m_HitLandedDelegate.Remove( m_CharacterHitLandedHandle );
    m_OwnerCharacter->m_GroundedDelegate.Remove( m_CharacterGroundedHandle );
    m_OwnerCharacter->m_AirborneDelegate.Remove( m_CharacterAirborneHandle );

    if( m_IsReaction )
    {
        m_OwnerCharacter->m_IsReacting = false;

        // #TODO what happens with AI?
        if( m_OwnerCharacter->IsPlayerControlled() )
        {
            m_OwnerCharacter->EnableInput( Cast<APlayerController>( m_OwnerCharacter->GetController() ) );
        }
    }

    m_OwnerCharacter->m_PretendIsGrounded = false;

    for( auto Pair : m_InstancedTransitions )
    {
        Pair.Value->OnStateExit();
    }
}

void UFightingCharacterState::OnTick( float DeltaTime )
{
    Super::OnTick( DeltaTime );

    if( !m_IsReaction && (m_OwnerCharacter->HasJustLandedHit() || m_AlwaysListenForBufferedInputSequence) )
    {
        if( EvaluateInputsSequenceBufferedTransition() )
        {
            // #TODO is this correct? can this transition have the ownership of that value?
            m_OwnerCharacter->ResetHasJustLandedHit();
            return;
        }
    }

    for( auto pair : m_InstancedTransitions )
    {
        if( pair.Value->CanPerformTransition() )
        {
            m_FSM->SetState( pair.Key );
        }
    }

    if( m_UpdateMovement )
    {
        m_OwnerCharacter->UpdateHorizontalMovement( m_OwnerCharacter->GetMovesBufferComponent()->m_InputMovement );
    }

    if( m_UpdateFacing )
    {
        m_OwnerCharacter->UpdateFacing();
    }
}

bool UFightingCharacterState::ThisStateOverridesInputsSequenceMapping( const FString& InputsSequenceName ) const
{
    return m_InputsSequenceNameToStateMap.Contains( InputsSequenceName );
}

FName UFightingCharacterState::GetFSMStateFromInputsSequence( const FString& InputsSequenceName )
{
    if( ThisStateOverridesInputsSequenceMapping( InputsSequenceName ) )
    {
        return m_InputsSequenceNameToStateMap[InputsSequenceName];
    }

    if( auto* row = GetStateMappingRowFromInputsSequence( InputsSequenceName ) )
    {
        return row->m_StateName;
    }

    FG_SLOG_ERR( FString::Printf(TEXT("Could not find input state mapping row for entry [%s]"), *InputsSequenceName) );

    return NAME_None;
}

FInputsSequenceStateMappingRow* UFightingCharacterState::GetStateMappingRowFromInputsSequence( const FString& InputsSequenceName )
{
    if( TObjectPtr<UDataTable> table = m_OwnerCharacter->GetMovesBufferComponent()->m_InputsToStatesDataTable )
    {
        for( auto name : table->GetRowNames() )
        {
            auto* row = table->FindRow<FInputsSequenceStateMappingRow>( name, TEXT( "" ) );
            verify( row );

            if( row->m_InputsSequence->m_Name == InputsSequenceName )
            {
                return row;
            }
        }
    }

    return nullptr;
}

void UFightingCharacterState::OnMontageEvent( UAnimMontage* Montage, EMontageEventType EventType )
{
    if( !m_IsReaction && m_MoveToExecute && EventType == EMontageEventType::Ended )
    {
        if( EvaluateInputsSequenceBufferedTransition() )
        {
            return;
        }
    }

    for( auto Pair : m_InstancedTransitions )
    {
        Pair.Value->OnMontageEvent( Montage, EventType );
    }

    // #TODO handle other cases
    switch( EventType )
    {
        case EMontageEventType::Ended:
            {
                OnMontageEnded( Montage );
                break;
            }
    }
}

bool UFightingCharacterState::EvaluateInputsSequenceBufferedTransition()
{
    TArray<FInputsSequenceBufferEntry> inputsSequenceSnapshot;
    m_OwnerCharacter->GetMovesBufferComponent()->GetInputsSequenceBufferSnapshot( inputsSequenceSnapshot, true, true );

    for( int32 i = inputsSequenceSnapshot.Num() - 1; i >= 0; --i )
    {
        if( ThisStateOverridesInputsSequenceMapping( inputsSequenceSnapshot[i].m_InputsSequenceName ) )
        {
            continue;
        }

        auto mappingRow = GetStateMappingRowFromInputsSequence( inputsSequenceSnapshot[i].m_InputsSequenceName );

        bool groundedContitionFailed = m_OwnerCharacter->IsGrounded() && !mappingRow->m_AllowWhenGrounded;
        bool airborneConditionFailed = m_OwnerCharacter->IsAirborne() && !mappingRow->m_AllowWhenAirborne;

        if( groundedContitionFailed || airborneConditionFailed )
        {
            inputsSequenceSnapshot.RemoveAt( i );
        }
    }

    if( !inputsSequenceSnapshot.IsEmpty() )
    {
        inputsSequenceSnapshot.Sort( []( const FInputsSequenceBufferEntry& A, const FInputsSequenceBufferEntry& B )
        {
            return A.m_Priority < B.m_Priority;
        } );

        FInputsSequenceBufferEntry& targetEntry = inputsSequenceSnapshot[0];

        FString selectedInputsSequence = targetEntry.m_InputsSequenceName;
        FName targetState              = GetFSMStateFromInputsSequence( selectedInputsSequence );

        if( !targetState.IsNone() )
        {
            m_OwnerCharacter->GetMovesBufferComponent()->UseBufferedInputsSequence( targetEntry );

            if( targetState.IsValid() )
            {
                m_FSM->SetState( targetState );
                return true;
            }

            FG_SLOG_ERR( TEXT("targetState is not valid.") );
        }
    }

    return false;
}

void UFightingCharacterState::OnCharacterAirborne_Implementation()
{
    for( auto Pair : m_InstancedTransitions )
    {
        Pair.Value->OnAirborne();
    }
}

void UFightingCharacterState::OnCharacterGrounded_Implementation()
{
    for( auto Pair : m_InstancedTransitions )
    {
        Pair.Value->OnGrounded();
    }
}

void UFightingCharacterState::OnCharacterHitLanded_Implementation( AActor* Target )
{
}

void UFightingCharacterState::OnMontageEnded_Implementation( UAnimMontage* Montage )
{
}
