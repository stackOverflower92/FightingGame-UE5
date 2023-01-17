// Copyright (c) Giammarco Agazzotti

#include "HitboxVisualizer.h"

AHitboxVisualizer::AHitboxVisualizer()
{
    PrimaryActorTick.bCanEverTick = true;
}

void AHitboxVisualizer::BeginPlay()
{
    Super::BeginPlay();
}

void AHitboxVisualizer::Tick( float DeltaTime )
{
    Super::Tick( DeltaTime );

    if( m_Knockback.IsSet() )
    {
        FVector lineEnd = GetActorLocation() + m_Knockback.GetValue().GetSafeNormal() * m_Radius;
        DrawDebugLine( GetWorld(), GetActorLocation(), lineEnd, FColor::White, false, -1, SDPG_Foreground, m_KnockbackLineThickness );
    }
}

void AHitboxVisualizer::SetRegularState()
{
    SetColor( m_RegularColor );
}

void AHitboxVisualizer::SetHitState()
{
    SetColor( m_HitColor );
}

void AHitboxVisualizer::SetBlockedState()
{
    SetColor( m_BlockedColor );
}

void AHitboxVisualizer::SetColor( const FLinearColor& Color )
{
    if( UMaterialInstanceDynamic* materialInstance = m_Sphere->CreateDynamicMaterialInstance( 0, m_Sphere->GetMaterial( 0 ) ) )
    {
        materialInstance->SetVectorParameterValue( TEXT( "Color" ), Color );
    }
}
