// Copyright (c) Giammarco Agazzotti

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "PushCharacterNotify.generated.h"

UCLASS()
class FIGHTINGGAME_API UPushCharacterNotify : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify( USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference ) override;

protected:
	/*Positive value: towards facing, negative value: opposite to facing*/
	UPROPERTY( EditAnywhere, BlueprintReadWrite, DisplayName = "Force" )
	float m_Force = 0.f;

	/*Angle (degrees) of force vector, calculated from the character forward vector*/
	UPROPERTY( EditAnywhere, BlueprintReadWrite, DisplayName = "Orientation" )
	float m_Orientation = .0f;
};
