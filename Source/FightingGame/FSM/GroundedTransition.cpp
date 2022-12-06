// Copyright (c) Giammarco Agazzotti


#include "GroundedTransition.h"

bool UGroundedTransition::CanPerformTransition()
{
	return m_CanTransition;
}

void UGroundedTransition::OnStateEnter( TObjectPtr<AFightingCharacter> Character )
{
	Super::OnStateEnter( Character );

	m_CanTransition = false;
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
