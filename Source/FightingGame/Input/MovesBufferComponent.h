// Copyright (c) Giammarco Agazzotti

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include <deque>

#include "InputEntry.h"
#include "FightingGame/Combat/MoveDataAsset.h"
#include "MovesBufferComponent.generated.h"

class AFightingCharacter;
class UInputComponent;
class UInputSequenceResolver;

struct FInputBufferEntry
{
    EInputEntry m_InputEntry;
    bool m_Used;
};

struct FMoveBufferEntry
{
    FName m_MoveName;
    bool m_Used;

    inline static FName s_MoveNone = FName( TEXT( "" ) );
};

UCLASS( ClassGroup = ( Custom ), meta = ( BlueprintSpawnableComponent ) )
class FIGHTINGGAME_API UMovesBufferComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UMovesBufferComponent();

    TObjectPtr<AFightingCharacter> m_OwnerCharacter = nullptr;

    // INPUT BUFFER [BEGIN]
    UFUNCTION( BlueprintCallable )
    void UseBufferedInput( EInputEntry Input );

    UFUNCTION( BlueprintCallable )
    bool IsInputBuffered( EInputEntry Input, bool ConsumeEntry = true );

    UFUNCTION( BlueprintCallable )
    void ClearInputsBuffer();

    UFUNCTION( BlueprintCallable )
    void InitInputBuffer();
    // INPUT BUFER [END]

    // MOVES BUFFER [BEGIN]
    UFUNCTION( BlueprintCallable )
    void UseBufferedMove( const FName& MoveName );

    UFUNCTION( BlueprintCallable )
    bool IsMoveBuffered( const FName& MoveName, bool ConsumeEntry = true );

    UFUNCTION( BlueprintCallable )
    void ClearMovesBuffer();

    UFUNCTION( BlueprintCallable )
    void InitMovesBuffer();

    // MOVES BUFFER [END]

    UPROPERTY( BlueprintReadOnly, DisplayName = "Input Movement" )
    float m_InputMovement = 0.f;

    UPROPERTY( BlueprintReadOnly, DisplayName = "Moving Right" )
    bool m_MovingRight = false;

    UPROPERTY( BlueprintReadOnly, DisplayName = "Moving Left" )
    bool m_MovingLeft = false;

protected:
    UPROPERTY( EditAnywhere, BlueprintReadWrite, DisplayName = "Inputs Buffer Size Frames" )
    int m_InputBufferSizeFrames = 6;

    UPROPERTY( EditAnywhere, BlueprintReadWrite, DisplayName = "Buffer Frame Rate (FPS)" )
    float m_BufferFrameRate = 30;

    UPROPERTY( EditAnywhere, BlueprintReadWrite, DisplayName = "Analog Movement Deadzone" )
    float m_AnalogMovementDeadzone = 0.1f;

    UPROPERTY( EditAnywhere, BlueprintReadWrite, DisplayName = "Directional Input Min Vector Length" )
    float m_MinDirectionalInputVectorLength = .5f;

    UPROPERTY( EditAnywhere, BlueprintReadWrite, DisplayName = "Directional Change Rotation Epsilon" )
    float m_DirectionalChangeRotationEpsilon = 15.f;

    UPROPERTY( EditAnywhere, BlueprintReadWrite, DisplayName = "Moves List" )
    TArray<TObjectPtr<UMoveDataAsset>> m_MovesList;

    UPROPERTY( EditAnywhere, BlueprintReadWrite, DisplayName = "Input Sequence Resolver" )
    TSubclassOf<UInputSequenceResolver> m_InputSequenceResolverClass = nullptr;

    virtual void BeginPlay() override;

public:
    virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

    void OnSetupPlayerInputComponent( UInputComponent* PlayerInputComponent );

    UFUNCTION( BlueprintCallable )
    float GetMovementDirection() const;

private:
    UPROPERTY()
    UInputComponent* m_PlayerInput = nullptr;

    UPROPERTY()
    TObjectPtr<UInputSequenceResolver> m_InputSequenceResolver = nullptr;

    std::deque<FInputBufferEntry> m_InputsBuffer;
    float m_IBElapsedFrameTime = 0.f;
    bool m_IBBufferChanged     = false;

    std::deque<FMoveBufferEntry> m_MovesBuffer;
    float m_MBElapsedFrameTime = 0.f;
    bool m_MBBufferChanged     = false;

    float m_MovementDirection = 0.f;

    FVector2D m_LastDirectionalInputVector;
    FVector2D m_DirectionalInputVector;
    EInputEntry m_LastDirectionalInputEntry = EInputEntry::None;

    void AddToInputBuffer( EInputEntry InputEntry );
    bool InputBufferContainsConsumable( EInputEntry InputEntry ) const;

    void AddToMovesBuffer( const FName& MoveName );
    bool MovesBufferContainsConsumable( const FName& MoveName );

    void OnMoveHorizontal( float Value );

    void OnStartJump();
    void OnStopJump();
    void OnAttack();
    void OnSpecial();

    void UpdateMovementDirection();
    void UpdateDirectionalInputs( UInputComponent* InputComponent );
    EInputEntry GetDirectionalInputEntryFromAngle( float Angle ) const;

    void OnInputRouteEnded( uint32 MoveUniqueId );
};
