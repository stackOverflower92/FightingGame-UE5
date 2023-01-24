// Copyright (c) Giammarco Agazzotti

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "SuperArmorNotifyState.generated.h"

UCLASS()
class FIGHTINGGAME_API USuperArmorNotifyState : public UAnimNotifyState
{
    GENERATED_BODY()

public:
    virtual void NotifyBegin( USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration,
                              const FAnimNotifyEventReference& EventReference ) override;

    virtual void NotifyEnd( USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference ) override;

protected:
    UPROPERTY( EditAnywhere, BlueprintReadOnly, DisplayName = "Damage Percent Multiplier" )
    float m_DamagePercentMultiplier = 1.f;

    UPROPERTY( EditAnywhere, BlueprintReadOnly, DisplayName = "Damage Multiplier" )
    float m_DamageMultiplier = 1.f;
};
