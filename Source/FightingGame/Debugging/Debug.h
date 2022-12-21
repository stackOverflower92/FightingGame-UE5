#pragma once

#define FG_CVAR(Var, CVarName, VarRef) FAutoConsoleVariableRef Var(CVarName, VarRef, TEXT(""));
#define FG_CVAR_DESC(Var, CVarName, CVarDesc, VarRef) FAutoConsoleVariableRef Var(CVarName, VarRef, CVarDesc);
#define FG_CVAR_FLAG_DESC(Var, CVarName, VarRef) FG_CVAR_DESC(##Var, ##CVarName, TEXT("1: Enable, 0: Disable"), ##VarRef)

#define FG_SLOG_INFO(Message) GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::White, Message)
#define FG_SLOG_WARN(Message) GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Yellow, Message)
#define FG_SLOG_ERR(Message) GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, Message)
