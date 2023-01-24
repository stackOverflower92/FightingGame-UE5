// Copyright (c) Giammarco Agazzotti

#include "MovesBufferComponent.h"

#include "imgui.h"
#include "Components/InputComponent.h"
#include "FightingGame/Character/FightingCharacter.h"
#include "FightingGame/Combat/InputSequenceResolver.h"
#include "FightingGame/Common/ConversionStatics.h"
#include "FightingGame/Common/MathStatics.h"
#include "FightingGame/Debugging/Debug.h"
#include "Kismet/KismetSystemLibrary.h"

namespace
{
    int32 loc_ShowInputBuffer = 0;
    FG_CVAR_FLAG_DESC( CVarShowInputBuffer, TEXT( "MovesBufferComponent.ShowInputBuffer" ), loc_ShowInputBuffer );

    int32 loc_ShowInputsSequenceBuffer = 0;
    FG_CVAR_FLAG_DESC( CVarShowMovesBuffer, TEXT( "MovesBufferComponent.ShowInputsSequenceBuffer" ), loc_ShowInputsSequenceBuffer );

    int32 loc_ShowDirectionalAngle = 0;
    FG_CVAR_FLAG_DESC( CVarShowDirectionalAngle, TEXT("MovesBufferComponent.ShowDirectionalAngle"), loc_ShowDirectionalAngle );

    int32 loc_ShowHoldInputsMap = 0;
    FG_CVAR_FLAG_DESC( CVarShowHoldInputsMap, TEXT("MovesBufferComponent.ShowHoldInputsMap"), loc_ShowHoldInputsMap );

    FName loc_JumpAction           = TEXT( "Jump" );
    FName loc_AttackAction         = TEXT( "Attack" );
    FName loc_SpecialAction        = TEXT( "Special" );
    FName loc_MoveHorizontalAction = TEXT( "MoveHorizontal" );
    FName loc_MoveVerticalAction   = TEXT( "MoveVertical" );
    FName loc_CounterAction        = TEXT( "Counter" );

    int32 loc_MaxEntryId                 = 1000;
    int32 loc_CurrentBufferEntryUniqueId = 0;
}

FBufferEntry::FBufferEntry( bool Used ): m_Used( Used )
{
    if( loc_CurrentBufferEntryUniqueId < loc_MaxEntryId )
    {
        ++loc_CurrentBufferEntryUniqueId;
    }
    else
    {
        loc_CurrentBufferEntryUniqueId = 0;
    }

    m_UniqueId = loc_CurrentBufferEntryUniqueId;
}

FInputBufferEntry::FInputBufferEntry( EInputEntry InputEntry, EInputPhase Phase, bool Used ) :
    FBufferEntry( Used ),
    m_InputEntry( InputEntry ),
    m_Phase( Phase )
{
}

FInputsSequenceBufferEntry::FInputsSequenceBufferEntry( const FString& InputsSequenceName, int32 Priority, bool Used ) :
    FBufferEntry( Used ),
    m_InputsSequenceName( InputsSequenceName ),
    m_Priority( Priority )
{
}

UMovesBufferComponent::UMovesBufferComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UMovesBufferComponent::BeginPlay()
{
    Super::BeginPlay();

    m_InputSequenceResolver = m_InputSequenceResolverClass
                                  ? NewObject<UInputSequenceResolver>( GetOwner(), m_InputSequenceResolverClass )
                                  : NewObject<UInputSequenceResolver>( GetOwner() );

    m_InputSequenceResolver->m_InputRouteEndedDelegate.AddUObject( this, &UMovesBufferComponent::OnInputRouteEnded );

    m_InputSequenceResolver->Init( m_InputsList );
}

void UMovesBufferComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
    Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

    m_IBElapsedFrameTime += DeltaTime;

    static float bufferFrameDuration = 1.f / m_InputBufferFrameRate;
    if( m_IBElapsedFrameTime >= bufferFrameDuration )
    {
        m_IBElapsedFrameTime = 0.f;

        if( !m_IBBufferChanged )
        {
            AddToInputBuffer( EInputEntry::None, EInputPhase::Pressed );
        }

        for( TTuple<EInputEntry, FHoldInputBufferData>& pair : m_HoldInputsMap )
        {
            if( !pair.Value.m_Expired )
            {
                ++pair.Value.m_ElapsedFrames;
            }
        }

        using HoldInputsMapType = TMap<EInputEntry, FHoldInputBufferData>;
        for( HoldInputsMapType::TIterator it = m_HoldInputsMap.CreateIterator(); it; ++it )
        {
            if( (*it).Value.m_Expired )
            {
                it.RemoveCurrent();
            }
        }
    }

    m_ISBElapsedFrameTime += DeltaTime;

    static float movesBufferFrameDuration = 1.f / m_InputsSequencesBufferFrameRate;
    if( m_ISBElapsedFrameTime >= movesBufferFrameDuration )
    {
        m_ISBElapsedFrameTime = 0.f;
        if( !m_ISBBufferChanged )
        {
            AddToInputsSequenceBuffer( FInputsSequenceBufferEntry::s_SequenceNone, 0 );
        }
    }

    if( loc_ShowInputBuffer )
    {
        if( m_OwnerCharacter && m_OwnerCharacter->m_PlayerIndex == 0 )
        {
            for( int32 i = 0; i < m_InputsBuffer.Num(); ++i )
            {
                FInputBufferEntry& entry = m_InputsBuffer[i];
                FColor color             = entry.m_Used ? FColor::Red : FColor::Green;

                FG_SLOG_KEY( i, FString::Printf( TEXT( "%s [%u]" ), *entry.ToString(), entry.m_UniqueId ), color );
            }
        }
    }

    if( loc_ShowInputsSequenceBuffer )
    {
        if( m_OwnerCharacter && m_OwnerCharacter->m_PlayerIndex == 0 )
        {
            for( int32 i = 0; i < m_InputsSequenceBuffer.Num(); ++i )
            {
                FInputsSequenceBufferEntry& entry = m_InputsSequenceBuffer[i];
                FColor color                      = entry.m_Used ? FColor::Red : FColor::Green;

                FG_SLOG_KEY( i + 20, FString::Printf( TEXT( "%s [%u]" ), *entry.ToString(), entry.m_UniqueId ), color );
            }
        }
    }

    if( loc_ShowHoldInputsMap )
    {
        FString title = FString::Printf( TEXT( "Hold Inputs (%s)" ), *m_OwnerCharacter->GetName() );
        ImGui::Begin( TCHAR_TO_ANSI( *title ) );
        {
            for( const auto& pair : m_HoldInputsMap )
            {
                if( !pair.Value.m_Expired )
                {
                    FString entryStringified = UConversionStatics::ConvertEnumValueToString( pair.Key );
                    FString text             = FString::Printf( TEXT( "%s - %u" ), *entryStringified, pair.Value.m_ElapsedFrames );

                    ImGui::Text( TCHAR_TO_ANSI( *text ) );
                }
            }
        }
        ImGui::End();
    }

    m_IBBufferChanged  = false;
    m_ISBBufferChanged = false;

    UpdateMovementDirection();

    if( m_PlayerInput )
    {
        float horizontalMovement = m_PlayerInput->GetAxisValue( loc_MoveHorizontalAction );

        m_InputMovement = horizontalMovement;

        m_MovingRight = horizontalMovement > m_AnalogMovementDeadzone;
        m_MovingLeft  = horizontalMovement < -m_AnalogMovementDeadzone;

        UpdateDirectionalInputs( m_PlayerInput );
    }
}

void UMovesBufferComponent::OnSetupPlayerInputComponent( UInputComponent* PlayerInputComponent )
{
    m_PlayerInput = PlayerInputComponent;
    if( m_PlayerInput )
    {
        m_PlayerInput->BindAction( loc_JumpAction, IE_Pressed, this, &UMovesBufferComponent::OnJumpPressed );
        m_PlayerInput->BindAction( loc_JumpAction, IE_Released, this, &UMovesBufferComponent::OnJumpReleased );

        m_PlayerInput->BindAction( loc_AttackAction, IE_Pressed, this, &UMovesBufferComponent::OnAttackPressed );
        m_PlayerInput->BindAction( loc_AttackAction, IE_Released, this, &UMovesBufferComponent::OnAttackReleased );

        m_PlayerInput->BindAction( loc_SpecialAction, IE_Pressed, this, &UMovesBufferComponent::OnSpecialPressed );
        m_PlayerInput->BindAction( loc_SpecialAction, IE_Released, this, &UMovesBufferComponent::OnSpecialReleased );

        m_PlayerInput->BindAction( loc_CounterAction, IE_Pressed, this, &UMovesBufferComponent::OnCounterPressed );
        m_PlayerInput->BindAction( loc_CounterAction, IE_Released, this, &UMovesBufferComponent::OnCounterReleased );

        m_PlayerInput->BindAxis( loc_MoveHorizontalAction );
        m_PlayerInput->BindAxis( loc_MoveVerticalAction );
    }

    InitInputBuffer();

    InitInputsSequenceBuffer();
}

