#pragma once

struct FHitData
{
	bool m_ForceOpponentFacing = true;
	float m_DamagePercent = 0.f;
	float m_Radius = 10.f;
	float m_KnockbackOrientation = 0.f;
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

	FHitData( bool InForceOpponentFacing, float InDamagePercent, float InRadius, float InKnockbackOrientation, bool InIgnoreKnockbackMultiplier, float InHitStunDuration,
	          bool InShake, const UWorld* InWorld, AActor* InOwner, USkeletalMeshComponent* InSkeletalMesh, const FName& InSocketToFollow, uint32 InId );

	FHitData( bool InForceOpponentFacing, float InDamagePercent, float InRadius, float InKnockbackOrientation, bool InIgnoreKnockbackMultiplier, float InHitStunDuration,
	          bool InShake, const UWorld* InWorld, AActor* InOwner, FVector InLocation, uint32 InId );

	friend bool operator==( const FHitData& Lhs, const FHitData& RHS );
	friend bool operator!=( const FHitData& Lhs, const FHitData& RHS );
};
