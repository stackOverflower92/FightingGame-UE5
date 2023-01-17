// Copyright (c) Giammarco Agazzotti

#include "Hittable.h"

void IHittable::OnHitReceived( const HitData& HitData )
{
}

bool IHittable::IsHittable()
{
    return true;
}

bool IHittable::IsBlocking()
{
    return false;
}
