// Copyright (c) Giammarco Agazzotti


#include "CounterNotifyState.h"

#include "CounterEntity.h"

void UCounterNotifyState::NotifyBegin( USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration,
                                       const FAnimNotifyEventReference& EventReference )
{
    Super::NotifyBegin( MeshComp, Animation, TotalDuration, EventReference );

    if( TObjectPtr<ICounterEntity> counterEntity = Cast<ICounterEntity>( MeshComp->GetOwner() ) )
    {
        counterEntity->SetIsCountering( true );
    }
}

void UCounterNotifyState::NotifyEnd( USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference )
{
    Super::NotifyEnd( MeshComp, Animation, EventReference );

    if( TObjectPtr<ICounterEntity> counterEntity = Cast<ICounterEntity>( MeshComp->GetOwner() ) )
    {
        counterEntity->SetIsCountering( false );
    }
}
