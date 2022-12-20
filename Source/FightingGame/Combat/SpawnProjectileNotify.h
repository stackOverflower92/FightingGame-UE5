// Copyright (c) Giammarco Agazzotti

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "SpawnProjectileNotify.generated.h"

class AProjectile;
UCLASS()
class FIGHTINGGAME_API USpawnProjectileNotify : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify( USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference ) override;

protected:
	UPROPERTY( EditAnywhere, DisplayName = "Projectile Class" )
	TSubclassOf<AProjectile> m_ProjectileClass = nullptr;

	UPROPERTY( EditAnywhere, DisplayName = "Spawn Offset (Account Facing)" )
	FVector m_SpawnOffset;

	UPROPERTY( EditAnywhere, DisplayName = "Base Speed" )
	float m_BaseSpeed = 100.f;

	UPROPERTY( EditAnywhere, DisplayName = "Lifetime" )
	float m_Lifetime = -1.f;
};
