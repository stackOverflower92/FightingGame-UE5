// Copyright (c) Giammarco Agazzotti


#include "InvincibilityNotifyState.h"

#include "FightingGame/Character/FightingCharacter.h"

void UInvincibilityNotifyState::NotifyBegin( USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration,
                                             const FAnimNotifyEventReference& EventReference )
{
	Super::NotifyBegin( MeshComp, Animation, TotalDuration, EventReference );

	if( auto* Character = Cast<AFightingCharacter>( MeshComp->GetOwner() ) )
	{
		Character->SetHittable( false );
	}
}

void UInvincibilityNotifyState::NotifyEnd( USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference )
{
	Super::NotifyEnd( MeshComp, Animation, EventReference );

	if( auto* Character = Cast<AFightingCharacter>( MeshComp->GetOwner() ) )
	{
		Character->SetHittable( true );
	}
}
