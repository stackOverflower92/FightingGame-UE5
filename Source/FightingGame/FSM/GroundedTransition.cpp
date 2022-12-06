// Copyright (c) Giammarco Agazzotti


#include "GroundedTransition.h"

void UGroundedTransition::OnStateEnter()
{
	Super::OnStateEnter();

	m_CanTransition = false;
}

bool UGroundedTransition::CanPerformTransition()
{
	return m_CanTransition;
}

void UGroundedTransition::OnGrounded()
{
	Super::OnGrounded();

	if( m_MustBeGrounded )
	{
		m_CanTransition = true;
	}
}

void UGroundedTransition::OnAirborne()
{
	Super::OnAirborne();

	if( !m_MustBeGrounded )
	{
		m_CanTransition = true;
	}
}
