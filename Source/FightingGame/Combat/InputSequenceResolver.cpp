﻿// Copyright (c) Giammarco Agazzotti

#include "InputSequenceResolver.h"

#include "FightingGame/Debugging/Debug.h"

void UInputSequenceResolver::Init( TArray<TObjectPtr<UMoveDataAsset>>& MovesList )
{
    for( int32 moveIdx = 0; moveIdx < MovesList.Num(); ++moveIdx )
    {
        auto it = m_Trees.FindByPredicate( [&]( TSharedPtr<FInputResolverNode> _root )
        {
            ensureMsgf( !MovesList[moveIdx]->m_InputsSequence.IsEmpty(), TEXT("Move [%s] has no inputs"), *MovesList[moveIdx]->m_Id );

            return _root->m_InputState == MovesList[moveIdx]->m_InputsSequence[0];
        } );

        if( it )
        {
            m_CurrentSequenceRoot = *it;
        }
        else
        {
            m_CurrentSequenceRoot                 = MakeShared<FInputResolverNode>();
            m_CurrentSequenceRoot->m_MoveUniqueId = MovesList[moveIdx]->GetUniqueID();
            m_CurrentSequenceRoot->m_InputState   = MovesList[moveIdx]->m_InputsSequence[0];

            m_Trees.Emplace( m_CurrentSequenceRoot );
        }

        for( int32 inputIdx = 1; inputIdx < MovesList[moveIdx]->m_InputsSequence.Num(); ++inputIdx )
        {
            TSharedPtr<FInputResolverNode> node = MakeShared<FInputResolverNode>();
            node->m_InputState                  = MovesList[moveIdx]->m_InputsSequence[inputIdx];
            node->m_MoveUniqueId                = MovesList[moveIdx]->GetUniqueID();

            InsertEntry( node );
        }
    }
}

void UInputSequenceResolver::RegisterInput( EInputEntry InputEntry )
{
    TArray<TSharedPtr<FInputResolverNode>> nodesArray;
    if( m_CurrentRouteNode )
    {
        nodesArray = m_CurrentRouteNode->m_Children;
    }
    else
    {
        nodesArray = m_Trees;
    }

    auto* it = nodesArray.FindByPredicate( [&InputEntry]( TSharedPtr<FInputResolverNode> _node )
    {
        // #TODO check state too
        return _node->m_InputState.m_InputEntry == InputEntry;
    } );

    if( it )
    {
        if( (*it)->m_Children.IsEmpty() )
        {
            // End of the route reached
            m_InputRouteEndedDelegate.Broadcast( (*it)->m_MoveUniqueId );

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

    /*if( m_RoutingStarted )
    {
        auto* it = m_CurrentRouteNode->m_Children.FindByPredicate( [&InputEntry]( TSharedPtr<FInputResolverNode> _node )
        {
            // #TODO check state too
            return _node->m_InputState.m_InputEntry == InputEntry;
        } );

        if( it )
        {
            m_CurrentRouteNode = *it;

            // #TODO handle timer

            if( m_CurrentRouteNode->m_Children.IsEmpty() )
            {
                // End of the route reached
                m_InputRouteEndedDelegate.Broadcast( m_CurrentRouteNode->m_MoveUniqueId );
            }
        }
    }
    else
    {
        auto* it = m_Trees.FindByPredicate( [&InputEntry]( TSharedPtr<FInputResolverNode> _node )
        {
            // #TODO check state too
            return _node->m_InputState.m_InputEntry == InputEntry;
        } );

        if( it )
        {
            m_RoutingStarted   = true;
            m_CurrentRouteNode = *it;

            // #TODO handle timer

            if( m_CurrentRouteNode->m_Children.IsEmpty() )
            {
                // End of the route reached
                m_InputRouteEndedDelegate.Broadcast( m_CurrentRouteNode->m_MoveUniqueId );
            }
        }
    }*/
}

void UInputSequenceResolver::InsertEntry( TSharedPtr<FInputResolverNode> Node )
{
    if( !m_CurrentSequenceRoot )
    {
        m_CurrentSequenceRoot = Node;
    }
    else
    {
        auto* it = m_CurrentSequenceRoot->m_Children.FindByPredicate( [&]( TSharedPtr<FInputResolverNode> _child )
        {
            return _child->m_InputState == Node->m_InputState;
        } );

        if( it )
        {
            m_CurrentSequenceRoot = *it;
            InsertEntry( Node );
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
