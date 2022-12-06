// Copyright (c) Giammarco Agazzotti

#pragma once

#include "CoreMinimal.h"
#include "FightingCharacterStateTransition.generated.h"

enum class EMontageEventType : uint8;
class AFightingCharacter;

UCLASS( Abstract, Blueprintable, BlueprintType, HideCategories = ("Cooking", "LOD", "Physics", "Activation", "Tags", "Rendering") )
class FIGHTINGGAME_API UFightingCharacterStateTransition : public UObject
{
	GENERATED_BODY()

public:
	virtual void OnInit( TObjectPtr<AFightingCharacter> Character );
	virtual void OnStateEnter();
	virtual void OnStateExit();

	virtual void OnMontageEvent( UAnimMontage* Montage, EMontageEventType MontageEvent );
	virtual void OnGrounded();
	virtual void OnAirborne();

	virtual bool CanPerformTransition();

protected:
	TObjectPtr<AFightingCharacter> m_Character = nullptr;
};
