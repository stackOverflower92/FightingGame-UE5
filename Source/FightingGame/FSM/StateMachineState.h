// Copyright (c) Giammarco Agazzotti

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "StateMachineState.generated.h"

class UStateMachineComponent;

UCLASS( Abstract, Blueprintable, BlueprintType, HideCategories = ("Cooking", "LOD", "Physics", "Activation", "Tags", "Rendering") )
class FIGHTINGGAME_API UStateMachineState : public UObject
{
    GENERATED_BODY()

public:
    UPROPERTY( EditAnywhere, BlueprintReadOnly, DisplayName = "State Id" )
    FName m_Id;

    UPROPERTY()
    TObjectPtr<UStateMachineComponent> m_FSM = nullptr;

    UPROPERTY( EditAnywhere, BlueprintReadOnly, DisplayName = "Repeatable" )
    bool m_Repeatable = true;

    UFUNCTION( BlueprintImplementableEvent, meta=(DisplayName="On Init"), Category = "State Machine|State" )
    void ReceiveOnInit();

    virtual void OnInit();

    UFUNCTION( BlueprintImplementableEvent, meta=(DisplayName="On Enter"), Category = "State Machine|State" )
    void ReceiveOnEnter();

    virtual void OnEnter();

    UFUNCTION( BlueprintImplementableEvent, meta=(DisplayName="On Exit"), Category = "State Machine|State" )
    void ReceiveOnExit();

    virtual void OnExit();

    UFUNCTION( BlueprintImplementableEvent, meta=(DisplayName="On Tick"), Category = "State Machine|State" )
    void ReceiveOnTick( float DeltaTime );

    virtual void OnTick( float DeltaTime );
};
