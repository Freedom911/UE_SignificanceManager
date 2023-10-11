//Copyright(c) 2023 FinalRockstar All Rights Reserved

#include "MortarProViewportClient.h"

#include "SignificanceManager.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"

UMortarProViewportClient::UMortarProViewportClient(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UMortarProViewportClient::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UWorld* MyWorld = GetWorld();
	if(MyWorld != nullptr)
	{
		const bool bHasValidData = CachedPlayerController.IsValid();
		if (!bHasValidData)
		{
			APlayerController* PC = UGameplayStatics::GetPlayerController(MyWorld, 0);
			CachedPlayerController = TWeakObjectPtr<class APlayerController>(PC);
		}

		if (bHasValidData)
		{
			//FSignificanceManagerModule a;
			USignificanceManager* SignificanceManager = USignificanceManager::Get(MyWorld);
			if (SignificanceManager)
			{
				FVector ViewLocation;
				FRotator ViewRotation;
				CachedPlayerController.Get()->GetPlayerViewPoint(ViewLocation, ViewRotation);

				TArray<FTransform> Viewpoints;
				Viewpoints.Emplace(ViewRotation, ViewLocation, FVector::OneVector);

				SignificanceManager->Update(Viewpoints);
			}
		}
	}
}