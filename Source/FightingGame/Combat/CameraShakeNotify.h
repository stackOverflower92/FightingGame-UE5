// Copyright (c) Giammarco Agazzotti

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "CameraShakeNotify.generated.h"

UCLASS()
class FIGHTINGGAME_API UCameraShakeNotify : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify( USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference ) override;

protected:
	UPROPERTY( EditAnywhere, BlueprintReadWrite, DisplayName = "Shake" )
	TSubclassOf<UCameraShakeBase> m_Shake = nullptr;
};
