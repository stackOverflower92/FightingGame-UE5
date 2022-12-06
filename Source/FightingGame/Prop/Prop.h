// Copyright (c) Giammarco Agazzotti

#pragma once

#include "CoreMinimal.h"
#include "FightingGame/Combat/Hittable.h"
#include "GameFramework/Actor.h"
#include "Prop.generated.h"

UCLASS()
class FIGHTINGGAME_API AProp : public AActor, public IHittable
{
	GENERATED_BODY()

public:
	AProp();

protected:
	virtual void BeginPlay() override;

public:
	virtual void OnHitReceived( const HitData& HitData ) override;

	virtual void Tick( float DeltaTime ) override;
};
