// Copyright (c) Giammarco Agazzotti

#include "GlobalDelegates.h"

GlobalDelegates& GlobalDelegates::Get()
{
    static GlobalDelegates instance;
    return instance;
}
