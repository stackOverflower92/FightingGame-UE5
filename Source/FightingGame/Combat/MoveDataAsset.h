// Copyright (c) Giammarco Agazzotti

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "FightingGame/Combat/MoveInputState.h"
#include "MoveDataAsset.generated.h"

enum class EInputEntry : uint8;
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

    UPROPERTY( EditAnywhere, BlueprintReadOnly, DisplayName = "Inputs Sequence" )
    TArray<FMoveInputState> m_InputsSequence;
};
