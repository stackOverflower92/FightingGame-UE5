// Copyright (c) Giammarco Agazzotti

#pragma once

#include "CoreMinimal.h"
#include "MathStatics.generated.h"

UCLASS()
class FIGHTINGGAME_API UMathStatics : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    UFUNCTION( BlueprintCallable, Category = "Math" )
    static float GetAngleBetween( const FVector2D A, const FVector2D B );
};
