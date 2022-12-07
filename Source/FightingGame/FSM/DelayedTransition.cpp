// Copyright (c) Giammarco Agazzotti


#include "DelayedTransition.h"

void UDelayedTransition::OnStateEnter()
{
	Super::OnStateEnter();

	m_CanPerform = false;

	ClearTimerIfActive();

	GetWorld()->GetTimerManager().SetTimer( m_TimerHandle, this, &UDelayedTransition::OnTimerEnded, m_Delay );
}

void UDelayedTransition::OnStateExit()
{
	Super::OnStateExit();

	ClearTimerIfActive();
}

bool UDelayedTransition::CanPerformTransition()
{
	return m_CanPerform;
}

void UDelayedTransition::ClearTimerIfActive()
{
	if( GetWorld()->GetTimerManager().IsTimerActive( m_TimerHandle ) )
	{
		GetWorld()->GetTimerManager().ClearTimer( m_TimerHandle );
	}
}

void UDelayedTransition::OnTimerEnded()
{
	m_CanPerform = true;
}
