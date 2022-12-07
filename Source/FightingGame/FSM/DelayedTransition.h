// Copyright (c) Giammarco Agazzotti

#pragma once

#include "CoreMinimal.h"
#include "FightingCharacterStateTransition.h"
#include "UObject/Object.h"
#include "DelayedTransition.generated.h"

UCLASS()
class FIGHTINGGAME_API UDelayedTransition : public UFightingCharacterStateTransition
{
	GENERATED_BODY()

public:
	virtual void OnStateEnter() override;
	virtual void OnStateExit() override;
	virtual bool CanPerformTransition() override;

protected:
	UPROPERTY( EditAnywhere, BlueprintReadWrite, DisplayName = "Delay" )
	float m_Delay = 0.f;

private:
	bool m_CanPerform = false;
	FTimerHandle m_TimerHandle;

	void ClearTimerIfActive();
	void OnTimerEnded();
};
