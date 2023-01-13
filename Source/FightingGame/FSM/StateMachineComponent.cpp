// Copyright (c) Giammarco Agazzotti

#include "StateMachineComponent.h"

#include "FightingGame/Debugging/Debug.h"

namespace
{
    int32 loc_DebugCurrentState = 0;
    FG_CVAR_FLAG_DESC( CVarDebugCurrentState, TEXT("StateMachine.DebugCurrentState"), loc_DebugCurrentState );

    int32 loc_DebugStatesHistory = 0;
    FG_CVAR_FLAG_DESC( CVarDebugStatesHistory, TEXT("StateMachine.DebugStatesHistory"), loc_DebugStatesHistory );
}

UStateMachineComponent::UStateMachineComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UStateMachineComponent::BeginPlay()
{
    Super::BeginPlay();

    if( m_StartOnBeginPlay )
    {
        Start();
    }
}

void UStateMachineComponent::Start()
{
    if( m_Started )
    {
        FG_SLOG_ERR( TEXT("State machine was already started") );
        return;
    }

    for( int i = 0; i < m_States.Num(); ++i )
    {
        TObjectPtr<UStateMachineState> inst = NewObject<UStateMachineState>( GetOwner(), m_States[i], m_States[i]->GetFName(), RF_NoFlags,
                                                                             m_States[i].GetDefaultObject(), true );

        inst->m_FSM = this;
        inst->OnInit();

        m_StatesInstances.Emplace( inst );
    }

    m_Started = true;

    if( TObjectPtr<UStateMachineState> firstState = GetStateById( m_FirstStateName ) )
    {
        // #TODO add overload to set state by state ptr
        SetState( firstState->m_Id );
    }
}

void UStateMachineComponent::SetState( const FName& StateId )
{
    m_CanTickCurrentState = false;

    if( m_CurrentState )
    {
        if( m_CurrentState->m_Id == StateId && !m_CurrentState->m_Repeatable )
        {
            FG_SLOG_ERR( FString::Printf(TEXT("You're trying to repeat state [%s] but it's not repeatable"), *StateId.ToString()) );

            m_CanTickCurrentState = true;
            return;
        }

        m_CurrentState->OnExit();
    }

    m_CurrentState = GetStateById( StateId );
    if( m_CurrentState )
    {
        m_CurrentState->OnEnter();
        m_CanTickCurrentState = true;

        if( m_StatesHistory.Num() > m_HistoryLength )
        {
            m_StatesHistory.RemoveAt( 0 );
        }

        m_StatesHistory.Emplace( m_CurrentState );
    }
    else
    {
        FG_SLOG_ERR( FString::Printf(TEXT("Cannot find state named [%s]"), *StateId.ToString()) );
    }
}

void UStateMachineComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
    Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

    if( m_CurrentState && m_CurrentState->IsValidLowLevel() && m_CanTickCurrentState )
    {
        m_CurrentState->OnTick( DeltaTime );
    }

    FString ownerName = GetOwner()->GetName();
    uint64 uniqueId64 = GetOwner()->GetUniqueID();
    if( loc_DebugCurrentState )
    {
        FG_SLOG_KEY( uniqueId64, FString::Printf(TEXT("(%s) STATE: %s"), *ownerName, *m_CurrentState->m_Id.ToString()), FColor::Orange );
    }

    if( loc_DebugStatesHistory )
    {
        static int32 startIndex = uniqueId64 + 1;
        FG_SLOG_KEY( startIndex - 1, FString::Printf( TEXT( "[STATES HISTORY %s]" ), *ownerName), FColor::Emerald );

        for( int32 i = 0; i < m_StatesHistory.Num(); ++i )
        {
            FG_SLOG_KEY( startIndex + i, FString::Printf(TEXT("%s"), *m_StatesHistory[i]->m_Id.ToString()), FColor::Emerald );
        }
    }
}

TObjectPtr<UStateMachineState> UStateMachineComponent::GetStateById( const FName& Id )
{
    auto* it = m_StatesInstances.FindByPredicate( [&Id]( TObjectPtr<UStateMachineState> _state )
    {
        return _state->m_Id == Id;
    } );

    return it ? *it : nullptr;
}
