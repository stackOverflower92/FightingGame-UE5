// Copyright (c) Giammarco Agazzotti

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/PlayerStart.h"
#include "IndexedPlayerStart.generated.h"

UCLASS()
class FIGHTINGGAME_API AIndexedPlayerStart : public APlayerStart
{
    GENERATED_BODY()

public:
    UPROPERTY( EditAnywhere, BlueprintReadOnly, DisplayName = "Index" )
    int32 m_Index = 0;
};
