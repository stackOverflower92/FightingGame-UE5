// Copyright (c) Giammarco Agazzotti

#pragma once

#include "CoreMinimal.h"
#include "FightingCharacterStateTransition.h"
#include "UObject/Object.h"
#include "GroundedTransition.generated.h"

UCLASS()
class FIGHTINGGAME_API UGroundedTransition : public UFightingCharacterStateTransition
{
	GENERATED_BODY()

public:
	virtual bool CanPerformTransition() override;
	virtual void OnStateEnter( TObjectPtr<AFightingCharacter> Character ) override;
	virtual void OnGrounded() override;
	virtual void OnAirborne() override;

protected:
	UPROPERTY( EditAnywhere, BlueprintReadWrite, DisplayName = "Must Be Grounded" )
	bool m_MustBeGrounded = true;

private:
	bool m_CanTransition = false;
};
