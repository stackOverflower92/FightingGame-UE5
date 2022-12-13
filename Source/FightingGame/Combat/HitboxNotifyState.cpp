// Copyright (c) Giammarco Agazzotti

#include "HitboxNotifyState.h"

#include "HitboxHandlerComponent.h"
#include "FightingGame/Character/FightingCharacter.h"
#include "FightingGame/Common/CombatStatics.h"

void UHitboxNotifyState::NotifyBegin( USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration,
                                      const FAnimNotifyEventReference& EventReference )
{
	Super::NotifyBegin( MeshComp, Animation, TotalDuration, EventReference );

	if( auto* character = Cast<AFightingCharacter>( MeshComp->GetOwner() ) )
	{
		for( const FHitboxNotifyData& notifyData : m_HitBoxes )
		{
			FName socketName = character->IsFacingRight() ? notifyData.m_SocketName : notifyData.m_SocketNameMirrored;

			const HitData& Data = HitData( notifyData.m_ForceOpponentFacing,
			                               notifyData.m_DamagePercent,
			                               notifyData.m_Radius,
			                               UCombatStatics::GetKnockbackFromOrientation( character, notifyData.m_KnockbackOrientation ) * notifyData.m_KnockbackForce,
			                               notifyData.m_IgnoreKnockbackMultiplier,
			                               notifyData.m_HitStopDuration,
			                               notifyData.m_Shake,
			                               MeshComp->GetWorld(),
			                               character,
			                               MeshComp,
			                               socketName,
			                               notifyData.m_UniqueId,
			                               notifyData.m_GroupId,
			                               notifyData.m_Priority );

			character->GetHitboxHandler()->AddHitbox( Data );
		}
	}
}

void UHitboxNotifyState::NotifyEnd( USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference )
{
	Super::NotifyEnd( MeshComp, Animation, EventReference );

	if( auto* character = Cast<AFightingCharacter>( MeshComp->GetOwner() ) )
	{
		for( const FHitboxNotifyData& notifyData : m_HitBoxes )
		{
			character->GetHitboxHandler()->RemoveHitbox( notifyData.m_UniqueId );
		}
	}
}
