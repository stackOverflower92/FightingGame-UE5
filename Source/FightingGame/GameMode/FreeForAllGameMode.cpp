#include "FreeForAllGameMode.h"

#include "EngineUtils.h"
#include "FightingGame/Camera/CameraManager.h"
#include "FightingGame/Character/FightingCharacter.h"
#include "FightingGame/Common/CombatStatics.h"
#include "FightingGame/Debugging/Debug.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetStringLibrary.h"

void AFreeForAllGameMode::BeginPlay()
{
	Super::BeginPlay();

	SpawnCharacters();
}

void AFreeForAllGameMode::SpawnCharacters()
{
	UWorld* world = GetWorld();
	for( TActorIterator<APlayerStart> it( world ); it; ++it )
	{
		// TODO: missing checks
		m_PlayerStarts.Emplace( *it );
	}

	m_PlayerStarts.Sort( []( const APlayerStart& A, const APlayerStart& B )
	{
		int32 AStartTag = UKismetStringLibrary::Conv_StringToInt( A.PlayerStartTag.ToString().TrimStartAndEnd() );
		int32 BStartTag = UKismetStringLibrary::Conv_StringToInt( B.PlayerStartTag.ToString().TrimStartAndEnd() );

		return AStartTag < BStartTag;
	} );

	for( int i = 0; i < m_AdditionalPlayers + 1; ++i )
	{
		APlayerController* player = i == 0 ? UGameplayStatics::GetPlayerController( world, 0 ) : UGameplayStatics::CreatePlayer( GetWorld() );
		//ABasePlayerState* playerState = Cast<ABasePlayerState>( player->PlayerState );
		//playerState->CustomSetPlayerName( FName( FString::Printf( TEXT( "Player %d" ), i ) ) );

		m_PlayerControllers.Emplace( player );

		APlayerStart* start           = m_PlayerStarts[i];
		AFightingCharacter* character = GetWorld()->SpawnActor<AFightingCharacter>( m_CharacterClass, start->GetTransform() );
		character->m_PlayerIndex      = i;

		m_Characters.Emplace( character );

		player->Possess( character );

		if( auto* facingEntity = Cast<IFacingEntity>( character ) )
		{
			UCombatStatics::FaceLocation( facingEntity, FVector::ZeroVector );
		}
		else
		{
			FG_SLOG_ERR( TEXT("Cast to IFacingEntity from character failed") );
		}
	}

	EnablePlayersInput( true );

	if( m_CameraManager )
	{
		m_CameraManager->Init();
	}
}

void AFreeForAllGameMode::EnablePlayersInput( bool Enable )
{
	for( APlayerController* player : m_PlayerControllers )
	{
		if( auto* Pawn = player->GetPawn() )
		{
			if( Enable )
			{
				Pawn->EnableInput( player );
			}
			else
			{
				Pawn->DisableInput( player );
			}
		}
	}
}
