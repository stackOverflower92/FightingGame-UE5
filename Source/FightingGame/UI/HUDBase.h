// Copyright (c) Giammarco Agazzotti

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "HUDBase.generated.h"

UCLASS()
class FIGHTINGGAME_API AHUDBase : public AHUD
{
    GENERATED_BODY()

public:
    AHUDBase();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick( float DeltaTime ) override;

    virtual void OnReady();
};
