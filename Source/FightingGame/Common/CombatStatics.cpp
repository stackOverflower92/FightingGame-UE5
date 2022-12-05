// Copyright (c) Giammarco Agazzotti

#include "CombatStatics.h"
#include "FightingGame/Character/FightingCharacter.h"
#include "FightingGame/Combat/MoveDataAsset.h"
#include "FightingGame/Animation/FightingCharacterAnimInstance.h"

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

bool UCombatStatics::FaceOther( AFightingCharacter* A, AActor* B, bool Instant /*= false*/ )
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

	bool Right = A->GetActorLocation().Y < B->GetActorLocation().Y;
	A->SetFacingRight( Right, Instant );

	return true;
}

bool UCombatStatics::FaceLocation( AFightingCharacter* A, const FVector& Location )
{
	if( !A )
	{
		UE_LOG( LogTemp, Error, TEXT("Character A is null") );
		return false;
	}

	bool Right = A->GetActorLocation().Y < Location.Y;
	A->SetFacingRight( Right );

	return true;
}

FVector UCombatStatics::GetKnockbackFromOrientation( AFightingCharacter* Character, float Orientation )
{
	ensureMsgf( Character, TEXT("Character is null") );

	bool FacingRight = Character->IsFacingRight();
	FVector Forward = FacingRight ? FVector::RightVector : -FVector::RightVector;

	float FinalKnockbackOrientation = FacingRight ? Orientation : -Orientation;

	FRotator Rotator = FRotator( 0, 0, FinalKnockbackOrientation );
	return Rotator.RotateVector( Forward );
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
