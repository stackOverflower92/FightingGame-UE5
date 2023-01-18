// Copyright (c) Giammarco Agazzotti

#pragma once

#include "CoreMinimal.h"

DECLARE_MULTICAST_DELEGATE( FGameModeInitializedDelegate )

#define DEFINE_GLOBAL_DELEGATE(DelegateType) \
public: F##DelegateType& Get##DelegateType() { return DelegateType; } \
private: F##DelegateType DelegateType;

#define DEFINE_GLOBAL_DELEGATE_TYPED(DelegateVariable, DelegateType) \
public: DelegateType& Get##DelegateVariable() { return DelegateVariable; } \
private: DelegateType DelegateVariable;

class FIGHTINGGAME_API GlobalDelegates
{
public:
    static GlobalDelegates& Get();

    FGameModeInitializedDelegate m_GameModeInitializedDelegate;
};
