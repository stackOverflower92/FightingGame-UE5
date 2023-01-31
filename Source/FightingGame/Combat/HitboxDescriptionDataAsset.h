// Copyright (c) Giammarco Agazzotti

#pragma once

#include "CoreMinimal.h"
#include "HitboxDescription.h"
#include "HitboxDescriptionDataAsset.generated.h"

UCLASS()
class FIGHTINGGAME_API UHitboxDescriptionDataAsset : public UDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY( EditAnywhere, BlueprintReadOnly, DisplayName = "Hitbox Description" )
    FHitboxDescription m_HitboxDescription;
};
