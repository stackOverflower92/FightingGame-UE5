// Copyright (c) Giammarco Agazzotti

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "FacingEntity.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UFacingEntity : public UInterface
{
	GENERATED_BODY()
};

class FIGHTINGGAME_API IFacingEntity
{
	GENERATED_BODY()

public:
	virtual bool IsFacingRight();
	virtual void SetFacingRight( bool Right, bool Instant );

	// #TODO return TOptional<FVector> ?
	virtual FVector GetLocation();
};
