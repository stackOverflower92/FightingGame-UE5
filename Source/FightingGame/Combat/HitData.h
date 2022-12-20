#pragma once

// #TODO this should become a USTRUCT i think
struct HitData
{
	bool m_ForceOpponentFacing = true;
	float m_DamagePercent      = 0.f;
	float m_Radius             = 10.f;
	FVector m_ProcessedKnockback;
	bool m_IgnoreKnockbackMultiplier = false;
	float m_HitStopDuration          = 0.f;
	bool m_Shake                     = false;
	const UWorld* m_World            = nullptr;
	AActor* m_Owner                  = nullptr;
	TArray<TObjectPtr<AActor>> m_AdditionalActorsToIgnore;
	FVector m_Location;
	USkeletalMeshComponent* m_SkeletalMesh = nullptr;
	FName m_SocketToFollow                 = "";
	uint32 m_Id;
	int m_GroupId;
	int m_Priority;
	bool m_PendingRemoval = false;

	HitData( bool InForceOpponentFacing, float InDamagePercent, float InRadius, const FVector& InProcessedKnockback, bool InIgnoreKnockbackMultiplier,
	         float InHitStunDuration, bool InShake, const UWorld* InWorld, AActor* InOwner, USkeletalMeshComponent* InSkeletalMesh, const FName& InSocketToFollow,
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
