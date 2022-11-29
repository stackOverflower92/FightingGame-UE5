// Copyright (c) Giammarco Agazzotti

#include "HitboxHandlerComponent.h"
#include "Hittable.h"
#include "FightingGame/Collision/CustomCollisionChannels.h"
#include "FightingGame/Common/CombatStatics.h"
#include "Kismet/KismetSystemLibrary.h"

UHitboxHandlerComponent::UHitboxHandlerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UHitboxHandlerComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UHitboxHandlerComponent::AddHitbox( FHitData HitData )
{
	m_ActiveHitboxes.AddUnique( HitData );
}

void UHitboxHandlerComponent::RemoveHitbox( uint32 HitUniqueId )
{
	auto* It = m_ActiveHitboxes.FindByPredicate( [&HitUniqueId]( const FHitData& HitData )
	{
		return HitData.m_Id == HitUniqueId;
	} );

	if( It )
	{
		(*It).m_PendingRemoval = true;
	}
}

void UHitboxHandlerComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	for( const auto& HitData : m_ActiveHitboxes )
	{
		if( !HitData.m_PendingRemoval )
		{
			UpdateHitbox( HitData );
		}
	}

	RemovePendingHitboxes();
}

bool UHitboxHandlerComponent::TraceHitbox( const FHitData& HitData, FHitResult& OutHit )
{
	TArray<TEnumAsByte<EObjectTypeQuery>> TargetTraceTypes;

	const EObjectTypeQuery TargetCollisionType = UEngineTypes::ConvertToObjectType( CUSTOM_TRACE_HURTBOX );
	TargetTraceTypes.Add( TargetCollisionType );

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add( HitData.m_Owner );

	const bool HasSocketToFollow = !HitData.m_SocketToFollow.ToString().IsEmpty();
	const FVector Location = HasSocketToFollow ? HitData.m_SkeletalMesh->GetSocketLocation( HitData.m_SocketToFollow ) : HitData.m_Location;

	const bool SuccessfulHit = UKismetSystemLibrary::SphereTraceSingleForObjects( HitData.m_World, Location, Location,
	                                                                              HitData.m_Radius, TargetTraceTypes,
	                                                                              false, ActorsToIgnore, EDrawDebugTrace::ForOneFrame, OutHit, true );

	return SuccessfulHit;
}

bool UHitboxHandlerComponent::WasActorAlreadyHit( AActor* Actor, uint32 HitboxId ) const
{
	ensureMsgf( Actor, TEXT("Actor is null") );

	auto* It = m_HitActorsMap.Find( HitboxId );
	if( !It ) return false;

	return It->Contains( Actor->GetUniqueID() );
}

void UHitboxHandlerComponent::RegisterHitActor( AActor* Actor, uint32 HitboxId )
{
	ensureMsgf( Actor, TEXT("Actor is null") );

	if( m_HitActorsMap.Contains( HitboxId ) )
	{
		m_HitActorsMap[HitboxId].AddUnique( Actor->GetUniqueID() );
	}
	else
	{
		TArray<uint32> HitActors;
		HitActors.AddUnique( Actor->GetUniqueID() );

		m_HitActorsMap.Emplace( HitboxId, HitActors );
	}
}

void UHitboxHandlerComponent::UpdateHitbox( const FHitData& HitData )
{
	FHitResult OutHit;
	const bool Success = TraceHitbox( HitData, OutHit );

	AActor* HitActor = OutHit.GetActor();
	if( Success && !WasActorAlreadyHit( HitActor, HitData.m_Id ) )
	{
		RegisterHitActor( HitActor, HitData.m_Id );

		if( auto* Hittable = Cast<IHittable>( HitActor ) )
		{
			Hittable->OnHit( HitData );
		}
	}
}

void UHitboxHandlerComponent::RemovePendingHitboxes()
{
	for( int i = m_ActiveHitboxes.Num() - 1; i >= 0; --i )
	{
		if( m_ActiveHitboxes[i].m_PendingRemoval )
		{
			m_HitActorsMap.Remove( m_ActiveHitboxes[i].m_Id );
			m_ActiveHitboxes.RemoveAt( i );
		}
	}
}
