#include "HitData.h"

HitData::HitData( bool InForceOpponentFacing, float InDamagePercent, float InDamageHP, float InRadius, const FVector& InProcessedKnockback,
                  float InKnockbackAngleDeg, bool InIgnoreKnockbackMultiplier,
                  float InHitStopDuration, float InOpponentHitStopDuration, bool InShake, UWorld* InWorld, AActor* InOwner, USkeletalMeshComponent* InSkeletalMesh,
                  const FName& InSocketToFollow, FVector InLocation, uint32 InId, int32 InGroupId,
                  int32 InPriority, bool InIsCounter, bool InIgnoreBlock, const TArray<TObjectPtr<AActor>>& InAdditionalActorsToIgnore,
                  TObjectPtr<USoundBase> InHitSound, bool InCanClang, int32 InClangPriority, bool InAllowOnGroundedOpponents, bool InAllowOnAirborneOpponents )
    : m_ForceOpponentFacing( InForceOpponentFacing ),
      m_DamagePercent( InDamagePercent ),
      m_DamageHP( InDamageHP ),
      m_Radius( InRadius ),
      m_ProcessedKnockback( InProcessedKnockback ),
      m_KnockbackAngleDeg( InKnockbackAngleDeg ),
      m_IgnoreKnockbackMultiplier( InIgnoreKnockbackMultiplier ),
      m_HitStopDuration( InHitStopDuration ),
      m_OpponentHitStopDuration( InOpponentHitStopDuration ),
      m_Shake( InShake ),
      m_World( InWorld ),
      m_Owner( InOwner ),
      m_SkeletalMesh( InSkeletalMesh ),
      m_SocketToFollow( InSocketToFollow ),
      m_Location( InLocation ),
      m_Id( InId ),
      m_GroupId( InGroupId ),
      m_Priority( InPriority ),
      m_IsCounter( InIsCounter ),
      m_IgnoreBlock( InIgnoreBlock ),
      m_AdditionalActorsToIgnore( InAdditionalActorsToIgnore ),
      m_HitSound( InHitSound ),
      m_CanClang( InCanClang ),
      m_ClangPriority( InClangPriority ),
      m_AllowOnGroundedOpponents( InAllowOnGroundedOpponents ),
      m_AllowOnAirborneOpponents( InAllowOnAirborneOpponents ),
      m_PendingRemoval( false )
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
