// Copyright (c) Giammarco Agazzotti

#include "HitboxNotifyState.h"

#include "HitboxHandlerComponent.h"
#include "FightingGame/Character/FightingCharacter.h"

void UHitboxNotifyState::NotifyBegin( USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration )
{
	Super::NotifyBegin( MeshComp, Animation, TotalDuration );

	if( auto* Character = Cast<AFightingCharacter>( MeshComp->GetOwner() ) )
	{
		m_CurrentHitData = new FHitData( m_ForceOpponentFacing,
		                                 m_DamagePercent,
		                                 m_Radius,
		                                 m_KnockbackOrientation,
		                                 m_IgnoreKnockbackMultiplier,
		                                 m_HitStunDuration,
		                                 m_Shake,
		                                 MeshComp->GetWorld(),
		                                 Character,
		                                 MeshComp,
		                                 m_SocketName,
		                                 GetUniqueID() );

		Character->GetHitboxHandler()->AddHitbox( *m_CurrentHitData );
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
