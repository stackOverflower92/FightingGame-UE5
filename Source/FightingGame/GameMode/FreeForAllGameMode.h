#pragma once

#include "CoreMinimal.h"
#include "FightingGame/FightingGameGameModeBase.h"
#include "FreeForAllGameMode.generated.h"

class ACameraManager;
class AFightingCharacter;
class APlayerStart;

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
	UPROPERTY()
	TArray<APlayerStart*> m_PlayerStarts;

	UPROPERTY()
	TArray<APlayerController*> m_PlayerControllers;

	UPROPERTY()
	TArray<AFightingCharacter*> m_Characters;

	void SpawnCharacters();
	void EnablePlayersInput( bool Enable );
};
