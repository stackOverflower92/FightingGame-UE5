// Copyright (c) Giammarco Agazzotti

#pragma once

#include "CoreMinimal.h"
#include "InputsSequenceToStateMap.generated.h"

UCLASS()
class FIGHTINGGAME_API UInputsSequenceToStateMap : public UDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY( EditAnywhere, BlueprintReadOnly, DisplayName = "Values" )
    TMap<FName, FName> m_Values;
};
