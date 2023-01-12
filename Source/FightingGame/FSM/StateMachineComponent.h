// Copyright (c) Giammarco Agazzotti

#pragma once

#include "CoreMinimal.h"
#include "StateMachineState.h"
#include "Components/ActorComponent.h"
#include "StateMachineComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FIGHTINGGAME_API UStateMachineComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UStateMachineComponent();

protected:
    UPROPERTY( EditAnywhere, BlueprintReadOnly, DisplayName = "States Classes", Category = "State Machine" )
    TArray<TSubclassOf<UStateMachineState>> m_States;

    UPROPERTY( EditAnywhere, BlueprintReadOnly, DisplayName = "Start On BeginPlay", Category = "State Machine" )
    bool m_StartOnBeginPlay = false;

    UPROPERTY( EditAnywhere, BlueprintReadOnly, DisplayName = "First State Name", Category = "State Machine" )
    FName m_FirstStateName;

    virtual void BeginPlay() override;

public:
    void Start();

    void SetState( const FName& StateId );

    virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

private:
    bool m_Started = false;
    TArray<TObjectPtr<UStateMachineState>> m_StatesInstances;
    TObjectPtr<UStateMachineState> m_CurrentState = nullptr;

    TObjectPtr<UStateMachineState> GetStateById( const FName& Id );
};
