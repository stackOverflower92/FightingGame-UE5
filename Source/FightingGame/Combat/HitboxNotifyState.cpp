// Copyright (c) Giammarco Agazzotti

#include "HitboxNotifyState.h"

#include "HitboxHandlerComponent.h"
#include "FightingGame/Character/FightingCharacter.h"
#include "FightingGame/Common/CombatStatics.h"
#include "FightingGame/Combat/HitboxDescription.h"

void UHitboxNotifyState::NotifyBegin( USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration,
                                      const FAnimNotifyEventReference& EventReference )
{
	Super::NotifyBegin( MeshComp, Animation, TotalDuration, EventReference );

	if( auto* character = Cast<AFightingCharacter>( MeshComp->GetOwner() ) )
	{
		for( const FHitboxDescription& hitboxDesc : m_HitBoxes )
		{
			FName socketName = character->IsFacingRight() ? hitboxDesc.m_SocketName : hitboxDesc.m_SocketNameMirrored;

			const HitData& Data = HitData( hitboxDesc.m_ForceOpponentFacing,
			                               hitboxDesc.m_DamagePercent,
			                               hitboxDesc.m_Radius,
			                               UCombatStatics::GetKnockbackFromOrientation( character, hitboxDesc.m_KnockbackOrientation ) * hitboxDesc.m_KnockbackForce,
			                               hitboxDesc.m_IgnoreKnockbackMultiplier,
			                               hitboxDesc.m_HitStopDuration,
			                               hitboxDesc.m_Shake,
			                               MeshComp->GetWorld(),
			                               character,
			                               MeshComp,
			                               socketName,
			                               hitboxDesc.m_UniqueId,
			                               hitboxDesc.m_GroupId,
			                               hitboxDesc.m_Priority );

			character->GetHitboxHandler()->AddHitbox( Data );
		}
	}
}

void UHitboxNotifyState::NotifyEnd( USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference )
{
	Super::NotifyEnd( MeshComp, Animation, EventReference );

	if( auto* character = Cast<AFightingCharacter>( MeshComp->GetOwner() ) )
	{
		for( const FHitboxDescription& hitboxDesc : m_HitBoxes )
		{
			character->GetHitboxHandler()->RemoveHitbox( hitboxDesc.m_UniqueId );
		}
	}
}
