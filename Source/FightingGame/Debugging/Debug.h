#pragma once

#if UE_BUILD_SHIPPING

#define FG_CVAR(Var, CVarName, VarRef)
#define FG_CVAR_DESC(Var, CVarName, CVarDesc, VarRef)
#define FG_CVAR_FLAG_DESC(Var, CVarName, VarRef)

#define FG_SLOG_KEY(Key, Message, Color)

#define FG_SLOG(Message, Color)
#define FG_SLOG_INFO(Message)
#define FG_SLOG_WARN(Message)
#define FG_SLOG_ERR(Message)

#define FG_TEXT(World, Location, Text)

#else

#define FG_CVAR(Var, CVarName, VarRef) FAutoConsoleVariableRef Var(CVarName, VarRef, TEXT(""))
#define FG_CVAR_DESC(Var, CVarName, CVarDesc, VarRef) FAutoConsoleVariableRef Var(CVarName, VarRef, CVarDesc)
#define FG_CVAR_FLAG_DESC(Var, CVarName, VarRef) FG_CVAR_DESC(##Var, ##CVarName, TEXT("1: Enable, 0: Disable"), ##VarRef)

#define FG_SLOG_KEY(Key, Message, Color) GEngine->AddOnScreenDebugMessage(##Key, 1.f, ##Color, ##Message)

#define FG_SLOG(Message, Color) FG_SLOG_KEY(-1, ##Message, ##Color)
#define FG_SLOG_INFO(Message) FG_SLOG(##Message, FColor::White)
#define FG_SLOG_WARN(Message) FG_SLOG(##Message, FColor::Yellow)
#define FG_SLOG_ERR(Message) FG_SLOG(##Message, FColor::Red)


#define FG_TEXT(World, Location, Text) UKismetSystemLibrary::DrawDebugString(##World, ##Location, ##Text)

#endif
