// Copyright (c) Giammarco Agazzotti

#pragma once

#include "CoreMinimal.h"
#include "FightingCharacterStateTransition.h"
#include "UObject/Object.h"
#include "InputTransition.generated.h"

UCLASS()
class FIGHTINGGAME_API UInputTransition : public UFightingCharacterStateTransition
{
	GENERATED_BODY()

public:
	UPROPERTY( EditAnywhere, DisplayName = "Input Entry" )
	FString m_InputEntry;

	virtual bool CanPerformTransition() override;
};
