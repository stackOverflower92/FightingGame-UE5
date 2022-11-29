// Copyright (c) Giammarco Agazzotti

#include "HitboxNotifyState.h"

#include "HitboxHandlerComponent.h"
#include "FightingGame/Character/FightingCharacter.h"
#include "FightingGame/Common/CombatStatics.h"

void UHitboxNotifyState::NotifyBegin( USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration )
{
	Super::NotifyBegin( MeshComp, Animation, TotalDuration );

	if( auto* Character = Cast<AFightingCharacter>( MeshComp->GetOwner() ) )
	{
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
		                               m_SocketName,
		                               GetUniqueID() );

		Character->GetHitboxHandler()->AddHitbox( Data );
	}
}

void UHitboxNotifyState::NotifyEnd( USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation )
{
	Super::NotifyEnd( MeshComp, Animation );

	if( auto* Character = Cast<AFightingCharacter>( MeshComp->GetOwner() ) )
	{
		Character->GetHitboxHandler()->RemoveHitbox( GetUniqueID() );
	}
}
