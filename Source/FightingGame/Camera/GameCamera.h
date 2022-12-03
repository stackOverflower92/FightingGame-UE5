// Copyright (c) Giammarco Agazzotti

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraActor.h"
#include "GameFramework/Actor.h"
#include "GameCamera.generated.h"

UCLASS()
class FIGHTINGGAME_API AGameCamera : public ACameraActor
{
	GENERATED_BODY()

public:
	AGameCamera();

protected:
	UPROPERTY( EditAnywhere, BlueprintReadWrite, DisplayName = "Id" )
	FString m_Id;

	virtual void BeginPlay() override;

public:
	virtual void Tick( float DeltaTime ) override;
	virtual void OnViewSet();

	FString GetId() const;
};
