//Copyright(c) 2023 FinalRockstar All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "SignificanceManager.h"
#include "Components/ActorComponent.h"
#include "MortarProSignificanceComponent.generated.h"


/**
 * @enum EMortarProSignificanceValue
 * Enum Containing Significance Value
 */
UENUM(BlueprintType)
enum class EMortarProSignificanceValue : uint8
{
	Hidden = 0, // When Actor is hidden
	Lowest = 1, // When Actor is extremely far
	Medium = 2, // No Longer Required for frequent updates
	Highest = 3 // No Optimization
};


/**
 * Event Generated after Significance Value is Changed
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMortarProEndSignificanceChanged, EMortarProSignificanceValue, Significance);

/**
 * @struct FMortarProGunSocketProperties
 * Structure Containing  Mapping between distance and significance Value
 */
USTRUCT(BlueprintType)
struct FMortarProSignificanceDistance
{
	GENERATED_BODY()

	FMortarProSignificanceDistance()
	{
		SignificanceValue = EMortarProSignificanceValue::Highest;
		MaxDistance = 500.f;
	}

	FMortarProSignificanceDistance(EMortarProSignificanceValue InSignificance, float InMaxDistance) :
		SignificanceValue(InSignificance),
		MaxDistance(InMaxDistance) {}

	/**
	 * Represents the Significance Value
	 */
	UPROPERTY(EditAnywhere, Category = "MortarPro|Optimization")
	EMortarProSignificanceValue SignificanceValue = EMortarProSignificanceValue::Highest;

	/**
	 * Represents The Max Distance at which the significance value will be applied
	 */
	UPROPERTY(EditAnywhere, Category = "MortarPro|Optimization", meta = (UIMin = 500.0, UIMax = 50000))
	float MaxDistance = 1000.f;


	/**
	 * Gets Maximum Distance Squared
	 * @return float value containing square of Maximum Distance
	 */
	float GetMaxDistSqrd() const { return MaxDistance * MaxDistance; }
};



/**
 * Generic Actor Component Class that manages Significance of the owner actor
 * Actor then can handle in its class what to do with significance value.
 * FMortarProEndSignificanceChanged is generated when significance value changes
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent), HideCategories = ("Collision", "Tags", "ComponentTick", "ComponentReplication", "Cooking", "Activation", "AssetUserData"))
class MORTARPROPLUGIN_API UMortarProSignificanceComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UMortarProSignificanceComponent();

	/**
	 * Event Generated whenever significance value is changed
	 */
	UPROPERTY(BlueprintAssignable, Category = "MortarPro|Optimization")
	FMortarProEndSignificanceChanged OnSignificanceChanged;

	/**
	 *  Whether to register with Significance Manager to optimize Tick.
	 *  By Default value is true
	 */
	UPROPERTY(EditAnywhere, Category = "MortarPro|Optimization")
	uint32 bManageSignificance : 1;

	/**
	 * MaxDistance per Significance, defaults to 0.0 (lowest) Significance if outside bounds of distances specified here.
	 */
	UPROPERTY(EditAnywhere, Category = "MortarPro|Optimization", meta = (TitleProperty = "MaxDistance"))
	TArray<FMortarProSignificanceDistance> SignificanceDistanceThresholds;

	/**
	 * Stores Current Significance
	 */
	UPROPERTY(BlueprintReadOnly, Category = "MortarPro|Optimization")
	EMortarProSignificanceValue CurrentSignificance;

protected:

	/**
	 * Register Owner Actor Here
	 */
	virtual void BeginPlay() override;

	/**
	 * Unregister Object from Significance Manager
	 */
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/**
	 *  Whether to set siginificance value to lowest when actor is hidden
	 */
	UPROPERTY(EditAnywhere, Category = "Optimization")
	uint32 bInsignificantWhenOwnerIsHidden : 1;

private:

	/**
	 * Function To register Owner to Significance Manager
	 */
	UFUNCTION()
	virtual void RegisterToSignificanceManager();

	/**
	 * Significance Function used for returning Significance Value
	 * @see https://docs.unrealengine.com/5.3/en-US/API/Plugins/SignificanceManager/USignificanceManager/FSignificanceFunction/
	 */
	float SignficanceFunction(USignificanceManager::FManagedObjectInfo* ObjectInfo, const FTransform& Viewpoint);

	/**
	 * Post Signficance Function
	 * @see https://docs.unrealengine.com/5.3/en-US/API/Plugins/SignificanceManager/USignificanceManager/FPostSignificanceFunction/
	 */
	void PostSignficanceFunction(USignificanceManager::FManagedObjectInfo* ObjectInfo, float OldSignificance, float Significance, bool bFinal);

	/**
	 * Gets Significance Value By Distance
	 * @param[in] Distance float variable containing Current Distance
	 * @return Significance Value
	 */
	float GetSignificanceByDistance(const float& Distance) const;

	/**
	* Tag Name of the registered object 
	*/
	FName RegisterObjectTag;

};
