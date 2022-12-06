// Copyright (c) Giammarco Agazzotti

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "FightingCharacterStateTransition.generated.h"

enum class EMontageEventType : uint8;
class AFightingCharacter;

UCLASS()
class FIGHTINGGAME_API UFightingCharacterStateTransition : public UDataAsset
{
	GENERATED_BODY()

public:
	virtual void OnStateEnter( TObjectPtr<AFightingCharacter> Character );
	virtual void OnStateExit();
	virtual void OnMontageEvent( UAnimMontage* Montage, EMontageEventType MontageEvent );
	virtual void OnGrounded();
	virtual void OnAirborne();

	virtual bool CanPerformTransition();

protected:
	TObjectPtr<AFightingCharacter> m_Character = nullptr;
};
