// Copyright (c) Giammarco Agazzotti


#include "PushCharacterNotify.h"

#include "FightingGame/Character/FightingCharacter.h"
#include "FightingGame/Common/CombatStatics.h"
#include "FightingGame/Debugging/Debug.h"

void UPushCharacterNotify::Notify( USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference )
{
	Super::Notify( MeshComp, Animation, EventReference );

	if( auto* character = Cast<AFightingCharacter>( MeshComp->GetOwner() ) )
	{
		if( auto* facingEntity = Cast<IFacingEntity>( character ) )
		{
			FVector Knockback = UCombatStatics::GetKnockbackFromOrientation( facingEntity, m_Orientation );
			character->LaunchCharacter( Knockback.GetSafeNormal() * m_Force, true, true );
		}
		else
		{
			FG_SLOG_ERR( TEXT("Cannot convert character to IFacingEntity") );
		}
	}
}
