// Copyright (c) Giammarco Agazzotti

#pragma once

#include "CoreMinimal.h"
#include "InputsSequence.h"
#include "Engine/DataTable.h"
#include "InputsSequenceStateMappingRow.generated.h"

USTRUCT( BlueprintType )
struct FIGHTINGGAME_API FInputsSequenceStateMappingRow : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY( EditAnywhere, BlueprintReadOnly, DisplayName = "Input Sequence" )
    TObjectPtr<UInputsSequence> m_InputsSequence = nullptr;

    UPROPERTY( EditAnywhere, BlueprintReadOnly, DisplayName = "State Name" )
    FName m_StateName;

    UPROPERTY( EditAnywhere, BlueprintReadOnly, DisplayName = "Allow When Grounded" )
    bool m_AllowWhenGrounded = true;

    UPROPERTY( EditAnywhere, BlueprintReadOnly, DisplayName = "Allow When Airborne" )
    bool m_AllowWhenAirborne = false;

    UPROPERTY( EditAnywhere, BlueprintReadOnly, DisplayName = "Allow As Cancel On Hit" )
    bool m_AllowAsOnHitCancel = false;
};
