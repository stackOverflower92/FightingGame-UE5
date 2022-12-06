// Copyright (c) Giammarco Agazzotti

#pragma once

#include "CoreMinimal.h"
#include "FightingCharacterStateTransition.h"
#include "UObject/Object.h"
#include "MontageEndedTransition.generated.h"

enum class EMontageEventType : uint8;
class UFightingCharacterAnimInstance;

UCLASS()
class FIGHTINGGAME_API UMontageEndedTransition : public UFightingCharacterStateTransition
{
	GENERATED_BODY()

public:
	virtual void OnStateEnter() override;

	virtual void OnMontageEvent( UAnimMontage* Montage, EMontageEventType MontageEvent ) override;

	virtual bool CanPerformTransition() override;

private:
	bool m_CanTransition = false;

	UPROPERTY()
	UFightingCharacterAnimInstance* m_AnimInstance = nullptr;
};
