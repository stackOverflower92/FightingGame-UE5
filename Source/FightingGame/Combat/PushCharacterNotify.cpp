// Copyright (c) Giammarco Agazzotti


#include "PushCharacterNotify.h"

#include "FightingGame/Character/FightingCharacter.h"
#include "FightingGame/Common/CombatStatics.h"

void UPushCharacterNotify::Notify( USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference )
{
	Super::Notify( MeshComp, Animation, EventReference );

	if( auto* Character = Cast<AFightingCharacter>( MeshComp->GetOwner() ) )
	{
		FVector Knockback = UCombatStatics::GetKnockbackFromOrientation( Character, m_Orientation );
		Character->LaunchCharacter( Knockback.GetSafeNormal() * m_Force, true, true );
	}
}
