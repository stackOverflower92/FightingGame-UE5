// Copyright (c) Giammarco Agazzotti

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "HitboxNotifyState.generated.h"

struct FHitboxDescription;

UCLASS()
class FIGHTINGGAME_API UHitboxNotifyState : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	UPROPERTY( EditAnywhere, BlueprintReadWrite, DisplayName = "Hitboxes" )
	TArray<FHitboxDescription> m_HitBoxes;

	virtual void NotifyBegin( USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration,
	                          const FAnimNotifyEventReference& EventReference ) override;

	virtual void NotifyEnd( USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference ) override;
};
