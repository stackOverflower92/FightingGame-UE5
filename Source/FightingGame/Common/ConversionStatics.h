// Copyright (c) Giammarco Agazzotti

#pragma once

#include "CoreMinimal.h"
#include "Kismet/KismetStringLibrary.h"
#include "ConversionStatics.generated.h"

UCLASS()
class FIGHTINGGAME_API UConversionStatics : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    template<typename EnumType>
    FORCEINLINE static FString ConvertEnumValueToString( const EnumType Value, bool includeEnumName = true )
    {
        FString converted = UEnum::GetValueAsString( Value );
        if( includeEnumName )
        {
            return converted;
        }

        FString left, right;
        if( UKismetStringLibrary::Split( converted, TEXT( "::" ), left, right ) )
        {
            return right;
        }

        return converted;
    }
};
