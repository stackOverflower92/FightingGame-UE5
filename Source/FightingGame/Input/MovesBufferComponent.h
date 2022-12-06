// Copyright (c) Giammarco Agazzotti

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include <deque>
#include "MovesBufferComponent.generated.h"

class AFightingCharacter;
class UInputComponent;

struct FInputBufferEntry
{
	FString Name;
	bool Used;
};

UCLASS( ClassGroup = ( Custom ), meta = ( BlueprintSpawnableComponent ) )
class FIGHTINGGAME_API UMovesBufferComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UMovesBufferComponent();

	TObjectPtr<AFightingCharacter> m_OwnerCharacter = nullptr;

	UFUNCTION( BlueprintCallable )
	void UseBufferedInput( FString Input );

	UFUNCTION( BlueprintCallable )
	bool IsInputBuffered( FString Input, bool ConsumeEntry = true );

	UFUNCTION( BlueprintCallable )
	TArray<FString> GetBufferedInputs() const;

	UFUNCTION( BlueprintCallable )
	void ClearBuffer();

	UFUNCTION( BlueprintCallable )
	void InitBuffer();

	UPROPERTY( EditAnywhere, BlueprintReadOnly, DisplayName = "Input Movement" )
	float m_InputMovement = 0.f;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, DisplayName = "Moving Right" )
	bool m_MovingRight = false;

	UPROPERTY( EditAnywhere, BlueprintReadOnly, DisplayName = "Moving Left" )
	bool m_MovingLeft = false;

protected:
	UPROPERTY( EditAnywhere, BlueprintReadWrite, DisplayName = "Buffer Size Frames" )
	int m_BufferSizeFrames = 6;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, DisplayName = "Buffer Frame Length" )
	float m_BufferFrameLength = 0.032;

public:
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

	void OnSetupPlayerInputComponent( UInputComponent* PlayerInputComponent );

	UFUNCTION( BlueprintCallable )
	float GetMovementDirection() const;

private:
	UPROPERTY()
	UInputComponent* m_PlayerInput = nullptr;

	std::deque<FInputBufferEntry> m_Buffer;
	float m_ElapsedFrameTime = 0.f;
	bool m_BufferChanged = false;

	float m_MovementDirection = 0.f;

	void AddMoveToBuffer( const FString& MoveName );
	bool BufferContainsConsumableInput( const FString& Input ) const;

	void OnMoveHorizontal( float Value );

	void OnStartJump();
	void OnStopJump();
	void OnStartMoveRight();
	void OnStopMoveRight();
	void OnStartMoveLeft();
	void OnStopMoveLeft();
	void OnAttack();

	void UpdateMovement();
};
