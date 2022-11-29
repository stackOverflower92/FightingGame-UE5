// Copyright (c) Giammarco Agazzotti

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "MoveDataAsset.generated.h"

class UAnimationAsset;

UCLASS()
class FIGHTINGGAME_API UMoveDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY( EditAnywhere, BlueprintReadOnly, DisplayName = "Id" )
	FString m_Id;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, DisplayName = "Animation Montage" )
	TObjectPtr<UAnimMontage> m_AnimationMontageAsset = nullptr;
};
