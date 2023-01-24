// Copyright (c) Giammarco Agazzotti

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "HitData.h"

#include "Hittable.generated.h"

// This class does not need to be modified.
UINTERFACE( MinimalAPI )
class UHittable : public UInterface
{
    GENERATED_BODY()
};

struct SuperArmorData;

class FIGHTINGGAME_API IHittable
{
    GENERATED_BODY()

public:
    virtual void OnHitReceived( const HitData& HitData );
    virtual bool IsHittable();
    virtual bool IsBlocking();
    virtual void EnableSuperArmor( bool Enable, const SuperArmorData& Data );
};
