// Copyright (c) Giammarco Agazzotti

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HitStopComponent.generated.h"

class AFightingCharacter;
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FIGHTINGGAME_API UHitStopComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UHitStopComponent();

protected:
	virtual void BeginPlay() override;

public:
	void EnableHitStop( float Duration, bool Shake );

	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

private:
	FTimerHandle m_HitStopStopTimerHandle;
	FTimerHandle m_HitStopBeginTimerHandle;

	float m_CachedHitStopDuration = 0.f;
	bool m_CachedDoMeshShake      = false;
	bool m_UpdateMeshShake        = false;

	bool m_HitStopRunning = false;

	UPROPERTY()
	TObjectPtr<AFightingCharacter> m_Character = nullptr;

	void StartBeginHitStopTimer( float Duration, bool Shake );
	void OnHitStopBeginTimerEnded();

	void StartStopHitStopTimer();
	void OnHitStopStopTimerEnded();
};
