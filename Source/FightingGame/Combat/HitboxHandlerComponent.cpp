// Copyright (c) Giammarco Agazzotti

#include "HitboxHandlerComponent.h"
#include "Hittable.h"
#include "FightingGame/Collision/CustomCollisionChannels.h"
#include "FightingGame/Common/CombatStatics.h"
#include "FightingGame/Debug/Debug.h"
#include "FightingGame/Debug/SphereVisualizer.h"
#include "Kismet/KismetSystemLibrary.h"

namespace
{
	int32 loc_ShowHitboxTraces = 0;
	FG_CVAR_DESC( CVarShowHitboxTraces, TEXT( "HitboxHandlerComponent.ShowHitboxTraces" ), TEXT("1: Enable, 0: Disable"), loc_ShowHitboxTraces );
}

UHitboxHandlerComponent::UHitboxHandlerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UHitboxHandlerComponent::BeginPlay()
{
	Super::BeginPlay();

	for( const auto& hitboxDesc : m_DefaultHitboxes )
	{
		AddHitbox( UCombatStatics::GenerateHitDataFromHitboxDescription( GetOwner(), nullptr, hitboxDesc ) );
	}
}

void UHitboxHandlerComponent::SetReferenceComponent( TObjectPtr<USceneComponent> Component )
{
	m_ReferenceComponent = Component;
}

void UHitboxHandlerComponent::AddHitbox( HitData Hit )
{
	m_ActiveHitboxes.AddUnique( Hit );

	if( loc_ShowHitboxTraces && m_HitboxVisualizer )
	{
		DEBUG_SpawnDebugSphere( Hit );
	}
}

void UHitboxHandlerComponent::RemoveHitbox( uint32 HitUniqueId )
{
	auto* it = m_ActiveHitboxes.FindByPredicate( [&HitUniqueId]( const HitData& HitData )
	{
		return HitData.m_Id == HitUniqueId;
	} );

	if( it )
	{
		it->m_PendingRemoval = true;
	}
}

void UHitboxHandlerComponent::ShowDebugTraces( bool Show )
{
	m_DebugTraces = Show;
}

void UHitboxHandlerComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	for( const auto& hitData : m_ActiveHitboxes )
	{
		if( !hitData.m_PendingRemoval )
		{
			UpdateHitbox( hitData );
		}
	}

	RemovePendingHitboxes();
}

bool UHitboxHandlerComponent::TraceHitbox( const HitData& HitData, FHitResult& OutHit )
{
	TArray<TEnumAsByte<EObjectTypeQuery>> targetTraceTypes;

	const EObjectTypeQuery targetCollisionType = UEngineTypes::ConvertToObjectType( CUSTOM_TRACE_HURTBOX );
	targetTraceTypes.Add( targetCollisionType );

	TArray<AActor*> actorsToIgnore;
	actorsToIgnore.Add( HitData.m_Owner );

	bool hasSocketToFollow = HitData.m_SkeletalMesh ? (!HitData.m_SocketToFollow.ToString().IsEmpty()) : false;
	FVector location       = hasSocketToFollow ? HitData.m_SkeletalMesh->GetSocketLocation( HitData.m_SocketToFollow ) : HitData.m_Location;

	bool isHitSuccessful = UKismetSystemLibrary::SphereTraceSingleForObjects( HitData.m_World, location, location,
	                                                                          HitData.m_Radius, targetTraceTypes,
	                                                                          false, actorsToIgnore, EDrawDebugTrace::None, OutHit, true );

	return isHitSuccessful;
}

bool UHitboxHandlerComponent::WasActorAlreadyHit( AActor* Actor, uint32 HitboxId )
{
	ensureMsgf( Actor, TEXT("Actor is null") );

	if( !m_ActorGroupsMap.Find( Actor->GetUniqueID() ) ) return false;

	auto* activeHitboxesIt = m_ActiveHitboxes.FindByPredicate( [&]( const HitData& _hitData )
	{
		return _hitData.m_Id == HitboxId;
	} );

	ensureMsgf( activeHitboxesIt, TEXT("HitData should be available at this point") );

	return m_ActorGroupsMap[Actor->GetUniqueID()].ContainsByPredicate( [&]( const FHitGroupPair& _hitPair )
	{
		return _hitPair.m_GroupId == activeHitboxesIt->m_GroupId;
	} );
}

void UHitboxHandlerComponent::RegisterHitActor( AActor* Actor, uint32 HitboxId )
{
	ensureMsgf( Actor, TEXT("Actor is null") );

	// Update actor groups map
	auto* it = m_ActiveHitboxes.FindByPredicate( [&]( const HitData& _hitData )
	{
		return _hitData.m_Id == HitboxId;
	} );
	ensureMsgf( it, TEXT("HitData should be available at this point") );

	if( m_ActorGroupsMap.Contains( Actor->GetUniqueID() ) )
	{
		m_ActorGroupsMap[Actor->GetUniqueID()].AddUnique( FHitGroupPair{HitboxId, it->m_GroupId} );
	}
	else
	{
		TArray<FHitGroupPair> pairs;
		pairs.AddUnique( FHitGroupPair{HitboxId, it->m_GroupId} );

		m_ActorGroupsMap.Emplace( Actor->GetUniqueID(), pairs );
	}
}

void UHitboxHandlerComponent::UpdateHitbox( const HitData& HitData )
{
	FHitResult outHit;
	const bool success = TraceHitbox( HitData, outHit );

	AActor* hitActor = outHit.GetActor();
	if( success && !WasActorAlreadyHit( hitActor, HitData.m_Id ) )
	{
		RegisterHitActor( hitActor, HitData.m_Id );

		if( auto* Hittable = Cast<IHittable>( hitActor ) )
		{
			Hittable->OnHitReceived( HitData );
			m_HitDelegate.Broadcast( hitActor, HitData );
		}
	}
}

void UHitboxHandlerComponent::RemovePendingHitboxes()
{
	for( int i = m_ActiveHitboxes.Num() - 1; i >= 0; --i )
	{
		if( m_ActiveHitboxes[i].m_PendingRemoval )
		{
			TArray<uint32> targetKeys;
			for( const auto& pair : m_ActorGroupsMap )
			{
				if( pair.Value.ContainsByPredicate( [&]( const FHitGroupPair& hitPair )
				{
					return hitPair.m_Id == m_ActiveHitboxes[i].m_Id;
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
					if( m_ActorGroupsMap[targetKey][j].m_Id == m_ActiveHitboxes[i].m_Id )
					{
						m_ActorGroupsMap[targetKey].RemoveAt( j );
					}
				}
			}

			DEBUG_DestroyDebugSphere( m_ActiveHitboxes[i].m_Id );

			m_ActiveHitboxes.RemoveAt( i );
		}
	}
}

void UHitboxHandlerComponent::DEBUG_SpawnDebugSphere( const HitData& Hit )
{
	TObjectPtr<ASphereVisualizer> inst = GetWorld()->SpawnActor<ASphereVisualizer>( m_HitboxVisualizer );
	inst->SetId( Hit.m_Id );
	inst->SetRadius( Hit.m_Radius );

	if( m_ReferenceComponent )
	{
		inst->AttachToComponent( m_ReferenceComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale, Hit.m_SocketToFollow );
	}

	m_HitboxVisualizers.Emplace( inst );
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
