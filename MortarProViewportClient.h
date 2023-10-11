//Copyright(c) 2023 FinalRockstar All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameViewportClient.h"
#include "MortarProViewportClient.generated.h"

/**
 * @class UMortarProViewportClient
 * Class Inheriting From UGameViewportClient. In this class we are only updating
 * viewport points in tick
 */
UCLASS()
class MORTARPROPLUGIN_API UMortarProViewportClient : public UGameViewportClient
{
	GENERATED_UCLASS_BODY()
	
public:

	/**
	 * Will be updating Significance manager here 
	 */
	virtual void Tick(float DeltaTime) override;


protected:
	/** 
	 * Cached ptr to our player controller that we should get the view from 
	 * */
	TWeakObjectPtr<class APlayerController> CachedPlayerController;
};