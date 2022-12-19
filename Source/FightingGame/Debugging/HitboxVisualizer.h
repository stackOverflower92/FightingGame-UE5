// Copyright (c) Giammarco Agazzotti

#pragma once

#include "CoreMinimal.h"
#include "SphereVisualizer.h"
#include "GameFramework/Actor.h"
#include "HitboxVisualizer.generated.h"

UCLASS()
class FIGHTINGGAME_API AHitboxVisualizer : public ASphereVisualizer
{
	GENERATED_BODY()

public:
	AHitboxVisualizer();

protected:
	UPROPERTY( EditAnywhere, BlueprintReadOnly, DisplayName = "Knockback Line Thickness" )
	float m_KnockbackLineThickness = 3.f;

	virtual void BeginPlay() override;

public:
	FORCEINLINE void SetKnockback( const FVector& Knockback ) { m_Knockback = Knockback; }

	virtual void Tick( float DeltaTime ) override;

protected:
	TOptional<FVector> m_Knockback;
};
