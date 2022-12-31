// Copyright (c) Giammarco Agazzotti

#include "InputSequenceResolver.h"

void UInputSequenceResolver::Init( TArray<TObjectPtr<UMoveDataAsset>>& MovesList )
{
    for( int32 moveIdx = 0; moveIdx < MovesList.Num(); ++moveIdx )
    {
        auto it = m_Trees.FindByPredicate( [&]( TSharedPtr<FInputResolverNode> _root )
        {
            ensureMsgf( !MovesList[moveIdx]->m_InputsSequence->m_Inputs.IsEmpty(), TEXT("Move [%s] has no inputs"), *MovesList[moveIdx]->m_Id.ToString() );

            return _root->m_InputState == MovesList[moveIdx]->m_InputsSequence->m_Inputs[0];
        } );

        if( it )
        {
            m_CurrentSequenceRoot = *it;
        }
        else
        {
            m_CurrentSequenceRoot = MakeShared<FInputResolverNode>( MovesList[moveIdx]->GetUniqueID(), MovesList[moveIdx]->m_InputsSequence->m_Inputs[0],
                                                                    MovesList[moveIdx]->m_AllowWhenGrounded, MovesList[moveIdx]->m_AllowWhenAirborne );

            m_Trees.Emplace( m_CurrentSequenceRoot );
        }

        for( int32 inputIdx = 1; inputIdx < MovesList[moveIdx]->m_InputsSequence->m_Inputs.Num(); ++inputIdx )
        {
            TSharedPtr<FInputResolverNode> node = MakeShared<FInputResolverNode>( MovesList[moveIdx]->GetUniqueID(),
                                                                                  MovesList[moveIdx]->m_InputsSequence->m_Inputs[inputIdx],
                                                                                  MovesList[moveIdx]->m_AllowWhenGrounded, MovesList[moveIdx]->m_AllowWhenAirborne );
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
