// Copyright (c) Giammarco Agazzotti

#include "InputSequenceResolver.h"

#include "FightingGame/Debugging/Debug.h"

void UInputSequenceResolver::Init( TArray<TObjectPtr<UMoveDataAsset>>& MovesList )
{
    for( int32 moveIdx = 0; moveIdx < MovesList.Num(); ++moveIdx )
    {
        auto it = m_Trees.FindByPredicate( [&]( TSharedPtr<FInputResolverNode> _root )
        {
            if( MovesList[moveIdx]->m_InputsSequence.IsEmpty() )
            {
                FG_SLOG_ERR( FString::Printf(TEXT("Move [%s] has no inputs"), *MovesList[moveIdx]->m_Id) );
                return false;
            }

            return _root->m_InputState == MovesList[moveIdx]->m_InputsSequence[0];
        } );

        TSharedPtr<FInputResolverNode> node;
        if( it )
        {
            node = *it;
        }
        else
        {
            node                 = MakeShared<FInputResolverNode>();
            node->m_MoveUniqueId = MovesList[moveIdx]->GetUniqueID();
            node->m_InputState   = MovesList[moveIdx]->m_InputsSequence[0];

            m_Trees.Emplace( node );
        }

        for( int32 inputIdx = 1; inputIdx < MovesList[moveIdx]->m_InputsSequence.Num(); ++inputIdx )
        {
            TSharedPtr<FInputResolverNode> newNode = MakeShared<FInputResolverNode>();
            newNode->m_InputState                  = MovesList[moveIdx]->m_InputsSequence[inputIdx];
            newNode->m_MoveUniqueId                = MovesList[moveIdx]->GetUniqueID();

            InsertEntry( node, newNode );
        }
    }

    FG_SLOG_INFO( TEXT("Bla") );
}

void UInputSequenceResolver::InsertEntry( TSharedPtr<FInputResolverNode> Root, TSharedPtr<FInputResolverNode> Node )
{
    if( !Root )
    {
        Root = Node;
    }
    else
    {
        auto* it = Root->m_Children.FindByPredicate( [&]( TSharedPtr<FInputResolverNode> _child )
        {
            return _child->m_InputState == Node->m_InputState;
        } );

        if( it )
        {
            TSharedPtr<FInputResolverNode> targetNode = *it;
            InsertEntry( targetNode, Node );
        }
        else
        {
            Node->m_Parent = Root;
            Root->m_Children.Emplace( Node );
        }
    }
}
