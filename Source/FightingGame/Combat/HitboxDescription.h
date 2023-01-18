// Copyright (c) Giammarco Agazzotti

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "HitboxDescription.generated.h"

USTRUCT( BlueprintType )
struct FHitboxDescription
{
    GENERATED_BODY()

    UPROPERTY( EditAnywhere, BlueprintReadWrite, DisplayName = "Socket Name (Regular)" )
    FName m_SocketName;

    UPROPERTY( EditAnywhere, BlueprintReadWrite, DisplayName = "Socket Name (Mirrored)" )
    FName m_SocketNameMirrored;

    UPROPERTY( EditAnywhere, BlueprintReadWrite, DisplayName = "Is Counter" )
    bool m_IsCounter = false;

    UPROPERTY( EditAnywhere, BlueprintReadWrite, DisplayName = "Ignore Block" )
    bool m_IgnoreBlock = false;

    UPROPERTY( EditAnywhere, BlueprintReadWrite, DisplayName = "Use Location (Instead of sockets)" )
    bool m_UseLocation = false;

    UPROPERTY( EditAnywhere, BlueprintReadWrite, DisplayName = "Location (Relative to the owner)",
        meta = (EditCondition = "m_UseLocation", EditConditionHides = "m_UseLocation") )
    FVector m_Location = FVector::ZeroVector;

    UPROPERTY( EditAnywhere, BlueprintReadWrite, DisplayName = "Priority" )
    int32 m_Priority = 0;

    UPROPERTY( EditAnywhere, BlueprintReadWrite, DisplayName = "Force Opponent Facing" )
    bool m_ForceOpponentFacing = true;

    UPROPERTY( EditAnywhere, BlueprintReadWrite, DisplayName = "Damage Percent" )
    float m_DamagePercent = 0.f;

    UPROPERTY( EditAnywhere, BlueprintReadWrite, DisplayName = "Damage HP" )
    float m_DamageHP = 0.f;

    UPROPERTY( EditAnywhere, BlueprintReadWrite, DisplayName = "Radius" )
    float m_Radius = 10.f;

    UPROPERTY( EditAnywhere, BlueprintReadWrite, DisplayName = "Knockback Orientation" )
    float m_KnockbackOrientation = 0.f;

    UPROPERTY( EditAnywhere, BlueprintReadWrite, DisplayName = "Knockback Force" )
    float m_KnockbackForce = 600.f;

    UPROPERTY( EditAnywhere, BlueprintReadWrite, DisplayName = "Ignore Knockback Multiplier" )
    bool m_IgnoreKnockbackMultiplier = false;

    UPROPERTY( EditAnywhere, BlueprintReadWrite, DisplayName = "Hit Stop Duration (Owner)" )
    float m_HitStopDuration = 0.f;

    UPROPERTY( EditAnywhere, BlueprintReadWrite, DisplayName = "Use Separate Hit Stop Values" )
    bool m_UseSeparateHitStopValues = false;

    UPROPERTY( EditAnywhere, BlueprintReadWrite, DisplayName = "Hit Stop Duration (Opponent)", meta=(EditCondition = "m_UseSeparateHitStopValues",
        EditConditionHides = "m_UseSeparateHitStopValues") )
    float m_OpponentHitStopDuration = 0.f;

    UPROPERTY( EditAnywhere, BlueprintReadWrite, DisplayName = "Shake" )
    bool m_Shake = true;
};
