// Copyright (c) Giammarco Agazzotti

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CounterEntity.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UCounterEntity : public UInterface
{
    GENERATED_BODY()
};

class FIGHTINGGAME_API ICounterEntity
{
    GENERATED_BODY()

public:
    virtual void SetIsCountering( bool IsCountering );
};
