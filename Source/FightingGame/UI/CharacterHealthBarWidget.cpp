// Copyright (c) Giammarco Agazzotti


#include "CharacterHealthBarWidget.h"

#include "EngineUtils.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "FightingGame/Character/FightingCharacter.h"
#include "FightingGame/Debugging/Debug.h"

void UCharacterHealthBarWidget::NativeConstruct()
{
    Super::NativeConstruct();

    for( TActorIterator<AFightingCharacter> it( GetWorld() ); it; ++it )
    {
        if( (*it)->m_PlayerIndex == CharacterNumber )
        {
            m_Character = *it;
            break;
        }
    }

    if( !m_Character )
    {
        FG_SLOG_ERR( TEXT("Could not find player for widget") );
        return;
    }

    NameText->SetText( FText::FromString( m_Character->GetName() ) );
}

void UCharacterHealthBarWidget::NativeTick( const FGeometry& MyGeometry, float InDeltaTime )
{
    Super::NativeTick( MyGeometry, InDeltaTime );

    if( m_Character )
    {
        HPGauge->Percent = m_Character->GetHP() / m_Character->GetInitialHP();
    }
}
