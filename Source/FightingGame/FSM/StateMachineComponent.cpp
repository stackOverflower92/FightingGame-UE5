// Copyright (c) Giammarco Agazzotti

#include "StateMachineComponent.h"

#include "FightingGame/Debugging/Debug.h"

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

    if( TObjectPtr<UStateMachineState> nextState = GetStateById( StateId ) )
    {
        m_CurrentState = nextState;
        m_CurrentState->OnEnter();
    }
    else
    {
        FG_SLOG_ERR( FString::Printf(TEXT("Could not find state named [%s]"), *StateId.ToString()) );
    }
}

void UStateMachineComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
    Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

    if( m_CurrentState )
    {
        m_CurrentState->OnTick( DeltaTime );
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
