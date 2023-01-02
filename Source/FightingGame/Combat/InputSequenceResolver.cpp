// Copyright (c) Giammarco Agazzotti

#include "InputSequenceResolver.h"

void UInputSequenceResolver::Init( const TArray<TObjectPtr<UInputsSequence>>& InputsList, const TArray<TTuple<bool, bool>>& GroundedAirborneStates )
{
    ensureMsgf( InputsList.Num() == GroundedAirborneStates.Num(), TEXT("Inputs list size differs from grounded airborne states size") );

    for( int32 i = 0; i < InputsList.Num(); ++i )
    {
        TObjectPtr<UInputsSequence> sequence = InputsList[i];
        TArray<FMoveInputState>& inputs      = sequence->m_Inputs;
        bool allowWhenGrounded               = GroundedAirborneStates[i].Key;
        bool allowWhenAirborne               = GroundedAirborneStates[i].Value;

        auto startsWithSameInput = [&]( TSharedPtr<FInputResolverNode> _root )
        {
            ensureMsgf( !inputs.IsEmpty(), TEXT("Sequence [%s] has no inputs"), *sequence->m_Name.ToString() );

            return _root->m_InputState == inputs[0];
        };

        if( auto it = m_Trees.FindByPredicate( startsWithSameInput ) )
        {
            m_CurrentSequenceRoot = *it;
        }
        else
        {
            m_CurrentSequenceRoot = MakeShared<FInputResolverNode>( sequence->GetUniqueID(), inputs[0],
                                                                    GroundedAirborneStates[0].Key, GroundedAirborneStates[0].Value );

            m_Trees.Emplace( m_CurrentSequenceRoot );
        }

        for( int32 j = 1; j < inputs.Num(); ++j )
        {
            auto node = MakeShared<FInputResolverNode>( sequence->GetUniqueID(), inputs[j], allowWhenGrounded, allowWhenAirborne );

            InsertNode( node );
        }
    }
}

void UInputSequenceResolver::RegisterInput( EInputEntry InputEntry )
{
    TArray<TSharedPtr<FInputResolverNode>> nodesArray = m_CurrentRouteNode ? m_CurrentRouteNode->m_Children : m_Trees;

    auto sameInputEntryLambda = [&InputEntry]( TSharedPtr<FInputResolverNode> _node )
    {
        // #TODO check state too
        return _node->m_InputState.m_InputEntry == InputEntry;
    };

    if( auto* it = nodesArray.FindByPredicate( sameInputEntryLambda ) )
    {
        if( (*it)->m_Children.IsEmpty() )
        {
            m_InputRouteEndedDelegate.Broadcast( (*it)->m_UniqueId );

            ResetRouteTimer();
            m_CurrentRouteNode = nullptr;
        }
        else
        {
            m_CurrentRouteNode = *it;

            ResetRouteTimer();
            StartRouteTimer();
        }
    }
    else
    {
        if( m_ResetRouteOnIncorrectInput )
        {
            ResetRouteTimer();
            m_CurrentRouteNode = nullptr;
        }
    }
}

void UInputSequenceResolver::InsertNode( TSharedPtr<FInputResolverNode> Node )
{
    if( !m_CurrentSequenceRoot )
    {
        m_CurrentSequenceRoot = Node;
    }
    else
    {
        auto sameInputStateLambda = [&]( TSharedPtr<FInputResolverNode> _child )
        {
            return _child->m_InputState == Node->m_InputState;
        };

        if( auto* it = m_CurrentSequenceRoot->m_Children.FindByPredicate( sameInputStateLambda ) )
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
