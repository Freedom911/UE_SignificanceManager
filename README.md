# UE_SignificanceManager
Generic Significance Manager class for Unreal Engine for managing tick of actors/components based on distance.

## Steps
1. Include the module **SignificanceManager** in build.cs file in **PublicDependencyModuleNames**.
2. Create SignificanceManager Component in your actor class.
3. Copy MortarProSignificanceComponent.h in Public/Components and MortarProSignificanceComponent.cpp in Private/Components.
4. Bind **FMortarProEndSignificanceChanged** of SignificanceManagerComponent in your actor class.
5. Add Viewport client class (MortarProViewportClient) in your project. (https://forums.unrealengine.com/t/how-do-i-create-a-custom-ugameviewportclient/289909/2)
6. Now whenever significance value is changed based on distance defined in **SignificanceDistanceThresholds** an event will be generated and handled in your actor class. Based on Significance Value user can adjust tick rate.
