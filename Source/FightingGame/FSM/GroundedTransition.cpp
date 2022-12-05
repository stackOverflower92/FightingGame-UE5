// Copyright (c) Giammarco Agazzotti


#include "GroundedTransition.h"

bool UGroundedTransition::CanPerformTransition()
{
	return m_CanTransition;
}

void UGroundedTransition::OnStateEnter()
{
	Super::OnStateEnter();

	m_CanTransition = false;
}

void UGroundedTransition::OnGrounded()
{
	Super::OnGrounded();

	m_CanTransition = true;
}
