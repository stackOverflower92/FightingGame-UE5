// Copyright (c) Giammarco Agazzotti

#include "HitboxHandlerComponent.h"

#include "GroundSensitiveEntity.h"
#include "Hittable.h"
#include "FightingGame/Common/CombatStatics.h"
#include "FightingGame/Debugging/Debug.h"
#include "FightingGame/Debugging/HitboxVisualizer.h"
#include "FightingGame/Debugging/SphereVisualizer.h"
#include "Kismet/KismetSystemLibrary.h"

namespace
{
    int32 loc_ShowHitboxTraces = 0;
    FG_CVAR_FLAG_DESC( CVarShowHitboxTraces, TEXT( "HitboxHandlerComponent.ShowHitboxTraces" ), loc_ShowHitboxTraces );
}

UHitboxHandlerComponent::UHitboxHandlerComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UHitboxHandlerComponent::BeginPlay()
{
    Super::BeginPlay();

    if( m_SpawnDefaultHitboxesOnBeginPlay )
    {
        SpawnDefaultHitboxes();
    }

    if( !m_HitboxVisualizer )
    {
        FG_SLOG_WARN( FString::Printf( TEXT( "HitboxVisualizer is null, hiboxes will not be shown for actor [%s]" ), *GetOwner()->GetName()) );
    }
}

void UHitboxHandlerComponent::EndPlay( const EEndPlayReason::Type EndPlayReason )
{
    Super::EndPlay( EndPlayReason );

    for( const auto& tuple : m_ActiveGroupedHitboxes )
    {
        for( const auto& hit : tuple.Value )
        {
            DEBUG_DestroyDebugSphere( hit.m_Id );
        }
    }
}

void UHitboxHandlerComponent::SetReferenceComponent( TObjectPtr<USceneComponent> Component )
{
    m_ReferenceComponent = Component;
}

void UHitboxHandlerComponent::AddHitbox( HitData Hit )
{
    if( m_ActiveGroupedHitboxes.Contains( Hit.m_GroupId ) )
    {
        m_ActiveGroupedHitboxes[Hit.m_GroupId].AddUnique( Hit );

        // #TODO temp, this is expensive
        m_ActiveGroupedHitboxes[Hit.m_GroupId].Sort( []( const HitData& A, const HitData& B )
        {
            return A.m_Priority < B.m_Priority;
        } );
    }
    else
    {
        m_ActiveGroupedHitboxes.Add( Hit.m_GroupId, {Hit} );
    }

    if( loc_ShowHitboxTraces && m_HitboxVisualizer )
    {
        DEBUG_SpawnDebugSphere( Hit );
    }
}

void UHitboxHandlerComponent::RemoveHitbox( uint32 HitUniqueId )
{
    for( auto& tuple : m_ActiveGroupedHitboxes )
    {
        auto* it = tuple.Value.FindByPredicate( [&HitUniqueId]( const HitData& HitData )
        {
            return HitData.m_Id == HitUniqueId;
        } );

        if( it )
        {
            it->m_PendingRemoval = true;
        }
    }
}

void UHitboxHandlerComponent::UpdateHitboxes()
{
    for( const auto& tuple : m_ActiveGroupedHitboxes )
    {
        for( int hitIdx = 0; hitIdx < tuple.Value.Num(); ++hitIdx )
        {
            const auto& hit = tuple.Value[hitIdx];
            if( !hit.m_PendingRemoval )
            {
                UpdateHitbox( hit );
            }
        }
    }
}

void UHitboxHandlerComponent::ShowDebugTraces( bool Show )
{
    m_DebugTraces = Show;
}

void UHitboxHandlerComponent::SpawnDefaultHitboxes()
{
    for( int i = 0; i < m_DefaultHitboxes.Num(); ++i )
    {
        AddHitbox( UCombatStatics::GenerateHitDataFromHitboxDescription( GetOwner(), nullptr, m_DefaultHitboxes[i],
                                                                         i, GetOwner()->GetUniqueID(), m_AdditionalActorsToIgnore ) );
    }
}

void UHitboxHandlerComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
    Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

    UpdateHitboxes();

    DEBUG_UpdateDebugSpheres();

    RemovePendingHitboxes();
}

bool UHitboxHandlerComponent::TraceHitbox( const HitData& HitData, FHitResult& OutHit, ECollisionChannel CollisionChannel /*= CUSTOM_TRACE_HURTBOX*/ )
{
    TArray<TEnumAsByte<EObjectTypeQuery>> targetTraceTypes;

    const EObjectTypeQuery targetCollisionType = UEngineTypes::ConvertToObjectType( CollisionChannel );
    targetTraceTypes.Add( targetCollisionType );

    FVector location = GetHitTraceLocation( HitData );

    TArray<TObjectPtr<AActor>> actorsToIgnore;
    HitData.GetActorsToIgnore( actorsToIgnore );

    bool didHit = UKismetSystemLibrary::SphereTraceSingleForObjects( HitData.m_World, location, location, HitData.m_Radius, targetTraceTypes,
                                                                     false, actorsToIgnore, EDrawDebugTrace::None, OutHit, true );

    return didHit;
}

bool UHitboxHandlerComponent::WasActorAlreadyHit( AActor* Actor, const HitData& Hit )
{
    ensureMsgf( Actor, TEXT("Actor is null") );

    if( !m_ActorGroupsMap.Find( Actor->GetUniqueID() ) ) return false;

    return m_ActorGroupsMap[Actor->GetUniqueID()].ContainsByPredicate( [&]( const FHitGroupPair _pair )
    {
        return _pair.m_GroupId == Hit.m_GroupId;
    } );
}

void UHitboxHandlerComponent::RegisterHitActor( AActor* Actor, const HitData& Hit )
{
    ensureMsgf( Actor, TEXT("Actor is null") );

    if( m_ActorGroupsMap.Contains( Actor->GetUniqueID() ) )
    {
        m_ActorGroupsMap[Actor->GetUniqueID()].AddUnique( FHitGroupPair{Hit.m_Id, Hit.m_GroupId} );
    }
    else
    {
        TArray groupsArray = {FHitGroupPair{Hit.m_Id, Hit.m_GroupId}};
        m_ActorGroupsMap.Emplace( Actor->GetUniqueID(), groupsArray );
    }
}

void UHitboxHandlerComponent::UpdateHitbox( const HitData& HitData )
{
    // #TODO check clang

    FHitResult outHurtboxHit;
    bool didHitHurtbox = TraceHitbox( HitData, outHurtboxHit );

    TObjectPtr<AActor> hitActor = outHurtboxHit.GetActor();
    if( auto* hittable = Cast<IHittable>( hitActor ) )
    {
        if( auto* groundSensitiveEntity = Cast<IGroundSensitiveEntity>( hitActor ) )
        {
            bool groundedCheckFailed = HitData.m_AllowOnGroundedOpponents && !groundSensitiveEntity->IsGrounded();
            bool airborneCheckFailed = HitData.m_AllowOnAirborneOpponents && !groundSensitiveEntity->IsAirborne();

            if( groundedCheckFailed && airborneCheckFailed )
            {
                return;
            }
        }

        if( hittable->IsHittable() )
        {
            if( didHitHurtbox && !WasActorAlreadyHit( hitActor, HitData ) )
            {
                RegisterHitActor( hitActor, HitData );

                hittable->OnHitReceived( HitData );
                m_HitDelegate.Broadcast( hitActor, HitData );

                if( TObjectPtr<AHitboxVisualizer> visualizer = DEBUG_GetHitboxVisualizerOrDefault( HitData.m_Id ) )
                {
                    if( hittable->IsBlocking() )
                    {
                        if( HitData.m_IgnoreBlock )
                        {
                            visualizer->SetHitState();
                        }
                        else
                        {
                            visualizer->SetBlockedState();
                        }
                    }
                    else
                    {
                        visualizer->SetHitState();
                    }
                }
            }
        }
    }
}

