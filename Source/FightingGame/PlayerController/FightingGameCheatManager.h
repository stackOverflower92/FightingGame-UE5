// Copyright (c) Giammarco Agazzotti

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CheatManager.h"
#include "UObject/Object.h"
#include "FightingGameCheatManager.generated.h"

UCLASS()
class FIGHTINGGAME_API UFightingGameCheatManager : public UCheatManager
{
	GENERATED_BODY()

public:
	UFUNCTION( exec, Category="Custom Cheats" )
	void ShowHitTraces( int32 Show );
};
