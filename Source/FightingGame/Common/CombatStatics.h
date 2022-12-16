// Copyright (c) Giammarco Agazzotti

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CombatStatics.generated.h"

struct FHitboxDescription;
struct HitData;

class IFacingEntity;
class AFightingCharacter;
class UMoveDataAsset;

UCLASS()
class FIGHTINGGAME_API UCombatStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION( BlueprintCallable, Category = "Combat" )
	static bool ExecuteMove( AFightingCharacter* Character, UMoveDataAsset* Move );

	static bool FaceOther( IFacingEntity* A, AActor* B, bool Instant = false );

	static bool FaceLocation( IFacingEntity* A, const FVector& Location );

	static HitData GenerateHitDataFromHitboxDescription( TObjectPtr<AActor> HitboxOwner, TObjectPtr<USkeletalMeshComponent> SkeletalMesh,
	                                                     const FHitboxDescription& HitboxDesc );

	static FVector GetKnockbackFromOrientation( IFacingEntity* FacingEntity, float Orientation );

	UFUNCTION( BlueprintCallable, Category = "Combat" )
	static bool ApplyKnockbackTo( const FVector& Direction, float Force, AFightingCharacter* Character, bool IgnoreMultiplier );

	UFUNCTION( BlueprintCallable, Category = "Combat" )
	static float GetMinCustomTimeDilation();

	UFUNCTION( BlueprintCallable, Category = "Combat" )
	static float GetHitStunInitialDelay();
};
