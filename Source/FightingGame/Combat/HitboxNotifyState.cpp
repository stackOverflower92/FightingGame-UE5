// Copyright (c) Giammarco Agazzotti

#include "HitboxNotifyState.h"

#include "HitboxDescriptionDataAsset.h"
#include "HitboxHandlerComponent.h"
#include "FightingGame/Character/FightingCharacter.h"
#include "FightingGame/Common/CombatStatics.h"
#include "FightingGame/Debugging/Debug.h"

void UHitboxNotifyState::NotifyBegin( USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration,
                                      const FAnimNotifyEventReference& EventReference )
{
    Super::NotifyBegin( MeshComp, Animation, TotalDuration, EventReference );

    if( auto* character = Cast<AFightingCharacter>( MeshComp->GetOwner() ) )
    {
        TArray<FHitboxDescription> aggregatedDescriptions;

        for( int32 i = 0; i < m_HitBoxes.Num(); ++i )
        {
            aggregatedDescriptions.Emplace( m_HitBoxes[i] );
        }

        for( int32 i = 0; i < m_HitboxesAssets.Num(); ++i )
        {
            if( !m_HitboxesAssets[i] )
            {
                FG_SLOG_ERR( "There is a null hitbox description data asset, will be skipped" );
                continue;
            }

            aggregatedDescriptions.Emplace( m_HitboxesAssets[i]->m_HitboxDescription );
        }

        for( int i = 0; i < aggregatedDescriptions.Num(); ++i )
        {
            const HitData& Data = UCombatStatics::GenerateHitDataFromHitboxDescription( character, MeshComp, aggregatedDescriptions[i], i, GetUniqueID() );
            character->GetHitboxHandler()->AddHitbox( Data );
        }
    }
}

void UHitboxNotifyState::NotifyEnd( USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference )
{
    Super::NotifyEnd( MeshComp, Animation, EventReference );

    if( auto* character = Cast<AFightingCharacter>( MeshComp->GetOwner() ) )
    {
        TArray<FHitboxDescription> aggregatedDescriptions;

        for( int32 i = 0; i < m_HitBoxes.Num(); ++i )
        {
            aggregatedDescriptions.Emplace( m_HitBoxes[i] );
        }

        for( int32 i = 0; i < m_HitboxesAssets.Num(); ++i )
        {
            if( m_HitboxesAssets[i] )
            {
                aggregatedDescriptions.Emplace( m_HitboxesAssets[i]->m_HitboxDescription );
            }
        }

        for( int i = 0; i < aggregatedDescriptions.Num(); ++i )
        {
            character->GetHitboxHandler()->RemoveHitbox( i );
        }
    }
}
