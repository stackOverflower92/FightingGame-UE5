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

void IHittable::EnableSuperArmor( const SuperArmorData& Data )
{
}

void IHittable::DisableSuperArmor()
{
}
