// Copyright (c) Giammarco Agazzotti

#pragma once

#include "CoreMinimal.h"
#include "MoveDataAsset.h"
#include "FightingGame/Input/InputEntry.h"
#include "UObject/Object.h"
#include "InputSequenceResolver.generated.h"

struct FInputResolverNode
{
    uint32 m_MoveUniqueId;
    FMoveInputState m_InputState;

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

    void Init( TArray<TObjectPtr<UMoveDataAsset>>& MovesList );
    void RegisterInput( EInputEntry InputEntry );

protected:
    UPROPERTY( EditAnywhere, BlueprintReadOnly, DisplayName = "Route Auto-Reset Time (Seconds)" )
    float m_RouteAutoResetTime = 0.1f;

private:
    TArray<TSharedPtr<FInputResolverNode>> m_Trees;
    TSharedPtr<FInputResolverNode> m_CurrentSequenceRoot = nullptr;

    TSharedPtr<FInputResolverNode> m_CurrentRouteNode = nullptr;

    FTimerHandle m_RouteTimerHandle;

    void InsertEntry( TSharedPtr<FInputResolverNode> Node );
    void StartRouteTimer();
    void ResetRouteTimer();
    void OnRouteTimerEnded();
};
