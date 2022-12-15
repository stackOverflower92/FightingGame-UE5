// Copyright (c) Giammarco Agazzotti

#pragma once

#include "CoreMinimal.h"
#include "FightingCharacterStateTransition.h"
#include "UObject/Object.h"
#include "MovementTransition.generated.h"

UENUM( BlueprintType )
enum class EThresholdComparison : uint8
{
	GreaterThan,
	LessThan,

	COUNT UMETA( Hidden ),
	INVALID UMETA( Hidden )
};

UCLASS()
class FIGHTINGGAME_API UMovementTransition : public UFightingCharacterStateTransition
{
	GENERATED_BODY()

public:
	virtual bool CanPerformTransition() override;

protected:
	UPROPERTY( EditAnywhere, DisplayName = "Threshold" )
	float m_Threshold = 0.f;

	UPROPERTY( EditAnywhere, DisplayName = "Comparison" )
	EThresholdComparison m_ThresholdComparison = EThresholdComparison::GreaterThan;
};
