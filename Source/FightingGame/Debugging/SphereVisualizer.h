// Copyright (c) Giammarco Agazzotti

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SphereVisualizer.generated.h"

UCLASS()
class FIGHTINGGAME_API ASphereVisualizer : public AActor
{
	GENERATED_BODY()

public:
	ASphereVisualizer();

protected:
	UPROPERTY( EditAnywhere, BlueprintReadWrite, DisplayName = "Mesh" )
	TObjectPtr<UStaticMeshComponent> m_Sphere;

	virtual void BeginPlay() override;

public:
	void SetRadius( float Radius );

	FORCEINLINE void SetId( int Id ) { m_Id = Id; }
	FORCEINLINE int GetId() const { return m_Id; };

	FORCEINLINE void SetVisualizerOwner( TObjectPtr<AActor> NewOwner ) { m_Owner = NewOwner; };
	FORCEINLINE TObjectPtr<AActor> GetVisualizerOwner() const { return m_Owner; }

	FORCEINLINE void SetLocation( const FVector& Location ) { m_Location = Location; }
	FORCEINLINE TOptional<FVector> GetLocation() const { return m_Location; }

	virtual void Tick( float DeltaTime ) override;

protected:
	int m_Id       = -1;
	float m_Radius = 0.f;

	UPROPERTY()
	TObjectPtr<AActor> m_Owner = nullptr;

	TOptional<FVector> m_Location;
};
