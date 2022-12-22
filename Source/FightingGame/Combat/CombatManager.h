// Copyright (c) Giammarco Agazzotti

#pragma once

#include "CoreMinimal.h"
#include "FightingGame/Common/Manager.h"
#include "GameFramework/Actor.h"
#include "CombatManager.generated.h"

UCLASS()
class FIGHTINGGAME_API ACombatManager : public AManager
{
	GENERATED_BODY()

public:
	ACombatManager();

	FORCEINLINE float GetHitStopStartDelay() const { return m_HitStopStartDelay; }
	FORCEINLINE float GetHitStopTimeDilation() const { return m_HitStopTimeDilation; }

protected:
	UPROPERTY( EditAnywhere, BlueprintReadOnly, DisplayName = "Hit Stop Start Delay" )
	float m_HitStopStartDelay = 0.f;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, DisplayName = "Hit Stop Time Dilation" )
	float m_HitStopTimeDilation = 0.001f;

	virtual void BeginPlay() override;

public:
	virtual void Tick( float DeltaTime ) override;
};
