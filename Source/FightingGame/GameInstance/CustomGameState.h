// Copyright (c) Giammarco Agazzotti

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "CustomGameState.generated.h"

class UCustomGameInstance;

UCLASS( Abstract, Blueprintable, BlueprintType, HideCategories = ("Cooking", "LOD", "Physics", "Activation", "Tags", "Rendering") )
class FIGHTINGGAME_API UCustomGameState : public UObject
{
    GENERATED_BODY()

public:
    UPROPERTY( EditAnywhere, BlueprintReadOnly, DisplayName = "State Id" )
    FName m_Id;

    UPROPERTY()
    TObjectPtr<UCustomGameInstance> m_GameInstance = nullptr;

    UPROPERTY( EditAnywhere, BlueprintReadOnly, DisplayName = "Repeatable" )
    bool m_Repeatable = false;

    UFUNCTION( BlueprintImplementableEvent, meta=(DisplayName="On Init"), Category = "Game State" )
    void ReceiveOnInit();

    virtual void OnInit();

    UFUNCTION( BlueprintImplementableEvent, meta=(DisplayName="On Enter"), Category = "Game State" )
    void ReceiveOnEnter();

    virtual void OnEnter();

    UFUNCTION( BlueprintImplementableEvent, meta=(DisplayName="On Exit"), Category = "Game State" )
    void ReceiveOnExit();

    virtual void OnExit();

    UFUNCTION( BlueprintImplementableEvent, meta=(DisplayName="On Tick"), Category = "Game State" )
    void ReceiveOnTick( float DeltaTime );

    virtual void OnTick( float DeltaTime );
};