bool UMovesBufferComponent::IsInputBuffered( EInputEntry Input, bool ConsumeEntry )
{
    for( int32 i = 0; i < m_InputsBuffer.Num(); ++i )
    {
        FInputBufferEntry& entry = m_InputsBuffer[i];
        if( entry.m_InputEntry != EInputEntry::None && entry.m_InputEntry == Input && !entry.m_Used )
        {
            if( ConsumeEntry )
            {
                entry.m_Used = true;
            }
            return true;
        }
    }

    return false;
}

float UMovesBufferComponent::GetMovementDirection() const
{
    return m_MovementDirection;
}

EInputEntry UMovesBufferComponent::GetDirectionalInputEntryFromAngle( float Angle ) const
{
    // Assuming the character is facing right

    static float forwardAngle = 90.f;
    static float downAngle    = 180.f;
    static float backAngle    = -90.f;
    static float upAngle      = 0.f;

    // Up
    if( Angle > upAngle - m_DirectionalChangeRotationEpsilon && Angle < upAngle + m_DirectionalChangeRotationEpsilon )
    {
        return EInputEntry::Up;
    }

    // Up-forward
    if( Angle >= upAngle + m_DirectionalChangeRotationEpsilon && Angle < forwardAngle - m_DirectionalChangeRotationEpsilon )
    {
        return EInputEntry::UpForward;
    }

    // Forward
    if( Angle > forwardAngle - m_DirectionalChangeRotationEpsilon && Angle < forwardAngle + m_DirectionalChangeRotationEpsilon )
    {
        return EInputEntry::Forward;
    }

    // Forward-down
    if( Angle >= forwardAngle + m_DirectionalChangeRotationEpsilon && Angle < downAngle - m_DirectionalChangeRotationEpsilon )
    {
        return EInputEntry::ForwardDown;
    }

    // Down
    if( (Angle >= downAngle && Angle >= downAngle - m_DirectionalChangeRotationEpsilon) ||
        (Angle > -downAngle && Angle < -downAngle + m_DirectionalChangeRotationEpsilon) )
    {
        return EInputEntry::Down;
    }

    // Down-back
    if( Angle > -downAngle + m_DirectionalChangeRotationEpsilon && Angle < backAngle - m_DirectionalChangeRotationEpsilon )
    {
        return EInputEntry::DownBackward;
    }

    // Back
    if( Angle > backAngle - m_DirectionalChangeRotationEpsilon && Angle < backAngle + m_DirectionalChangeRotationEpsilon )
    {
        return EInputEntry::Backward;
    }

    // Back-Up
    if( Angle >= backAngle + m_DirectionalChangeRotationEpsilon && Angle < upAngle - m_DirectionalChangeRotationEpsilon )
    {
        return EInputEntry::BackwardUp;
    }

    return EInputEntry::None;
}

void UMovesBufferComponent::OnInputRouteEnded( TObjectPtr<UInputsSequence> InputsSequence )
{
    AddToInputsSequenceBuffer( InputsSequence->m_Name, InputsSequence->m_Priority );
}

void UMovesBufferComponent::AddToInputBuffer( EInputEntry InputEntry, EInputPhase Phase )
{
    EInputEntry targetEntry = m_OwnerCharacter->IsFacingRight() ? InputEntry : MirrorInputEntry( InputEntry );

    m_InputsBuffer.Emplace( FInputBufferEntry( targetEntry, Phase, false ) );
    m_InputsBuffer.RemoveAt( 0 );

    m_IBBufferChanged = true;

    if( InputEntry != EInputEntry::None )
    {
        auto result = m_InputSequenceResolver->RegisterInput( targetEntry );

        switch( result )
        {
            case EInputRegistrationResult::InputNotFound:
                {
                    m_InputSequenceResolver->RegisterInput( targetEntry );
                    break;
                }

            default: break;
        }
    }
}

bool UMovesBufferComponent::InputBufferContainsConsumable( EInputEntry InputEntry ) const
{
    for( const FInputBufferEntry& entry : m_InputsBuffer )
    {
        if( entry.m_InputEntry == InputEntry && !entry.m_Used )
        {
            return true;
        }
    }

    return false;
}

void UMovesBufferComponent::AddToInputsSequenceBuffer( const FString& InputsSequenceName, int32 Priority )
{
    m_InputsSequenceBuffer.Emplace( FInputsSequenceBufferEntry( InputsSequenceName, Priority, false ) );
    m_InputsSequenceBuffer.RemoveAt( 0 );

    m_ISBBufferChanged = true;
}

