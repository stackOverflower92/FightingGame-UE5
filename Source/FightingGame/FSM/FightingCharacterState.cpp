// Copyright (c) Giammarco Agazzotti

#include "FightingCharacterState.h"

#include "FightingCharacterStateTransition.h"
#include "FightingGame/Character/FightingCharacter.h"
#include "FightingGame/Animation/FightingCharacterAnimInstance.h"
#include "FightingGame/Common/CombatStatics.h"
#include "FightingGame/Common/FSMStatics.h"
#include "FightingGame/Input/MovesBufferComponent.h"

void UFightingCharacterState::Init_Implementation()
{
    Super::Init_Implementation();

    m_OwnerCharacter = Cast<AFightingCharacter>( ActorOwner );
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

void UFightingCharacterState::Enter_Implementation()
{
    Super::Enter_Implementation();

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

void UFightingCharacterState::Exit_Implementation()
{
    Super::Exit_Implementation();

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

void UFightingCharacterState::Update_Implementation( float DeltaTime )
{
    Super::Update_Implementation( DeltaTime );

    if( m_OwnerCharacter->HasJustLandedHit() || m_AlwaysListenForBufferedInputSequence )
    {
        if( TryExecuteBufferedInputsSequences() )
        {
            return;
        }
    }

    for( auto Pair : m_InstancedTransitions )
    {
        if( Pair.Value->CanPerformTransition() )
        {
            UFSMStatics::SetState( FSMOwner, Pair.Key );
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

FName UFightingCharacterState::GetDesiredFSMStateFromInputsSequence( const FName& InputsSequenceName )
{
    if( m_InputsSequenceNameToStateMap.Contains( InputsSequenceName ) )
    {
        return m_InputsSequenceNameToStateMap[InputsSequenceName];
    }

    return FName( NAME_None );
}

void UFightingCharacterState::OnMontageEvent( UAnimMontage* Montage, EMontageEventType EventType )
{
    if( TryExecuteBufferedInputsSequences() )
    {
        return;
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

bool UFightingCharacterState::TryExecuteBufferedInputsSequences()
{
    // #TODO this may become a flag on the exposed properties of this state?
    bool canExecuteBufferedMoves = m_MoveToExecute != nullptr;
    if( canExecuteBufferedMoves )
    {
        TArray<FInputsSequenceBufferEntry> inputsSequenceSnapshot;
        m_OwnerCharacter->GetMovesBufferComponent()->GetInputsSequenceBufferSnapshot( inputsSequenceSnapshot, true );

        if( !inputsSequenceSnapshot.IsEmpty() )
        {
            inputsSequenceSnapshot.Sort( []( const FInputsSequenceBufferEntry& A, const FInputsSequenceBufferEntry& B )
            {
                return A.m_Priority < B.m_Priority;
            } );

            FName selectedInputsSequence = inputsSequenceSnapshot[0].m_InputsSequenceName;
            FName targetState            = GetDesiredFSMStateFromInputsSequence( selectedInputsSequence );

            if( !targetState.IsNone() )
            {
                // #TODO this does weird things!
                m_OwnerCharacter->GetMovesBufferComponent()->InitInputsSequenceBuffer();

                UFSMStatics::SetState( m_OwnerCharacter->GetFSM(), targetState );

                return true;
            }
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
