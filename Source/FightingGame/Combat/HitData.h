#pragma once

struct HitData
{
	bool m_ForceOpponentFacing = true;
	float m_DamagePercent = 0.f;
	float m_Radius = 10.f;
	FVector m_ProcessedKnockback;
	bool m_IgnoreKnockbackMultiplier = false;
	float m_HitStunDuration = 0.f;
	bool m_Shake = false;
	const UWorld* m_World = nullptr;
	AActor* m_Owner = nullptr;
	FVector m_Location;
	USkeletalMeshComponent* m_SkeletalMesh = nullptr;
	FName m_SocketToFollow = "";
	uint32 m_Id;
	bool m_PendingRemoval = false;

	HitData( bool InForceOpponentFacing, float InDamagePercent, float InRadius, const FVector& InProcessedKnockback, bool InIgnoreKnockbackMultiplier, float InHitStunDuration,
	         bool InShake, const UWorld* InWorld, AActor* InOwner, USkeletalMeshComponent* InSkeletalMesh, const FName& InSocketToFollow, uint32 InId );

	HitData( bool InForceOpponentFacing, float InDamagePercent, float InRadius, const FVector& InProcessedKnockback, bool InIgnoreKnockbackMultiplier, float InHitStunDuration,
	         bool InShake, const UWorld* InWorld, AActor* InOwner, FVector InLocation, uint32 InId );

	friend bool operator==( const HitData& Lhs, const HitData& RHS );
	friend bool operator!=( const HitData& Lhs, const HitData& RHS );
};
