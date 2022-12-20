// Copyright (c) Giammarco Agazzotti

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GroundSensitiveEntity.generated.h"

UINTERFACE()
class UGroundSensitiveEntity : public UInterface
{
	GENERATED_BODY()
};

class FIGHTINGGAME_API IGroundSensitiveEntity
{
	GENERATED_BODY()

public:
	virtual bool IsGrounded() const;
	virtual bool IsAirborne() const;
};
