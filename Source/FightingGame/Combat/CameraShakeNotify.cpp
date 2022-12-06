// Copyright (c) Giammarco Agazzotti


#include "CameraShakeNotify.h"

#include "Kismet/GameplayStatics.h"

void UCameraShakeNotify::Notify( USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference )
{
	Super::Notify( MeshComp, Animation, EventReference );

	if( auto* PC = UGameplayStatics::GetPlayerController( MeshComp->GetWorld(), 0 ) )
	{
		PC->PlayerCameraManager->StartCameraShake( m_Shake );
	}
}
