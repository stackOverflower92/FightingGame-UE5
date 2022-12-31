// Copyright (c) Giammarco Agazzotti

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "FightingGame/Combat/MoveInputState.h"
#include "FightingGame/Input/InputsSequence.h"
#include "MoveDataAsset.generated.h"

enum class EInputEntry : uint8;
class UAnimationAsset;

UCLASS()
class FIGHTINGGAME_API UMoveDataAsset : public UDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY( EditAnywhere, BlueprintReadOnly, DisplayName = "Id" )
    FName m_Id;

    UPROPERTY( EditAnywhere, BlueprintReadOnly, DisplayName = "Animation Montage" )
    TObjectPtr<UAnimMontage> m_AnimationMontageAsset = nullptr;

    UPROPERTY( EditAnywhere, BlueprintReadOnly, DisplayName = "Inputs Sequence" )
    TObjectPtr<UInputsSequence> m_InputsSequence;

    UPROPERTY( EditAnywhere, BlueprintReadOnly, DisplayName = "Allow When Grounded" )
    bool m_AllowWhenGrounded = true;

    UPROPERTY( EditAnywhere, BlueprintReadOnly, DisplayName = "Allow When Airborne" )
    bool m_AllowWhenAirborne = false;
};
