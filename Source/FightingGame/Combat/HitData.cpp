#include "HitData.h"

HitData::HitData( bool InForceOpponentFacing, float InDamagePercent, float InRadius, const FVector& InProcessedKnockback, bool InIgnoreKnockbackMultiplier,
                  float InHitStunDuration, bool InShake, const UWorld* InWorld, AActor* InOwner, USkeletalMeshComponent* InSkeletalMesh, const FName& InSocketToFollow,
                  uint32 InId, int InGroupId, int InPriority )
	: m_ForceOpponentFacing( InForceOpponentFacing ),
	  m_DamagePercent( InDamagePercent ),
	  m_Radius( InRadius ),
	  m_ProcessedKnockback( InProcessedKnockback ),
	  m_IgnoreKnockbackMultiplier( InIgnoreKnockbackMultiplier ),
	  m_HitStopDuration( InHitStunDuration ),
	  m_Shake( InShake ),
	  m_World( InWorld ),
	  m_Owner( InOwner ),
	  m_SkeletalMesh( InSkeletalMesh ),
	  m_SocketToFollow( InSocketToFollow ),
	  m_Id( InId ),
	  m_GroupId( InGroupId ),
	  m_Priority( InPriority )
{
}

HitData::HitData( bool InForceOpponentFacing, float InDamagePercent, float InRadius, const FVector& InProcessedKnockback, bool InIgnoreKnockbackMultiplier,
                  float InHitStunDuration, bool InShake, const UWorld* InWorld, AActor* InOwner, FVector InLocation, uint32 InId, int InGroupId, int InPriority )
	: m_ForceOpponentFacing( InForceOpponentFacing ),
	  m_DamagePercent( InDamagePercent ),
	  m_Radius( InRadius ),
	  m_ProcessedKnockback( InProcessedKnockback ),
	  m_IgnoreKnockbackMultiplier( InIgnoreKnockbackMultiplier ),
	  m_HitStopDuration( InHitStunDuration ),
	  m_Shake( InShake ),
	  m_World( InWorld ),
	  m_Owner( InOwner ),
	  m_Location( InLocation ),
	  m_Id( InId ),
	  m_GroupId( InGroupId ),
	  m_Priority( InPriority )
{
}

bool operator==( const HitData& Lhs, const HitData& RHS )
{
	return Lhs.m_Id == RHS.m_Id;
}

bool operator!=( const HitData& Lhs, const HitData& RHS )
{
	return !(Lhs == RHS);
}
