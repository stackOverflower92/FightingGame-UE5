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
		for( int i = 0; i < m_HitBoxes.Num(); ++i )
		{
			const HitData& Data = UCombatStatics::GenerateHitDataFromHitboxDescription( character, MeshComp, m_HitBoxes[i], i, GetUniqueID() );
			character->GetHitboxHandler()->AddHitbox( Data );
		}
	}
}

void UHitboxNotifyState::NotifyEnd( USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference )
{
	Super::NotifyEnd( MeshComp, Animation, EventReference );

	if( auto* character = Cast<AFightingCharacter>( MeshComp->GetOwner() ) )
	{
		for( int i = 0; i < m_HitBoxes.Num(); ++i )
		{
			character->GetHitboxHandler()->RemoveHitbox( i );
		}
	}
}
