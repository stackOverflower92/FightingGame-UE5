#pragma once

#include "CoreMinimal.h"
#include "FightingGame/FightingGameGameModeBase.h"
#include "FreeForAllGameMode.generated.h"

enum class EDeathReason : uint8;
class AIndexedPlayerStart;
class ACameraManager;
class AFightingCharacter;

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

    /*
     * This only works with exactly 2 players
     */
    UPROPERTY( EditAnywhere, BlueprintReadWrite, DisplayName = "Enable characters auto-facing" )
    bool m_EnableCharactersAutoFacing = false;

    UPROPERTY( EditAnywhere, BlueprintReadWrite, DisplayName = "Damage Increases Characters Percent Value" )
    bool m_DamageIncreasesCharactersPercent = false;

private:
    TArray<TObjectPtr<AIndexedPlayerStart>> m_PlayerStarts;
    TArray<TObjectPtr<APlayerController>> m_PlayerControllers;
    TArray<TObjectPtr<AFightingCharacter>> m_Characters;

    void SpawnCharacters();
    void EnablePlayersInput( bool Enable );

    void OnCharacterDeath( TObjectPtr<AFightingCharacter> Character, EDeathReason Reason );
};
