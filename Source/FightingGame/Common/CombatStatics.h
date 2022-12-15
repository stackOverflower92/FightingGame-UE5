// Copyright (c) Giammarco Agazzotti

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CombatStatics.generated.h"

class AFightingCharacter;
class UMoveDataAsset;

UCLASS()
class FIGHTINGGAME_API UCombatStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION( BlueprintCallable, Category = "Combat" )
	static bool ExecuteMove( AFightingCharacter* Character, UMoveDataAsset* Move );

	UFUNCTION( BlueprintCallable, Category = "Combat" )
	static bool FaceOther( AFightingCharacter* A, AActor* B, bool Instant = false );

	UFUNCTION( BlueprintCallable, Category = "Combat" )
	static bool FaceLocation( AFightingCharacter* A, const FVector& Location );

	UFUNCTION( BlueprintCallable, Category = "Combat" )
	static FVector GetKnockbackFromOrientation( AFightingCharacter* Character, float Orientation );

	UFUNCTION( BlueprintCallable, Category = "Combat" )
	static bool ApplyKnockbackTo( const FVector& Direction, float Force, AFightingCharacter* Character, bool IgnoreMultiplier );

	UFUNCTION( BlueprintCallable, Category = "Combat" )
	static float GetMinCustomTimeDilation();

	UFUNCTION( BlueprintCallable, Category = "Combat" )
	static float GetHitStunInitialDelay();
};
