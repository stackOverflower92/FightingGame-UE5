// Copyright (c) Giammarco Agazzotti

#include "FacingEntity.h"

bool IFacingEntity::IsFacingRight()
{
	return true;
}

void IFacingEntity::SetFacingRight( bool Right, bool Instant )
{
}

FVector IFacingEntity::GetLocation()
{
	return FVector::ZeroVector;
}
