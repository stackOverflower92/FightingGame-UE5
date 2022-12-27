// Copyright (c) Giammarco Agazzotti

#pragma once

#include "CoreMinimal.h"
#include "FightingCharacterStateTransition.h"
#include "FightingGame/Input/InputEntry.h"
#include "UObject/Object.h"
#include "InputTransition.generated.h"

UCLASS()
class FIGHTINGGAME_API UInputTransition : public UFightingCharacterStateTransition
{
    GENERATED_BODY()

public:
    UPROPERTY( EditAnywhere, DisplayName = "Input Entry" )
    EInputEntry m_InputEntry;

    UPROPERTY( EditAnywhere, DisplayName = "Require Hit Landed" )
    bool m_RequireHitLanded = false;

    virtual bool CanPerformTransition() override;
};
