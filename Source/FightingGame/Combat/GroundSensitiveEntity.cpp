// Copyright (c) Giammarco Agazzotti


#include "GroundSensitiveEntity.h"

bool IGroundSensitiveEntity::IsGrounded() const
{
	return true;
}

bool IGroundSensitiveEntity::IsAirborne() const
{
	return false;
}
