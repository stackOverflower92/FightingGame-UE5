#include "HitData.h"

FHitData::FHitData( bool InForceOpponentFacing, float InDamagePercent, float InRadius, float InKnockbackOrientation, bool InIgnoreKnockbackMultiplier,
                    float InHitStunDuration, bool InShake, const UWorld* InWorld, AActor* InOwner, USkeletalMeshComponent* InSkeletalMesh, const
                    FName& InSocketToFollow,
                    uint32 InId )
	: m_ForceOpponentFacing( InForceOpponentFacing ),
	  m_DamagePercent( InDamagePercent ),
	  m_Radius( InRadius ),
	  m_KnockbackOrientation( InKnockbackOrientation ),
	  m_IgnoreKnockbackMultiplier( InIgnoreKnockbackMultiplier ),
	  m_HitStunDuration( InHitStunDuration ),
	  m_Shake( InShake ),
	  m_World( InWorld ),
	  m_Owner( InOwner ),
	  m_SkeletalMesh( InSkeletalMesh ),
	  m_SocketToFollow( InSocketToFollow ),
	  m_Id( InId )
{
}

FHitData::FHitData( bool InForceOpponentFacing, float InDamagePercent, float InRadius, float InKnockbackOrientation, bool InIgnoreKnockbackMultiplier,
                    float InHitStunDuration, bool InShake, const UWorld* InWorld, AActor* InOwner, FVector InLocation, uint32 InId )
	: m_ForceOpponentFacing( InForceOpponentFacing ),
	  m_DamagePercent( InDamagePercent ),
	  m_Radius( InRadius ),
	  m_KnockbackOrientation( InKnockbackOrientation ),
	  m_IgnoreKnockbackMultiplier( InIgnoreKnockbackMultiplier ),
	  m_HitStunDuration( InHitStunDuration ),
	  m_Shake( InShake ),
	  m_World( InWorld ),
	  m_Owner( InOwner ),
	  m_Location( InLocation ),
	  m_Id( InId )
{
}

bool operator==( const FHitData& Lhs, const FHitData& RHS )
{
	return Lhs.m_Id == RHS.m_Id;
}

bool operator!=( const FHitData& Lhs, const FHitData& RHS )
{
	return !(Lhs == RHS);
}
