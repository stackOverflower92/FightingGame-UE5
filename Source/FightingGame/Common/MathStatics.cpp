// Copyright (c) Giammarco Agazzotti

#include "MathStatics.h"

float UMathStatics::GetAngleBetween( const FVector2D A, const FVector2D B )
{
    auto normA = A.GetSafeNormal();
    auto normB = B.GetSafeNormal();

    float Ang1 = FMath::Atan2( normA.X, normA.Y );
    float Ang2 = FMath::Atan2( normB.X, normB.Y );

    float Ang = FMath::RadiansToDegrees( Ang1 - Ang2 );

    if( Ang > 180.0f )
    {
        Ang -= 360.0f;
    }
    else if( Ang < -180.0f )
    {
        Ang += 360.0f;
    }

    return Ang;
}
