// Copyright (c) Giammarco Agazzotti

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CameraManager.generated.h"

class AGameCamera;
UCLASS()
class FIGHTINGGAME_API ACameraManager : public AActor
{
	GENERATED_BODY()

public:
	ACameraManager();

protected:
	UPROPERTY( EditAnywhere, DisplayName="First Camera Id" )
	FString m_FirstCameraId = TEXT( "" );

	virtual void BeginPlay() override;

public:
	virtual void Tick( float DeltaTime ) override;

	UFUNCTION( BlueprintCallable )
	void Init();

	UFUNCTION( BlueprintCallable )
	bool SetCamera( FString Id );

private:
	TArray<TObjectPtr<AGameCamera>> m_GameCameras;
};