bool UMovesBufferComponent::InputsSequenceBufferContainsConsumable( const FString& MoveName )
{
    for( const FInputsSequenceBufferEntry& entry : m_InputsSequenceBuffer )
    {
        if( entry.m_InputsSequenceName == MoveName && !entry.m_Used )
        {
            return true;
        }
    }

    return false;
}

void UMovesBufferComponent::AddUniqueToHoldInputsMap( EInputEntry InputEntry )
{
    if( !m_HoldInputsMap.Contains( InputEntry ) )
    {
        m_HoldInputsMap.Emplace( InputEntry );
    }
}

void UMovesBufferComponent::RemoveFromHoldInputsMap( EInputEntry InputEntry )
{
    if( m_HoldInputsMap.Contains( InputEntry ) )
    {
        m_HoldInputsMap[InputEntry].m_Expired = true;
    }
}

void UMovesBufferComponent::ClearInputsBuffer()
{
    m_InputsBuffer.Empty();
}

void UMovesBufferComponent::InitInputBuffer()
{
    ClearInputsBuffer();

    for( int32 i = 0; i < m_InputBufferSizeFrames; ++i )
    {
        m_InputsBuffer.Emplace( FInputBufferEntry( EInputEntry::None, EInputPhase::Pressed, false ) );
    }
}

void UMovesBufferComponent::UseBufferedInputsSequence( const FString& InputsSequenceName )
{
    verify( InputsSequenceBufferContainsConsumable( InputsSequenceName ) );

    for( FInputsSequenceBufferEntry& entry : m_InputsSequenceBuffer )
    {
        if( entry.m_InputsSequenceName == InputsSequenceName )
        {
            entry.m_Used = true;
        }
    }
}

void UMovesBufferComponent::UseBufferedInputsSequence( const FInputsSequenceBufferEntry& Entry, bool UseId )
{
    for( FInputsSequenceBufferEntry& entry : m_InputsSequenceBuffer )
    {
        if( UseId )
        {
            if( entry.m_UniqueId == Entry.m_UniqueId )
            {
                entry.m_Used = true;
            }
        }
        else
        {
            if( entry.m_InputsSequenceName == Entry.m_InputsSequenceName )
            {
                entry.m_Used = true;
            }
        }
    }
}

void UMovesBufferComponent::ClearInputsSequenceBuffer()
{
    m_InputsSequenceBuffer.Empty();
}

void UMovesBufferComponent::InitInputsSequenceBuffer()
{
    ClearInputsSequenceBuffer();

    for( int i = 0; i < m_InputsSequencesBufferSizeFrames; ++i )
    {
        m_InputsSequenceBuffer.Emplace( FInputsSequenceBufferEntry( FInputsSequenceBufferEntry::s_SequenceNone, 0, false ) );
    }
}

void UMovesBufferComponent::GetInputsSequenceBufferSnapshot( TArray<FInputsSequenceBufferEntry>& OutEntries, bool SkipEmptyEntries, bool SkipUsedEntries,
                                                             bool UniqueEntries )
{
    OutEntries.Reset();

    for( const auto& entry : m_InputsSequenceBuffer )
    {
        bool currentEntryIsNone = entry.m_InputsSequenceName == FInputsSequenceBufferEntry::s_SequenceNone;

        if( SkipEmptyEntries && currentEntryIsNone )
        {
            continue;
        }

        if( SkipUsedEntries && entry.m_Used )
        {
            continue;
        }

        if( UniqueEntries )
        {
            auto predIsUniqueEntry = [&]( const FInputsSequenceBufferEntry& _entry )
            {
                return _entry.m_InputsSequenceName == entry.m_InputsSequenceName;
            };

            if( !OutEntries.ContainsByPredicate( predIsUniqueEntry ) )
            {
                OutEntries.Emplace( entry );
            }
        }
        else
        {
            OutEntries.Emplace( entry );
        }
    }
}

bool UMovesBufferComponent::IsInputsSequenceBuffered( const FString& InputsSequenceName, bool ConsumeEntry /*= true*/ )
{
    for( int32 i = 0; i < m_InputsSequenceBuffer.Num(); ++i )
    {
        FInputsSequenceBufferEntry& entry = m_InputsSequenceBuffer[i];

        bool isValid     = entry.m_InputsSequenceName != FInputsSequenceBufferEntry::s_SequenceNone;
        bool hasSameName = entry.m_InputsSequenceName == InputsSequenceName;
        bool canBeUsed   = !entry.m_Used;

        if( isValid && hasSameName && canBeUsed )
        {
            if( ConsumeEntry )
            {
                entry.m_Used = true;
            }

            return true;
        }
    }

    return false;
}

