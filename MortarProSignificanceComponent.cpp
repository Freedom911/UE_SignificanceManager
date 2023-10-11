//Copyright(c) 2023 FinalRockstar All Rights Reserved


#include "MortarProSignificanceComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "SignificanceManager.h"

UMortarProSignificanceComponent::UMortarProSignificanceComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	bManageSignificance = true;
	bInsignificantWhenOwnerIsHidden = true;

	//Add BY default Highest significance value
	SignificanceDistanceThresholds.Emplace(FMortarProSignificanceDistance(EMortarProSignificanceValue::Highest, 5000.f));
}


void UMortarProSignificanceComponent::BeginPlay()
{
	Super::BeginPlay();

	// Register only if user selects to use significance Manager
	if (bManageSignificance)
	{
		RegisterToSignificanceManager();
	}
}

//Register To significance Manager
void UMortarProSignificanceComponent::RegisterToSignificanceManager()
{
	RegisterObjectTag = "";

	AActor* MortarOwner = GetOwner();

	USignificanceManager* SignificanceManager = USignificanceManager::Get(GetWorld());

	if (SignificanceManager && MortarOwner)
	{
		auto Significance = [&](USignificanceManager::FManagedObjectInfo* ObjectInfo, const FTransform& Viewpoint) -> float
			{
				return SignficanceFunction(ObjectInfo, Viewpoint);
			};

		auto PostSignificance = [&](USignificanceManager::FManagedObjectInfo* ObjectInfo, float OldSignificance, float Significance, bool bFinal)
			{
				PostSignficanceFunction(ObjectInfo, OldSignificance, Significance, bFinal);
			};

		RegisterObjectTag = MortarOwner->GetClass()->GetFName();
		SignificanceManager->RegisterObject(this, RegisterObjectTag, Significance, USignificanceManager::EPostSignificanceType::Sequential, PostSignificance);
	}

}

//Unregister Significance Manager when actor is being destroyed
void UMortarProSignificanceComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (bManageSignificance)
	{
		// Unregister
		if (USignificanceManager* SignificanceManager = USignificanceManager::Get(GetWorld()))
		{
			SignificanceManager->UnregisterObject(this);
		}
	}

	Super::EndPlay(EndPlayReason);
}


//Return Significance Value based on Distance
float UMortarProSignificanceComponent::SignficanceFunction(USignificanceManager::FManagedObjectInfo* ObjectInfo, const FTransform& Viewpoint)
{

	if (ObjectInfo->GetTag() == RegisterObjectTag)
	{
		AActor* MortarOwner = GetOwner();
		if (MortarOwner)
		{
			//Check if the actor is hidden
			if (bInsignificantWhenOwnerIsHidden && GetOwner()->IsHidden())
			{
				return static_cast<float>(EMortarProSignificanceValue::Hidden);
			}


			FVector ActorLocation = GetOwner()->GetActorLocation();
			const float DistanceSqrd = (ActorLocation - Viewpoint.GetLocation()).SizeSquared();

			return GetSignificanceByDistance(DistanceSqrd);
		}
	}
	return 0.f;
}

//POst Significance Function
void UMortarProSignificanceComponent::PostSignficanceFunction(USignificanceManager::FManagedObjectInfo* ObjectInfo, float OldSignificance, float Significance, bool bFinal)
{
	if (ObjectInfo->GetTag() == RegisterObjectTag)
	{
		//Emit Event When Significance Value CHanges from Previous
		if (Significance != OldSignificance)
		{
			CurrentSignificance = static_cast<EMortarProSignificanceValue>(Significance);
			UE_LOG(LogMortarProPlugin, Log, TEXT("Significance for %s changed to %s"), *GetOwner()->GetName(), *UEnum::GetValueAsString(CurrentSignificance));
			OnSignificanceChanged.Broadcast(CurrentSignificance);

		}
	}
}

float UMortarProSignificanceComponent::GetSignificanceByDistance(const float& DistanceSqrd) const
{
	const int32 NumThresholds = SignificanceDistanceThresholds.Num();
	if (NumThresholds == 0)
	{
		return static_cast<float>(EMortarProSignificanceValue::Highest);
	}

	//Check for Last Threshold value
	if (DistanceSqrd >= SignificanceDistanceThresholds[NumThresholds - 1].GetMaxDistSqrd())
	{
		// Max distance reached assume lowest significance
		return static_cast<float>(EMortarProSignificanceValue::Lowest);
	}

	for (int32 Idx = 0; Idx < NumThresholds; Idx++)
	{
		const FMortarProSignificanceDistance& Item = SignificanceDistanceThresholds[Idx];
		if (DistanceSqrd <= (Item.GetMaxDistSqrd()))
		{
			return static_cast<float>(Item.SignificanceValue);
		}
	}

	return static_cast<float>(EMortarProSignificanceValue::Highest);
}

