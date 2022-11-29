// Copyright (c) Giammarco Agazzotti

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "HitboxNotifyState.generated.h"


UCLASS()
class FIGHTINGGAME_API UHitboxNotifyState : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	UPROPERTY( EditAnywhere, BlueprintReadWrite, DisplayName = "Socket Name" )
	FName m_SocketName;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, DisplayName = "Priority" )
	int m_Priority = 0;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, DisplayName = "Force Opponent Facing" )
	bool m_ForceOpponentFacing = true;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, DisplayName = "Damage Percent" )
	float m_DamagePercent = 0.f;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, DisplayName = "Radius" )
	float m_Radius = 10.f;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, DisplayName = "Knockback Orientation" )
	float m_KnockbackOrientation = 0.f;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, DisplayName = "Knockback Force" )
	float m_KnockbackForce = 600.f;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, DisplayName = "Ignore Knockback Multiplier" )
	bool m_IgnoreKnockbackMultiplier = false;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, DisplayName = "Hit Stun Duration" )
	float m_HitStunDuration = 0.f;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, DisplayName = "Shake" )
	bool m_Shake = true;

	virtual void NotifyBegin( USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration ) override;
	virtual void NotifyEnd( USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation ) override;
};