void UMovesBufferComponent::OnMoveHorizontal( float Value )
{
    if( FMath::Abs( Value ) > m_AnalogMovementDeadzone )
    {
        m_InputMovement = Value;

        m_MovingRight = FMath::Sign( Value ) > 0;
        m_MovingLeft  = FMath::Sign( Value ) < 0;
    }
}

// TODO: these are all the same, make it generic maybe?
void UMovesBufferComponent::OnJumpPressed()
{
    AddToInputBuffer( EInputEntry::Jump, EInputPhase::Pressed );
    AddUniqueToHoldInputsMap( EInputEntry::Jump );
}

void UMovesBufferComponent::OnJumpReleased()
{
    RemoveFromHoldInputsMap( EInputEntry::Jump );
}

void UMovesBufferComponent::OnAttackPressed()
{
    AddToInputBuffer( EInputEntry::Attack, EInputPhase::Pressed );
    AddUniqueToHoldInputsMap( EInputEntry::Attack );
}

void UMovesBufferComponent::OnAttackReleased()
{
    RemoveFromHoldInputsMap( EInputEntry::Attack );
}

void UMovesBufferComponent::OnSpecialPressed()
{
    AddToInputBuffer( EInputEntry::Special, EInputPhase::Pressed );
    AddUniqueToHoldInputsMap( EInputEntry::Special );
}

void UMovesBufferComponent::OnSpecialReleased()
{
    RemoveFromHoldInputsMap( EInputEntry::Special );
}

void UMovesBufferComponent::OnCounterPressed()
{
    AddToInputBuffer( EInputEntry::Counter, EInputPhase::Pressed );
    AddUniqueToHoldInputsMap( EInputEntry::Counter );
}

void UMovesBufferComponent::OnCounterReleased()
{
    RemoveFromHoldInputsMap( EInputEntry::Counter );
}

void UMovesBufferComponent::UpdateMovementDirection()
{
    if( (m_MovingRight && m_MovingLeft) || (!m_MovingRight && !m_MovingLeft) )
    {
        m_MovementDirection = 0.f;
    }
    else
    {
        if( m_MovingRight )
        {
            m_MovementDirection = 1.f;
        }
        else if( m_MovingLeft )
        {
            m_MovementDirection = -1.f;
        }
    }
}

void UMovesBufferComponent::UpdateDirectionalInputs( UInputComponent* InputComponent )
{
    m_DirectionalInputVector.X = InputComponent->GetAxisValue( TEXT( "MoveHorizontal" ) );
    m_DirectionalInputVector.Y = InputComponent->GetAxisValue( TEXT( "MoveVertical" ) );

    if( m_DirectionalInputVector.Length() > m_MinDirectionalInputVectorLength )
    {
        float angle       = UMathStatics::GetSignedAngle( m_DirectionalInputVector, FVector2d( 0.f, 1.f ) );
        EInputEntry entry = GetDirectionalInputEntryFromAngle( angle );

        if( loc_ShowDirectionalAngle )
        {
            FG_TEXT( GetWorld(), GetOwner()->GetActorLocation(),
                     FString::Printf( TEXT( "[Input: %s]" ), *UConversionStatics::ConvertEnumValueToString( entry, false ) ) );
        }

        if( entry != m_LastDirectionalInputEntry )
        {
            m_LastDirectionalInputEntry = entry;

            AddToInputBuffer( entry, EInputPhase::Pressed );
        }
    }
    else
    {
        if( m_LastDirectionalInputEntry != EInputEntry::Neutral )
        {
            m_LastDirectionalInputEntry = EInputEntry::Neutral;

            AddToInputBuffer( EInputEntry::Neutral, EInputPhase::Pressed );
        }
    }

    m_LastDirectionalInputVector = m_DirectionalInputVector;
}

void UMovesBufferComponent::UseBufferedInput( EInputEntry Input )
{
    verify( InputBufferContainsConsumable( Input ) );

    for( FInputBufferEntry& entry : m_InputsBuffer )
    {
        if( entry.m_InputEntry == Input )
        {
            entry.m_Used = true;
        }
    }
}

void UMovesBufferComponent::UseBufferedInput( int32 UniqueId )
{
}
