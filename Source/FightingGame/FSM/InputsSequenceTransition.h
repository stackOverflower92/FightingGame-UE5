// Copyright (c) Giammarco Agazzotti

#pragma once

#include "CoreMinimal.h"
#include "FightingCharacterStateTransition.h"
#include "UObject/Object.h"
#include "InputsSequenceTransition.generated.h"

UCLASS()
class FIGHTINGGAME_API UInputsSequenceTransition : public UFightingCharacterStateTransition
{
    GENERATED_BODY()

public:
    UPROPERTY( EditAnywhere, DisplayName = "Inputs Sequence Name" )
    FString m_InputsSequenceName;

    UPROPERTY( EditAnywhere, DisplayName = "Require Hit Landed" )
    bool m_RequireHitLanded = false;

    virtual bool CanPerformTransition() override;
};
