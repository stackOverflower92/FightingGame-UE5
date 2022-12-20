#pragma once

// #TODO this should become a USTRUCT i think
struct HitData
{
	bool m_ForceOpponentFacing;
	float m_DamagePercent;
	float m_Radius;
	FVector m_ProcessedKnockback;
	bool m_IgnoreKnockbackMultiplier;
	float m_HitStopDuration;
	bool m_Shake;
	const UWorld* m_World;
	AActor* m_Owner;
	USkeletalMeshComponent* m_SkeletalMesh;
	FName m_SocketToFollow;
	FVector m_Location;
	uint32 m_Id;
	int m_GroupId;
	int m_Priority;
	TArray<TObjectPtr<AActor>> m_AdditionalActorsToIgnore;
	bool m_PendingRemoval;

	explicit HitData( bool InForceOpponentFacing, float InDamagePercent, float InRadius, const FVector& InProcessedKnockback, bool InIgnoreKnockbackMultiplier,
	                  float InHitStunDuration, bool InShake, const UWorld* InWorld, AActor* InOwner, USkeletalMeshComponent* InSkeletalMesh,
	                  const FName& InSocketToFollow,
	                  FVector InLocation, uint32 InId, int InGroupId, int InPriority, const TArray<TObjectPtr<AActor>>& InAdditionalActorsToIgnore );

	TArray<TObjectPtr<AActor>> GetActorsToIgnore() const
	
	{
		TArray<TObjectPtr<AActor>> excluded = m_AdditionalActorsToIgnore;
		excluded.Add( m_Owner );

		return excluded;
	}

	friend bool operator==( const HitData& Lhs, const HitData& RHS );
	friend bool operator!=( const HitData& Lhs, const HitData& RHS );
};
