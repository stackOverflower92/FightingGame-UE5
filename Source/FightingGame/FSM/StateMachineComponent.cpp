// Copyright (c) Giammarco Agazzotti

#include "StateMachineComponent.h"

#include "FightingGame/Debugging/Debug.h"

namespace
{
    int32 loc_DebugCurrentState = 0;
    FG_CVAR_FLAG_DESC( CVarDebugDamageStats, TEXT("StateMachine.DebugCurrentState"), loc_DebugCurrentState );
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
    if( m_CurrentState )
    {
        m_CurrentState->OnExit();
    }

    m_CanUpdateCurrentState = false;

    if( TObjectPtr<UStateMachineState> nextState = GetStateById( StateId ) )
    {
        m_CurrentState = nextState;
        m_CurrentState->OnEnter();
    }
    else
    {
        FG_SLOG_ERR( FString::Printf(TEXT("Could not find state named [%s]"), *StateId.ToString()) );
    }

    m_CanUpdateCurrentState = true;
}

void UStateMachineComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
    Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

    if( m_CurrentState && m_CanUpdateCurrentState )
    {
        m_CurrentState->OnTick( DeltaTime );
    }

    if( loc_DebugCurrentState )
    {
        FG_SLOG_KEY( static_cast<uint64>(GetUniqueID()), FString::Printf(TEXT("(%s) STATE: %s"),
                         *GetOwner()->GetName(), *m_CurrentState->m_Id.ToString()), FColor::Orange );
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
