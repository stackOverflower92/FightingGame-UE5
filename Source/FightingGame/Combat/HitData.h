#pragma once

class USoundCue;

// #TODO this should become a USTRUCT i think
struct HitData
{
    bool m_ForceOpponentFacing;
    float m_DamagePercent;
    float m_DamageHP;
    float m_Radius;
    FVector m_ProcessedKnockback;
    bool m_IgnoreKnockbackMultiplier;
    float m_HitStopDuration;
    float m_OpponentHitStopDuration;
    bool m_Shake;
    UWorld* m_World;
    AActor* m_Owner;
    USkeletalMeshComponent* m_SkeletalMesh;
    FName m_SocketToFollow;
    FVector m_Location;
    uint32 m_Id;
    int32 m_GroupId;
    int32 m_Priority;
    bool m_IsCounter;
    bool m_IgnoreBlock;
    TArray<TObjectPtr<AActor>> m_AdditionalActorsToIgnore;
    TObjectPtr<USoundBase> m_HitSound;
    bool m_PendingRemoval;

    explicit HitData( bool InForceOpponentFacing, float InDamagePercent, float InDamageHP, float InRadius, const FVector& InProcessedKnockback,
                      bool InIgnoreKnockbackMultiplier,
                      float InHitStopDuration, float InOpponentHitStopDuration, bool InShake, UWorld* InWorld, AActor* InOwner, USkeletalMeshComponent* InSkeletalMesh,
                      const FName& InSocketToFollow, FVector InLocation, uint32 InId, int32 InGroupId,
                      int32 InPriority, bool InIsCounter, bool InIgnoreBlock, const TArray<TObjectPtr<AActor>>& InAdditionalActorsToIgnore,
                      TObjectPtr<USoundBase> InHitSound );

    void GetActorsToIgnore( TArray<TObjectPtr<AActor>>& OutActors ) const
    {
        OutActors.Reset();

        OutActors.Emplace( m_Owner );
        for( int32 i = 0; i < m_AdditionalActorsToIgnore.Num(); ++i )
        {
            OutActors.Emplace( m_AdditionalActorsToIgnore[i] );
        }
    }

    friend bool operator==( const HitData& Lhs, const HitData& RHS );
    friend bool operator!=( const HitData& Lhs, const HitData& RHS );
};
