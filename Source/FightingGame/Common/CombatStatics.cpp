// Copyright (c) Giammarco Agazzotti

#include "CombatStatics.h"
#include "FightingGame/Character/FightingCharacter.h"
#include "FightingGame/Combat/MoveDataAsset.h"
#include "FightingGame/Animation/FightingCharacterAnimInstance.h"
#include "FightingGame/Combat/HitboxDescription.h"

namespace
{
	constexpr float loc_MinCustomTimeDilation = 0.001f;
	constexpr float loc_HitStunInitialDelay   = 0.05f;
}

bool UCombatStatics::ExecuteMove( AFightingCharacter* Character, UMoveDataAsset* Move )
{
	if( !Character )
	{
		UE_LOG( LogTemp, Error, TEXT( "Character is null" ) );
		return false;
	}

	if( !Move )
	{
		UE_LOG( LogTemp, Error, TEXT( "Move data asset is null" ) );
		return false;
	}

	USkeletalMeshComponent* Mesh = Character->GetMesh();
	if( !Mesh )
	{
		UE_LOG( LogTemp, Error, TEXT( "Character has no skeletal mesh" ) );
		return false;
	}

	if( Move->m_AnimationMontageAsset )
	{
		UFightingCharacterAnimInstance* Inst = Cast<UFightingCharacterAnimInstance>( Mesh->GetAnimInstance() );
		if( !Inst )
		{
			UE_LOG( LogTemp, Error, TEXT( "Anim instance must be UFightingCharacterAnimInstance type" ) );
			return false;
		}

		Inst->AnimationRequested( Move->m_AnimationMontageAsset );
	}

	return true;
}

bool UCombatStatics::FaceOther( IFacingEntity* A, AActor* B, bool Instant /*= false*/ )
{
	if( !A )
	{
		UE_LOG( LogTemp, Error, TEXT("Character A is null") );
		return false;
	}

	if( !B )
	{
		UE_LOG( LogTemp, Error, TEXT("Actor B is null") );
		return false;
	}

	bool Right = A->GetLocation().Y < B->GetActorLocation().Y;
	A->SetFacingRight( Right, Instant );

	return true;
}

bool UCombatStatics::FaceLocation( IFacingEntity* A, const FVector& Location )
{
	if( !A )
	{
		UE_LOG( LogTemp, Error, TEXT("Character A is null") );
		return false;
	}

	bool Right = A->GetLocation().Y < Location.Y;
	A->SetFacingRight( Right, false );

	return true;
}

HitData UCombatStatics::GenerateHitDataFromHitboxDescription( TObjectPtr<AActor> HitboxOwner, TObjectPtr<USkeletalMeshComponent> SkeletalMesh,
                                                              const FHitboxDescription& HitboxDesc, int Id, int32 GroupId,
                                                              TArray<TObjectPtr<AActor>> AdditionalActorsToIgnore /*= {}*/ )
{
	FName socketName;
	IFacingEntity* facingEntity = Cast<IFacingEntity>( HitboxOwner );

	// #TODO how to handle the case where the owner is not an IFacingEntity? should we require it?
	if( facingEntity )
	{
		socketName = facingEntity->IsFacingRight() ? HitboxDesc.m_SocketName : HitboxDesc.m_SocketNameMirrored;
	}
	else
	{
		socketName = HitboxDesc.m_SocketName;
	}

	FVector processedKnockback = GetKnockbackFromOrientation( facingEntity, HitboxDesc.m_KnockbackOrientation ) * HitboxDesc.m_KnockbackForce;

	FVector targetLocation = HitboxDesc.m_Location;
	if( facingEntity )
	{
		targetLocation.Y *= facingEntity->IsFacingRight() ? 1.f : -1.f;
	}

	return HitData( HitboxDesc.m_ForceOpponentFacing,
	                HitboxDesc.m_DamagePercent,
	                HitboxDesc.m_Radius,
	                processedKnockback,
	                HitboxDesc.m_IgnoreKnockbackMultiplier,
	                HitboxDesc.m_HitStopDuration,
	                HitboxDesc.m_Shake,
	                HitboxOwner->GetWorld(),
	                HitboxOwner,
	                SkeletalMesh,
	                socketName,
	                targetLocation,
	                Id,
	                GroupId,
	                HitboxDesc.m_Priority,
	                AdditionalActorsToIgnore );
}

FVector UCombatStatics::GetKnockbackFromOrientation( IFacingEntity* FacingEntity, float Orientation )
{
	ensureMsgf( FacingEntity, TEXT("Character is null") );

	float targetRoll = FacingEntity->IsFacingRight() ? -Orientation : -180.f + Orientation;
	FRotator rotator = FRotator( 0, 0, targetRoll );
	return rotator.RotateVector( FVector::RightVector );
}

bool UCombatStatics::ApplyKnockbackTo( const FVector& Direction, float Force, AFightingCharacter* Character, bool IgnoreMultiplier )
{
	if( !Character )
	{
		UE_LOG( LogTemp, Error, TEXT("Character is null") );
		return false;
	}

	float MultipliedForce = Force * (IgnoreMultiplier ? 1.f : Character->GetKnockbackMultiplier());
	Character->LaunchCharacter( Direction.GetSafeNormal() * MultipliedForce, true, true );

	return true;
}

float UCombatStatics::GetMinCustomTimeDilation()
{
	return loc_MinCustomTimeDilation;
}

float UCombatStatics::GetHitStunInitialDelay()
{
	return loc_HitStunInitialDelay;
}
