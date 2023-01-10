// Copyright (c) Giammarco Agazzotti

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include <deque>

#include "InputEntry.h"
#include "FightingGame/Combat/MoveDataAsset.h"
#include "FightingGame/Common/ConversionStatics.h"
#include "FightingGame/FSM/FightingCharacterState.h"
#include "MovesBufferComponent.generated.h"

class AFightingCharacter;
class UInputComponent;
class UInputSequenceResolver;

struct FInputBufferEntry
{
    EInputEntry m_InputEntry;
    bool m_Used;

    FORCEINLINE FString ToString()
    {
        return m_InputEntry == EInputEntry::None ? TEXT( "---" ) : UConversionStatics::ConvertEnumValueToString( m_InputEntry, false );
    }
};

struct FInputsSequenceBufferEntry
{
    FName m_InputsSequenceName;
    int32 m_Priority;
    bool m_Used;

    inline static FName s_SequenceNone = FName( TEXT( "" ) );

    FORCEINLINE FString ToString()
    {
        return m_InputsSequenceName == s_SequenceNone ? TEXT( "---" ) : m_InputsSequenceName.ToString();
    }
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
    void UseBufferedInputsSequence( const FName& InputsSequenceName );

    UFUNCTION( BlueprintCallable )
    bool IsInputsSequenceBuffered( const FName& InputsSequenceName, bool ConsumeEntry = true );

    UFUNCTION( BlueprintCallable )
    void ClearInputsSequenceBuffer();

    UFUNCTION( BlueprintCallable )
    void InitInputsSequenceBuffer();

    void GetInputsSequenceBufferSnapshot( TArray<FInputsSequenceBufferEntry>& OutEntries, bool SkipEmptyEntries );

    // MOVES BUFFER [END]

    UPROPERTY( BlueprintReadOnly, DisplayName = "Input Movement" )
    float m_InputMovement = 0.f;

    UPROPERTY( BlueprintReadOnly, DisplayName = "Moving Right" )
    bool m_MovingRight = false;

    UPROPERTY( BlueprintReadOnly, DisplayName = "Moving Left" )
    bool m_MovingLeft = false;

protected:
    UPROPERTY( EditAnywhere, BlueprintReadWrite, DisplayName = "Inputs Buffer Size Frames" )
    int32 m_InputBufferSizeFrames = 6;

    UPROPERTY( EditAnywhere, BlueprintReadWrite, DisplayName = "Inputs Sequence Buffer Size Frames" )
    int32 m_InputsSequencesBufferSizeFrames = 10;

    UPROPERTY( EditAnywhere, BlueprintReadWrite, DisplayName = "Input Buffer Frame Rate (FPS)" )
    float m_InputBufferFrameRate = 30;

    UPROPERTY( EditAnywhere, BlueprintReadWrite, DisplayName = "Inputs Sequence Buffer Frame Rate (FPS)" )
    float m_InputsSequencesBufferFrameRate = 30;

    UPROPERTY( EditAnywhere, BlueprintReadWrite, DisplayName = "Analog Movement Deadzone" )
    float m_AnalogMovementDeadzone = 0.1f;

    UPROPERTY( EditAnywhere, BlueprintReadWrite, DisplayName = "Directional Input Min Vector Length" )
    float m_MinDirectionalInputVectorLength = .5f;

    UPROPERTY( EditAnywhere, BlueprintReadWrite, DisplayName = "Directional Change Rotation Epsilon" )
    float m_DirectionalChangeRotationEpsilon = 15.f;

    UPROPERTY( EditAnywhere, BlueprintReadWrite, DisplayName = "Inputs List" )
    TArray<TObjectPtr<UInputsSequence>> m_InputsList;

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

    std::deque<FInputsSequenceBufferEntry> m_InputsSequenceBuffer;
    float m_ISBElapsedFrameTime = 0.f;
    bool m_ISBBufferChanged     = false;

    float m_MovementDirection = 0.f;

    FVector2D m_LastDirectionalInputVector;
    FVector2D m_DirectionalInputVector;
    EInputEntry m_LastDirectionalInputEntry = EInputEntry::None;

    void AddToInputBuffer( EInputEntry InputEntry );
    bool InputBufferContainsConsumable( EInputEntry InputEntry ) const;

    void AddToInputsSequenceBuffer( const FName& InputsSequenceName, int32 Priority );
    bool InputsSequenceBufferContainsConsumable( const FName& MoveName );

    void OnMoveHorizontal( float Value );

    void OnStartJump();
    void OnStopJump();
    void OnAttack();
    void OnSpecial();

    void UpdateMovementDirection();
    void UpdateDirectionalInputs( UInputComponent* InputComponent );
    EInputEntry GetDirectionalInputEntryFromAngle( float Angle ) const;

    void OnInputRouteEnded( TObjectPtr<UInputsSequence> InputsSequence );
};
