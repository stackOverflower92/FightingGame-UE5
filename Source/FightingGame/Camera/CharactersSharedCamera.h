// Copyright (c) Giammarco Agazzotti

#pragma once

#include "CoreMinimal.h"
#include "GameCamera.h"
#include "GameFramework/Actor.h"
#include "CharactersSharedCamera.generated.h"

class AFightingCharacter;
UCLASS()
class FIGHTINGGAME_API ACharactersSharedCamera : public AGameCamera
{
	GENERATED_BODY()

protected:
	UPROPERTY( EditAnywhere, DisplayName = "Minimum Distance" )
	float m_MinDistance = 0.f;

	UPROPERTY( EditAnywhere, DisplayName = "Auto Add Targets On Begin Play" )
	bool m_AutoAddTargetsOnBeginPlay = true;

	UPROPERTY( EditAnywhere, DisplayName = "Movement Damping" )
	float MovementDamping = 2.f;

	UPROPERTY( EditAnywhere, DisplayName = "Position Offset" )
	FVector m_PositionOffset;

	virtual void BeginPlay() override;

public:
	ACharactersSharedCamera();

	virtual void Tick( float DeltaTime ) override;
	virtual void OnViewSet() override;

	bool AddTarget( AActor* Target );
	bool RemoveTarget( AActor* Target );

private:
	UPROPERTY()
	TArray<AActor*> m_Targets;

	UPROPERTY()
	TMap<AActor*, FVector> m_TargetsCentersMap;

	void AddAvailableTargets();

	FVector GetCenterPosition() const;

	void UpdateCameraPosition( float DeltaTime );
};
