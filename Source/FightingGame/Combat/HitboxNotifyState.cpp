// Copyright (c) Giammarco Agazzotti

#include "HitboxNotifyState.h"

#include "HitboxHandlerComponent.h"
#include "FightingGame/Character/FightingCharacter.h"
#include "FightingGame/Common/CombatStatics.h"

void UHitboxNotifyState::NotifyBegin( USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration,
                                      const FAnimNotifyEventReference& EventReference )
{
	Super::NotifyBegin( MeshComp, Animation, TotalDuration, EventReference );

	if( auto* Character = Cast<AFightingCharacter>( MeshComp->GetOwner() ) )
	{
		FName SocketName = Character->IsFacingRight() ? m_SocketName : m_SocketNameMirrored;

		const HitData& Data = HitData( m_ForceOpponentFacing,
		                               m_DamagePercent,
		                               m_Radius,
		                               UCombatStatics::GetKnockbackFromOrientation( Character, m_KnockbackOrientation ) * m_KnockbackForce,
		                               m_IgnoreKnockbackMultiplier,
		                               m_HitStunDuration,
		                               m_Shake,
		                               MeshComp->GetWorld(),
		                               Character,
		                               MeshComp,
		                               SocketName,
		                               GetUniqueID() );

		Character->GetHitboxHandler()->AddHitbox( Data );
	}
}

void UHitboxNotifyState::NotifyEnd( USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference )
{
	Super::NotifyEnd( MeshComp, Animation, EventReference );

	if( auto* Character = Cast<AFightingCharacter>( MeshComp->GetOwner() ) )
	{
		Character->GetHitboxHandler()->RemoveHitbox( GetUniqueID() );
	}
}
