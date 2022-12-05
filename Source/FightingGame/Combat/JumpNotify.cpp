// Copyright (c) Giammarco Agazzotti


#include "JumpNotify.h"

#include "FightingGame/Character/FightingCharacter.h"

void UJumpNotify::Notify( USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference )
{
	Super::Notify( MeshComp, Animation, EventReference );

	if( AFightingCharacter* Character = Cast<AFightingCharacter>( MeshComp->GetOwner() ) )
	{
		Character->Jump();
	}
}
