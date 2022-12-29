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

UCLASS()
class FIGHTINGGAME_API UInputSequenceResolver : public UObject
{
    GENERATED_BODY()

public:
    void Init( TArray<TObjectPtr<UMoveDataAsset>>& MovesList );

protected:
    UPROPERTY( EditAnywhere, BlueprintReadOnly, DisplayName = "Route Auto-Reset Time (Seconds)" )
    float m_RouteAutoResetTime = 0.1f;

private:
    TArray<TSharedPtr<FInputResolverNode>> m_Trees;
    TSharedPtr<FInputResolverNode> m_CurrentSequenceRoot = nullptr;

    void InsertEntry( TSharedPtr<FInputResolverNode> Node );
};
