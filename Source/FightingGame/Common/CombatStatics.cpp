// Copyright (c) Giammarco Agazzotti

#include "CombatStatics.h"
#include "FightingGame/Character/FightingCharacter.h"
#include "FightingGame/Combat/MoveDataAsset.h"
#include "FightingGame/Animation/FightingCharacterAnimInstance.h"
#include "FightingGame/Combat/HitboxDescription.h"
#include "FightingGame/Debugging/Debug.h"

namespace
{
    constexpr float loc_MinCustomTimeDilation = 0.001f;
    constexpr float loc_HitStunInitialDelay   = 0.f; //0.05f;
}

bool UCombatStatics::ExecuteMove( AFightingCharacter* Character, UMoveDataAsset* Move )
{
    if( !Character )
    {
        FG_SLOG_ERR( TEXT( "Character is null" ) );
        return false;
    }

    if( !Move )
    {
        FG_SLOG_ERR( TEXT( "Move data asset is null" ) );
        return false;
    }

    USkeletalMeshComponent* Mesh = Character->GetMesh();
    if( !Mesh )
    {
        FG_SLOG_ERR( TEXT( "Character has no skeletal mesh" ) );
        return false;
    }

    if( Move->m_AnimationMontageAsset )
    {
        UFightingCharacterAnimInstance* Inst = Cast<UFightingCharacterAnimInstance>( Mesh->GetAnimInstance() );
        if( !Inst )
        {
            FG_SLOG_ERR( TEXT( "Anim instance must be UFightingCharacterAnimInstance type" ) );
            return false;
        }

        Inst->AnimationRequested( Move->m_AnimationMontageAsset );
    }

    return true;
}

bool UCombatStatics::FaceOther( TObjectPtr<IFacingEntity> A, TObjectPtr<AActor> B, bool Instant /*= false*/ )
{
    if( !A )
    {
        FG_SLOG_ERR( TEXT("Character A is null") );
        return false;
    }

    if( !B )
    {
        FG_SLOG_ERR( TEXT( "Actor B is null" ) );
        return false;
    }

    bool Right = A->GetLocation().Y < B->GetActorLocation().Y;
    A->SetFacingRight( Right, Instant );

    return true;
}

bool UCombatStatics::FaceLocation( TObjectPtr<IFacingEntity> A, const FVector& Location )
{
    if( !A )
    {
        FG_SLOG_ERR( TEXT( "Character A is null" ) );
        return false;
    }

    bool Right = A->GetLocation().Y < Location.Y;
    A->SetFacingRight( Right, false );

    return true;
}

bool UCombatStatics::IsOtherOnTheRight( TObjectPtr<IFacingEntity> Me, TObjectPtr<IFacingEntity> Other )
{
    ensureMsgf( Me, TEXT("[Me] facing entity is null") );
    ensureMsgf( Other, TEXT("[Other] facing entity is null") );

    return Other->GetLocation().Y > Me->GetLocation().Y;
}

bool UCombatStatics::IsOtherOnTheLeft( TObjectPtr<IFacingEntity> Me, TObjectPtr<IFacingEntity> Other )
{
    return !IsOtherOnTheRight( Me, Other );
}

HitData UCombatStatics::GenerateHitDataFromHitboxDescription( TObjectPtr<AActor> HitboxOwner, TObjectPtr<USkeletalMeshComponent> SkeletalMesh,
                                                              const FHitboxDescription& HitboxDesc, int32 Id, int32 GroupId,
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

    float hitStopDuration         = HitboxDesc.m_HitStopDuration;
    float opponentHitStopDuration = HitboxDesc.m_UseSeparateHitStopValues ? HitboxDesc.m_OpponentHitStopDuration : hitStopDuration;

    TObjectPtr<USkeletalMeshComponent> skeletalMesh = HitboxDesc.m_UseLocation ? nullptr : SkeletalMesh;

    return HitData( HitboxDesc.m_ForceOpponentFacing,
                    HitboxDesc.m_DamagePercent,
                    HitboxDesc.m_DamageHP,
                    HitboxDesc.m_Radius,
                    processedKnockback,
                    HitboxDesc.m_IgnoreKnockbackMultiplier,
                    hitStopDuration,
                    opponentHitStopDuration,
                    HitboxDesc.m_Shake,
                    HitboxOwner->GetWorld(),
                    HitboxOwner,
                    skeletalMesh,
                    socketName,
                    targetLocation,
                    Id,
                    GroupId,
                    HitboxDesc.m_Priority,
                    HitboxDesc.m_IsCounter,
                    HitboxDesc.m_IgnoreBlock,
                    AdditionalActorsToIgnore,
                    HitboxDesc.m_HitSound,
                    HitboxDesc.m_CanClang,
                    HitboxDesc.m_ClangPriority,
                    HitboxDesc.m_AllowOnGroundedOpponents,
                    HitboxDesc.m_AllowOnAirborneOpponents );
}

FVector UCombatStatics::GetKnockbackFromOrientation( TObjectPtr<IFacingEntity> FacingEntity, float Orientation )
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
        FG_SLOG_ERR( TEXT("Character is null") );
        return false;
    }

    float MultipliedForce = Force * (IgnoreMultiplier ? 1.f : Character->GetKnockbackMultiplier());
    Character->LaunchCharacter( Direction.GetSafeNormal() * MultipliedForce, true, true );

    return true;
}

float UCombatStatics::GetHitStopTimeDilation()
{
    return loc_MinCustomTimeDilation;
}

float UCombatStatics::GetHitStopInitialDelay()
{
    return loc_HitStunInitialDelay;
}
