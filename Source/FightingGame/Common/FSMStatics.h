// Copyright (c) Giammarco Agazzotti

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FSMStatics.generated.h"

class UFSM;

UCLASS()
class FIGHTINGGAME_API UFSMStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION( BlueprintCallable, Category = "FSM" )
	static bool Init( UFSM* Fsm, FName FirstStateName );

	UFUNCTION( BlueprintCallable, Category = "FSM" )
	static bool SetState( UFSM* Fsm, FName StateName );

	UFUNCTION( BlueprintCallable, Category = "FSM" )
	static bool PushState( UFSM* Fsm, FName StateName );

	UFUNCTION( BlueprintCallable, Category = "FSM" )
	static bool PopState( UFSM* Fsm );

private:
	static bool IsFSMValid( UFSM* Fsm );
};
