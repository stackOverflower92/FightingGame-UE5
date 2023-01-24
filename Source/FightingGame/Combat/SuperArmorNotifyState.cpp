// Copyright (c) Giammarco Agazzotti


#include "SuperArmorNotifyState.h"

#include "Hittable.h"
#include "SuperArmorData.h"

void USuperArmorNotifyState::NotifyBegin( USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration,
                                          const FAnimNotifyEventReference& EventReference )
{
    Super::NotifyBegin( MeshComp, Animation, TotalDuration, EventReference );

    if( auto* hittable = Cast<IHittable>( MeshComp->GetOwner() ) )
    {
        hittable->EnableSuperArmor( true, SuperArmorData{m_DamagePercentMultiplier, m_DamageMultiplier} );
    }
}

void USuperArmorNotifyState::NotifyEnd( USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference )
{
    Super::NotifyEnd( MeshComp, Animation, EventReference );

    if( auto* hittable = Cast<IHittable>( MeshComp->GetOwner() ) )
    {
        hittable->EnableSuperArmor( false, SuperArmorData{m_DamagePercentMultiplier, m_DamageMultiplier} );
    }
}
