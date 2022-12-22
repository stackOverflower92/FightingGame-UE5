#pragma once

#include "CoreMinimal.h"
#include "FightingGame/FightingGameGameModeBase.h"
#include "FreeForAllGameMode.generated.h"

class ACameraManager;
class AFightingCharacter;
class APlayerStart;

// #TODO check which part of this class can be moved to GameState

UCLASS()
class FIGHTINGGAME_API AFreeForAllGameMode : public AFightingGameGameModeBase
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = Classes, DisplayName = "Character Class" )
	TSubclassOf<AFightingCharacter> m_CharacterClass;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, DisplayName = "Additional Players" )
	int m_AdditionalPlayers = 0;

private:
	TArray<TObjectPtr<APlayerStart>> m_PlayerStarts;
	TArray<TObjectPtr<APlayerController>> m_PlayerControllers;
	TArray<TObjectPtr<AFightingCharacter>> m_Characters;

	void SpawnCharacters();
	void EnablePlayersInput( bool Enable );
};
