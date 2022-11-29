// Copyright (c) Giammarco Agazzotti

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "FightingCharacterAnimInstance.generated.h"

UENUM( BlueprintType )
enum class EMontageEventType : uint8
{
	Ended,
	Interrupted,

	COUNT UMETA( Hidden ),
	INVALID UMETA( Hidden ),
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams( FMontageEvent, UAnimMontage*, Montage, EMontageEventType, EventType );

UCLASS()
class FIGHTINGGAME_API UFightingCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UFUNCTION( BlueprintImplementableEvent )
	void AnimationRequested( UAnimMontage* AnimationSequence );

	UPROPERTY( BlueprintAssignable, BlueprintCallable, DisplayName = "Montage Event" )
	FMontageEvent m_MontageEvent;
};
