// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "FightingGameGameModeBase.generated.h"

class ACameraManager;
class AGameFramework;

UCLASS()
class FIGHTINGGAME_API AFightingGameGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	FORCEINLINE TObjectPtr<AGameFramework> GetGameFramework() const { return m_GameFrameworkInstance; }

protected:
	UPROPERTY()
	TObjectPtr<ACameraManager> m_CameraManager = nullptr;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = Classes, DisplayName = "Game Framework Class" )
	TSubclassOf<AGameFramework> m_GameFrameworkClass;

	UPROPERTY()
	TObjectPtr<AGameFramework> m_GameFrameworkInstance = nullptr;

	void InitCameraManager();
};
