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
			const HitData& Data = UCombatStatics::GenerateHitDataFromHitboxDescription( character, MeshComp, hitboxDesc );
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
