// Copyright (c) Giammarco Agazzotti

#pragma once

#include "CoreMinimal.h"
#include "MoveDataAsset.h"
#include "FightingGame/Input/InputEntry.h"
#include "UObject/Object.h"
#include "InputSequenceResolver.generated.h"

struct FInputResolverNode
{
    explicit FInputResolverNode( uint32 MoveUniqueId, FMoveInputState InputState, bool AllowWhenGrounded, bool AllowWhenAirborne )
        : m_MoveUniqueId( MoveUniqueId ), m_InputState( InputState ), m_AllowWhenGrounded( AllowWhenGrounded ), m_AllowWhenAirborne( AllowWhenAirborne )
    {
    }

    uint32 m_MoveUniqueId;
    FMoveInputState m_InputState;
    bool m_AllowWhenGrounded = true;
    bool m_AllowWhenAirborne = false;

    TSharedPtr<FInputResolverNode> m_Parent = nullptr;
    TArray<TSharedPtr<FInputResolverNode>> m_Children;
};

DECLARE_MULTICAST_DELEGATE_OneParam( FInputRouteEnded, uint32 )

UCLASS( Abstract, Blueprintable, BlueprintType, HideCategories = ("Cooking", "LOD", "Physics", "Activation", "Tags", "Rendering") )
class FIGHTINGGAME_API UInputSequenceResolver : public UObject
{
    GENERATED_BODY()

public:
    FInputRouteEnded m_InputRouteEndedDelegate;

    void Init( const TArray<TObjectPtr<UMoveDataAsset>>& MovesList );
    void RegisterInput( EInputEntry InputEntry );

protected:
    UPROPERTY( EditAnywhere, BlueprintReadOnly, DisplayName = "Route Auto-Reset Time (Seconds)" )
    float m_RouteAutoResetTime = 0.1f;

    UPROPERTY( EditAnywhere, BlueprintReadOnly, DisplayName = "Reset Route On Incorrect Input" )
    bool m_ResetRouteOnIncorrectInput = true;

private:
    TArray<TSharedPtr<FInputResolverNode>> m_Trees;
    TSharedPtr<FInputResolverNode> m_CurrentSequenceRoot = nullptr;

    TSharedPtr<FInputResolverNode> m_CurrentRouteNode = nullptr;

    FTimerHandle m_RouteTimerHandle;

    void InsertNode( TSharedPtr<FInputResolverNode> Node );
    void StartRouteTimer();
    void ResetRouteTimer();
    void OnRouteTimerEnded();
};
