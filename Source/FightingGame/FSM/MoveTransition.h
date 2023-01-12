// Copyright (c) Giammarco Agazzotti

#pragma once

#include "CoreMinimal.h"
#include "FightingCharacterStateTransition.h"
#include "UObject/Object.h"
#include "MoveTransition.generated.h"

UCLASS()
class FIGHTINGGAME_API UMoveTransition : public UFightingCharacterStateTransition
{
    GENERATED_BODY()

public:
    UPROPERTY( EditAnywhere, BlueprintReadOnly, DisplayName = "Move Name" )
    FString m_MoveName;

    UPROPERTY( EditAnywhere, DisplayName = "Require Hit Landed" )
    bool m_RequireHitLanded = false;

    virtual bool CanPerformTransition() override;
};
