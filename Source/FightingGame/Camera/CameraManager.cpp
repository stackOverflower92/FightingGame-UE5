// Copyright (c) Giammarco Agazzotti


#include "CameraManager.h"

#include "GameCamera.h"
#include "Kismet/GameplayStatics.h"

ACameraManager::ACameraManager()
{
    PrimaryActorTick.bCanEverTick = true;
}

void ACameraManager::BeginPlay()
{
    Super::BeginPlay();
}

void ACameraManager::Tick( float DeltaTime )
{
    Super::Tick( DeltaTime );
}

void ACameraManager::Init()
{
    TArray<TObjectPtr<AActor>> AllGameCameraActors;
    UGameplayStatics::GetAllActorsOfClass( GetWorld(), AGameCamera::StaticClass(), AllGameCameraActors );

    for( int32 i = 0; i < AllGameCameraActors.Num(); ++i )
    {
        m_GameCameras.Emplace( Cast<AGameCamera>( AllGameCameraActors[i] ) );
    }

    if( !m_FirstCameraId.IsEmpty() )
    {
        if( !SetCamera( m_FirstCameraId ) )
        {
            UE_LOG( LogTemp, Error, TEXT("Could not set camera with id [%s]"), *m_FirstCameraId );
        }
    }
}

bool ACameraManager::SetCamera( FString Id )
{
    if( APlayerController* PC = UGameplayStatics::GetPlayerController( GetWorld(), 0 ) )
    {
        auto* It = m_GameCameras.FindByPredicate( [&]( AGameCamera* Camera )
        {
            return Camera->GetId() == Id;
        } );

        if( !It ) return false;

        PC->SetViewTargetWithBlend( *It );
        (*It)->OnViewSet();

        return true;
    }

    return false;
}
