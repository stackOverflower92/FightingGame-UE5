// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "FightingGameGameModeBase.generated.h"

class ACameraManager;

UCLASS()
class FIGHTINGGAME_API AFightingGameGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

protected:
	TObjectPtr<ACameraManager> m_CameraManager = nullptr;
};
