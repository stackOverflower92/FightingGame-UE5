// Copyright (c) Giammarco Agazzotti

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharacterHealthBarWidget.generated.h"

class UProgressBar;
class UTextBlock;
class AFightingCharacter;

UCLASS( Abstract )
class FIGHTINGGAME_API UCharacterHealthBarWidget : public UUserWidget
{
    GENERATED_BODY()

protected:
    UPROPERTY( EditAnywhere, BlueprintReadWrite )
    int32 CharacterNumber = 0;

    UPROPERTY( EditAnywhere, meta=(BindWidget) )
    TObjectPtr<UTextBlock> NameText = nullptr;

    UPROPERTY( EditAnywhere, meta=(BindWidget) )
    TObjectPtr<UProgressBar> HPGauge = nullptr;

    TObjectPtr<AFightingCharacter> m_Character = nullptr;

    virtual void NativeConstruct() override;
    virtual void NativeTick( const FGeometry& MyGeometry, float InDeltaTime ) override;
};
