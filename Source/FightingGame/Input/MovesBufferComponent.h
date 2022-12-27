// Copyright (c) Giammarco Agazzotti

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include <deque>

#include "InputEntry.h"
#include "MovesBufferComponent.generated.h"

class AFightingCharacter;
class UInputComponent;

struct FInputBufferEntry
{
    EInputEntry m_MoveType;
    bool m_Used;
};

UCLASS( ClassGroup = ( Custom ), meta = ( BlueprintSpawnableComponent ) )
class FIGHTINGGAME_API UMovesBufferComponent : public UActorComponent
{
    GENERATED_BODY()

    using AngleRange = TTuple<float, float>;

public:
    UMovesBufferComponent();

    TObjectPtr<AFightingCharacter> m_OwnerCharacter = nullptr;

    UFUNCTION( BlueprintCallable )
    void UseBufferedInput( EInputEntry Input );

    UFUNCTION( BlueprintCallable )
    bool IsInputBuffered( EInputEntry Input, bool ConsumeEntry = true );

    UFUNCTION( BlueprintCallable )
    TArray<EInputEntry> GetBufferedInputs() const;

    UFUNCTION( BlueprintCallable )
    void ClearBuffer();

    UFUNCTION( BlueprintCallable )
    void InitBuffer();

    UPROPERTY( BlueprintReadOnly, DisplayName = "Input Movement" )
    float m_InputMovement = 0.f;

    UPROPERTY( BlueprintReadOnly, DisplayName = "Moving Right" )
    bool m_MovingRight = false;

    UPROPERTY( BlueprintReadOnly, DisplayName = "Moving Left" )
    bool m_MovingLeft = false;

protected:
    UPROPERTY( EditAnywhere, BlueprintReadWrite, DisplayName = "Buffer Size Frames" )
    int m_BufferSizeFrames = 6;

    UPROPERTY( EditAnywhere, BlueprintReadWrite, DisplayName = "Buffer Frame Length" )
    float m_BufferFrameLength = 0.032;

    UPROPERTY( EditAnywhere, BlueprintReadWrite, DisplayName = "Analog Movement Deadzone" )
    float m_AnalogMovementDeadzone = 0.1f;

    UPROPERTY( EditAnywhere, BlueprintReadWrite, DisplayName = "Directional Input Min Vector Length" )
    float m_MinDirectionalInputVectorLength = .5f;

    UPROPERTY( EditAnywhere, BlueprintReadWrite, DisplayName = "Directional Change Rotation Epsilon" )
    float m_DirectionalChangeRotationEpsilon = 15.f;

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
    bool m_BufferChanged     = false;

    float m_MovementDirection = 0.f;

    FVector2D m_LastDirectionalInputVector;
    FVector2D m_DirectionalInputVector;

    EInputEntry m_LastDirectionalInputEntry = EInputEntry::None;

    FORCEINLINE EInputEntry GetDirectionalInputEntryFromAngle( float Angle ) const
    {
        static float halfDelta = 10.f;

        static float forwardAngle = 90.f;
        static float downAngle    = 180.f;
        static float backAngle    = -90.f;
        static float upAngle      = 0.f;

        // Up
        if( Angle > upAngle - halfDelta && Angle < upAngle + halfDelta )
        {
            return EInputEntry::Up;
        }

        // Up-forward
        if( Angle >= upAngle + halfDelta && Angle < forwardAngle - halfDelta )
        {
            return EInputEntry::UpForward;
        }

        // Forward
        if( Angle > forwardAngle - halfDelta && Angle < forwardAngle + halfDelta )
        {
            return EInputEntry::Forward;
        }

        // Forward-down
        if( Angle >= forwardAngle + halfDelta && Angle < downAngle - halfDelta )
        {
            return EInputEntry::ForwardDown;
        }

        // Down
        if( (Angle >= downAngle && Angle >= downAngle - halfDelta) ||
            (Angle > -downAngle && Angle < -downAngle + halfDelta) )
        {
            return EInputEntry::Down;
        }

        // Down-back
        if( Angle > -downAngle + halfDelta && Angle < backAngle - halfDelta )
        {
            return EInputEntry::DownBackward;
        }

        // Back
        if( Angle > backAngle - halfDelta && Angle < backAngle + halfDelta )
        {
            return EInputEntry::Backward;
        }

        // Back-Up
        if( Angle >= backAngle + halfDelta && Angle < upAngle - halfDelta )
        {
            return EInputEntry::BackwardUp;
        }

        return EInputEntry::None;
    }

    void AddMoveToBuffer( EInputEntry MoveType );
    bool BufferContainsConsumableInput( EInputEntry MoveType ) const;

    void OnMoveHorizontal( float Value );

    void OnStartJump();
    void OnStopJump();
    void OnAttack();
    void OnSpecial();

    void UpdateMovementDirection();
    void UpdateDirectionalInputs( UInputComponent* InputComponent );
};