void UHitboxHandlerComponent::RemovePendingHitboxes()
{
    for( auto& tuple : m_ActiveGroupedHitboxes )
    {
        for( int i = tuple.Value.Num() - 1; i >= 0; --i )
        {
            if( tuple.Value[i].m_PendingRemoval )
            {
                TArray<uint32> targetKeys;
                for( const auto& pair : m_ActorGroupsMap )
                {
                    if( pair.Value.ContainsByPredicate( [&]( const FHitGroupPair& hitPair )
                    {
                        return hitPair.m_Id == tuple.Value[i].m_Id;
                    } ) )
                    {
                        targetKeys.Emplace( pair.Key );
                    }
                }

                for( int keyIdx = 0; keyIdx < targetKeys.Num(); ++keyIdx )
                {
                    uint32 targetKey = targetKeys[keyIdx];

                    for( int j = m_ActorGroupsMap[targetKey].Num() - 1; j >= 0; --j )
                    {
                        if( m_ActorGroupsMap[targetKey][j].m_Id == tuple.Value[i].m_Id )
                        {
                            m_ActorGroupsMap[targetKey].RemoveAt( j );
                        }
                    }
                }

                DEBUG_DestroyDebugSphere( tuple.Value[i].m_Id );

                tuple.Value.RemoveAt( i );
            }
        }
    }
}

FVector UHitboxHandlerComponent::GetHitTraceLocation( const HitData& Hit )
{
    bool hasSocketToFollow = Hit.m_SkeletalMesh ? (!Hit.m_SocketToFollow.ToString().IsEmpty()) : false;

    if( hasSocketToFollow )
    {
        FVector socketLocation = Hit.m_SkeletalMesh->GetSocketLocation( Hit.m_SocketToFollow );
        socketLocation.X       = Hit.m_Owner->GetActorLocation().X;

        return socketLocation;
    }

    return Hit.m_Owner->GetActorLocation() + Hit.m_Location;
}

void UHitboxHandlerComponent::DEBUG_SpawnDebugSphere( const HitData& Hit )
{
    if( m_HitboxVisualizer )
    {
        TObjectPtr<AHitboxVisualizer> inst = Hit.m_World->SpawnActor<AHitboxVisualizer>( m_HitboxVisualizer );

        inst->SetId( Hit.m_Id );
        inst->SetRadius( Hit.m_Radius );
        inst->SetVisualizerOwner( Hit.m_Owner );
        inst->SetKnockback( Hit.m_ProcessedKnockback );

        if( Hit.m_SkeletalMesh )
        {
            if( m_ReferenceComponent )
            {
                inst->AttachToComponent( m_ReferenceComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale, Hit.m_SocketToFollow );
            }
        }
        else
        {
            inst->SetLocation( Hit.m_Location );
        }

        m_HitboxVisualizers.Emplace( inst );
    }
}

TObjectPtr<AHitboxVisualizer> UHitboxHandlerComponent::DEBUG_GetHitboxVisualizerOrDefault( int HitboxId )
{
    auto* it = m_HitboxVisualizers.FindByPredicate( [&HitboxId]( TObjectPtr<AHitboxVisualizer> _vis )
    {
        return _vis->GetId() == HitboxId;
    } );

    return it ? *it : nullptr;
}

void UHitboxHandlerComponent::DEBUG_DestroyDebugSphere( int HitboxId )
{
    for( int visIdx = m_HitboxVisualizers.Num() - 1; visIdx >= 0; --visIdx )
    {
        if( HitboxId == m_HitboxVisualizers[visIdx]->GetId() )
        {
            GetWorld()->DestroyActor( m_HitboxVisualizers[visIdx] );
            m_HitboxVisualizers.RemoveAt( visIdx );
        }
    }
}

void UHitboxHandlerComponent::DEBUG_UpdateDebugSpheres()
{
    for( TObjectPtr<ASphereVisualizer> visualizer : m_HitboxVisualizers )
    {
        if( visualizer->GetLocation().IsSet() && visualizer->GetVisualizerOwner() )
        {
            visualizer->SetActorLocation( visualizer->GetVisualizerOwner()->GetActorLocation() + visualizer->GetLocation().GetValue() );
        }
    }
}
