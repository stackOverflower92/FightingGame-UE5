// Copyright (c) Giammarco Agazzotti

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Manager.generated.h"

class AGameFramework;

UCLASS()
class FIGHTINGGAME_API AManager : public AActor
{
	GENERATED_BODY()

public:
	AManager();

	virtual void OnRegister( AGameFramework& Framework );
	virtual void OnDeregister( AGameFramework& Framework );
};
