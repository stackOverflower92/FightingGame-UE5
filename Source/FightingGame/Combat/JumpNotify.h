// Copyright (c) Giammarco Agazzotti

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "JumpNotify.generated.h"

UCLASS()
class FIGHTINGGAME_API UJumpNotify : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify( USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference ) override;
};
