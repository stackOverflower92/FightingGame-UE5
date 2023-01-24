// Copyright (c) Giammarco Agazzotti

#include "InputSequenceResolver.h"
#include "FightingGame/Debugging/Debug.h"

void UInputSequenceResolver::Init( const TArray<TObjectPtr<UInputsSequence>>& InputsList )
{
    for( int32 i = 0; i < InputsList.Num(); ++i )
    {
        TObjectPtr<UInputsSequence> sequence = InputsList[i];
        TArray<FMoveInputState>& inputs      = sequence->m_Inputs;

        auto startsWithSameInput = [&]( TSharedPtr<FInputResolverNode> _root )
        {
            ensureMsgf( !inputs.IsEmpty(), TEXT("Sequence [%s] has no inputs"), *sequence->m_Name );

            return _root->m_InputState == inputs[0];
        };

        if( auto it = m_Trees.FindByPredicate( startsWithSameInput ) )
        {
            m_CurrentSequenceRoot = *it;
        }
        else
        {
            m_CurrentSequenceRoot = MakeShared<FInputResolverNode>( sequence, inputs[0] );

            m_Trees.Emplace( m_CurrentSequenceRoot );
        }

        for( int32 j = 1; j < inputs.Num(); ++j )
        {
            auto node = MakeShared<FInputResolverNode>( sequence, inputs[j] );

            InsertNode( node );
        }
    }
}

EInputRegistrationResult UInputSequenceResolver::RegisterInput( EInputEntry InputEntry )
{
    if( !m_CurrentRouteNode && InputEntry == EInputEntry::None )
    {
        FG_SLOG_ERR( TEXT("A combo cannot start with a null entry") );
    }

    TArray<TSharedPtr<FInputResolverNode>> nodesArray = m_CurrentRouteNode ? m_CurrentRouteNode->m_Children : m_Trees;

    auto predSameInputEntry = [&InputEntry]( TSharedPtr<FInputResolverNode> _node )
    {
        return _node->m_InputState.m_InputEntry == InputEntry;
    };

    if( auto* it = nodesArray.FindByPredicate( predSameInputEntry ) )
    {
        if( (*it)->m_Children.IsEmpty() )
        {
            m_InputRouteEndedDelegate.Broadcast( (*it)->m_InputsSequence );
            m_CurrentRouteNode = nullptr;

            return EInputRegistrationResult::RouteEnded;
        }

        m_CurrentRouteNode = *it;
        ResetRouteTimer();
        StartRouteTimer();

        return EInputRegistrationResult::InputFound;
    }

    ResetRouteTimer();
    m_CurrentRouteNode = nullptr;

    return EInputRegistrationResult::InputNotFound;
}

void UInputSequenceResolver::InsertNode( TSharedPtr<FInputResolverNode> Node )
{
    if( !m_CurrentSequenceRoot )
    {
        m_CurrentSequenceRoot = Node;
    }
    else
    {
        auto predIsSameInputState = [&]( TSharedPtr<FInputResolverNode> _child )
        {
            return _child->m_InputState == Node->m_InputState;
        };

        if( auto* it = m_CurrentSequenceRoot->m_Children.FindByPredicate( predIsSameInputState ) )
        {
            m_CurrentSequenceRoot = *it;
            InsertNode( Node );
        }
        else
        {
            Node->m_Parent = m_CurrentSequenceRoot;
            m_CurrentSequenceRoot->m_Children.Emplace( Node );

            m_CurrentSequenceRoot = Node;
        }
    }
}

void UInputSequenceResolver::StartRouteTimer()
{
    FTimerManager& timerManager = GetWorld()->GetTimerManager();
    timerManager.SetTimer( m_RouteTimerHandle, this, &UInputSequenceResolver::OnRouteTimerEnded, m_RouteAutoResetTime );
}

void UInputSequenceResolver::ResetRouteTimer()
{
    FTimerManager& timerManager = GetWorld()->GetTimerManager();
    if( timerManager.IsTimerActive( m_RouteTimerHandle ) )
    {
        timerManager.ClearTimer( m_RouteTimerHandle );
    }
}

void UInputSequenceResolver::OnRouteTimerEnded()
{
    m_CurrentRouteNode = nullptr;
}
