// Copyright 2023, Dakota Dawe, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "DataTypes/SKGToolsDataTypes.h"
#include "DataTypes/SKGLEDataTypes.h"
#include "Components/ActorComponent.h"
#include "Actors/SKGMapEditorDirectionalLight.h"
#include "SKGMLEHandlerComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHistoryChanged, ESKGMLEHistoryActionType, ActionType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChange, ESKGMLEActionType, ActionType);

class ASKGMapEditorDirectionalLight;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable )
class SKGMULTIPLAYERLEVELEDITOR_API USKGMLEHandlerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USKGMLEHandlerComponent();

protected:	
	UPROPERTY(EditDefaultsOnly, Category = "SKGMapEditor")
	TEnumAsByte<ECollisionChannel> TraceCollisionChannel;
	UPROPERTY(EditDefaultsOnly, Category = "SKGMapEditor")
	FSKGMLESnappingConfiguration DefaultSnappingConfiguration;
	UPROPERTY(EditDefaultsOnly, Category = "SKGMapEditor")
	float MaxSelectDistance;
	UPROPERTY(EditDefaultsOnly, Category = "SKGMapEditor")
	float ReplicationDelay;
	UPROPERTY(EditDefaultsOnly, Category = "SKGMapEditor")
	bool bPerformMovementModeCheck;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SKGMapEditor")
	FString MapDirectory;

	TWeakObjectPtr<APawn> OwningPawn;
	TWeakObjectPtr<APawn> ReturnPawn;
	TWeakObjectPtr<APlayerController> PlayerController;
	TWeakObjectPtr<ASKGMapEditorDirectionalLight> DirectionalLight;
	FSKGMLESnappingConfiguration CurrentSnappingConfiguration;
	bool bHasBeenInitialized;
	bool bInMovementMode;
	bool bCanSelect;
	float AccumulativeTime;
	bool bAppendPaste;
	bool bIsDrag;
	UPROPERTY()
	FSKGMLECopiedActors CopiedActors;
	UPROPERTY()
	TArray<AActor*> AllActors;
	UPROPERTY()
	TArray<AActor*> AllOwnedActors;
	UPROPERTY()
	TArray<AActor*> AttemptedDeletedActors;
	TArray<FSKGMLEItemTransform> SelectedActorRelativeTransforms;
	
	TWeakObjectPtr<class USKGMLEGizmoWorldSubsystem> GizmoSubystem;
	
	UFUNCTION(Server, Unreliable, WithValidation)
	void Server_UnpossessToReturnPawn();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	FHitResult MouseTraceSingle(const float Distance, const ECollisionChannel CollisionChannel, const bool bDrawDebugLine = false);
	
	UPROPERTY(ReplicatedUsing = OnRep_SpawnedActor)
	TArray<AActor*> SpawnedActors;
	UFUNCTION()
	void OnRep_SpawnedActor();
	UPROPERTY()
	TArray<AActor*> LocalActors;
	
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SetActorTransforms(const TArray<FSKGMLEItemTransform>& ActorTransforms);
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SpawnActor(TSubclassOf<AActor> ActorClass, const FTransform& Transform, bool bPrePlace);
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SpawnActorPaste(FSKGMLECopiedActors CopiedActorsToSpawn, const FTransform& Transform);
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_Delete(const TArray<AActor*>& Actors, bool bDeleteOnlyOwned);
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_ApplyMaterial(const FSKGMLEMeshComponent& MaterialData);
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SetWorldTimeOfDay(float TimeOfDay);
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SetWorldBrightness(float Brightness);
	UFUNCTION(Server, Unreliable, WithValidation)
	void Server_UpdatePrePlacedTransform(const FTransform& Transform);
	AActor* SpawnActorPaste(FSKGMLECopiedActors CopiedActorsToSpawn, const FTransform& Transform);
	
	// Returns true if should continue, false if return
	bool PerformMovementModeCheck() const;

	UFUNCTION()
	void OnHistoryChanged(ESKGMLEHistoryActionType ActionType) const;
	UPROPERTY(BlueprintAssignable, Category = "SKGMapEditor | Events");
	FOnHistoryChanged OnHistoryChangedDelegate;
	UFUNCTION()
	void OnHistoryChangedMinMax(ESKGMLEHistoryActionType ActionType) const { OnHistoryChangedMinMaxDelegate.Broadcast(ActionType); OnChangeDelegate.Broadcast(ESKGMLEActionType::Transform); }
	UPROPERTY(BlueprintAssignable, Category = "SKGMapEditor | Events");
	FOnHistoryChanged OnHistoryChangedMinMaxDelegate;
	UFUNCTION()
	void OnChange(ESKGMLEActionType ActionType) const { OnChangeDelegate.Broadcast(ActionType); }
	UFUNCTION()
	void OnWorldChange() const { OnChangeDelegate.Broadcast(ESKGMLEActionType::WorldSettings); }
	UPROPERTY(BlueprintAssignable, Category = "SKGMapEditor | Events");
	FOnChange OnChangeDelegate;

	void CleanupAllSpawnedActors();
	void Delete(TArray<AActor*> Actors, bool bDeleteOnlyOwned);

	virtual void BeginPlay() override;
	virtual void OnComponentDestroyed(bool bDestroyingHierarchy) override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	UFUNCTION(BlueprintCallable, Category = "SKGMapEditor | Initilization")
	void Init();
	UFUNCTION(BlueprintCallable, Category = "SKGMapEditor | Initilization")
	void SetReturnPawn(APawn* Pawn);
	UFUNCTION(BlueprintCallable, Category = "SKGMapEditor | Initilization")
	void UnpossessToReturnPawn();
	UFUNCTION(BlueprintPure, Category = "SKGMapEditor | Initilization")
	bool HasValidReturnPawn() const { return ReturnPawn != nullptr; }
	
	UFUNCTION(BlueprintCallable, Category = "SKGMapEditor | Spawn")
	AActor* SpawnActor(TSubclassOf<AActor> ActorClass, const FTransform& Transform, bool bPrePlace);
	UFUNCTION(BlueprintCallable, Category = "SKGMapEditor | Spawn")
	AActor* SpawnActorLocalPrePlace(TSubclassOf<AActor> ActorClass, const FTransform& Transform);
	UFUNCTION(BlueprintCallable, Category = "SKGMapEditor | Spawn")
	void TryDuplicateDrag();

	UFUNCTION(BlueprintCallable, Category = "SKGMapEditor | Movement")
	void EnterMovementMode(bool bEnter);
	UFUNCTION(BlueprintPure, Category = "SKGMapEditor | Movement")
	bool IsInMovementMode() const { return bInMovementMode; }
	
	UFUNCTION(BlueprintCallable, Category = "SKGMapEditor | Select")
	FHitResult MouseTrace(float Distance, bool bDrawDebugLine = false);
	UFUNCTION(BlueprintCallable, Category = "SKGMapEditor | Select")
	void FindAndGrab(bool bAppendSelection);
	UFUNCTION(BlueprintCallable, Category = "SKGMapEditor | Select")
	void Grab(AActor* Actor, bool bAppendSelection);
	UFUNCTION(BlueprintCallable, Category = "SKGMapEditor | Select")
	void Release();
	UFUNCTION(BlueprintCallable, Category = "SKGMapEditor | Select")
	void PlacePrePlacedActor();
	UFUNCTION(BlueprintPure, Category = "SKGMapEditor | Select")
	bool IsDraggingLocalActor() const { return LocalActors.Num() > 0; }
	UFUNCTION(BlueprintPure, Category = "SKGMapEditor | Spawn")
	TArray<AActor*> GetAllActors(bool bReCache, bool bShowNonSelectableActors = false);
	// If bClean is true, it will remove all invalid actors from the owned array
	UFUNCTION(BlueprintPure, Category = "SKGMapEditor | Select")
	TArray<AActor*> GetAllOwnedActors(bool bClean = false, bool bShowNonSelectableActors = false);

	UFUNCTION(BlueprintCallable, Category = "SKGMapEditor | Selection")
	void EnableHighlightingOfSelection(bool bEnable);
	UFUNCTION(BlueprintCallable, Category = "SKGMapEditor | Selection")
	void EnableSelecting(bool bEnable) { bCanSelect = bEnable; OnChangeDelegate.Broadcast(ESKGMLEActionType::CanSelectToggle); }
	UFUNCTION(BlueprintCallable, Category = "SKGMapEditor | Selection")
	TArray<AActor*> GetSelection();
	UFUNCTION(BlueprintCallable, Category = "SKGMapEditor | Selection")
	void SetSelectionAtTransform(const FTransform& Transform, bool bMoveSelectionToCenter);
	UFUNCTION(BlueprintCallable, Category = "SKGMapEditor | Selection")
	bool GetSelectionTransform(FTransform& OUTTransform);
	UFUNCTION(BlueprintCallable, Category = "SKGMapEditor | Selection")
	void ApplyMaterial(const FSKGMLEMeshComponent& MaterialData);
	
	UFUNCTION(BlueprintCallable, Category = "SKGMapEditor | Gizmo")
	void SetCoordinateSystem(ESKGMLECoordinate CoordinateSystem);
	UFUNCTION(BlueprintPure, Category = "SKGMapEditor | Gizmo")
	ESKGMLECoordinate GetCoordinateSystem() const;
	UFUNCTION(BlueprintCallable, Category = "SKGMapEditor | Gizmo")
	void SetSnappingConfiguration(const FSKGMLESnappingConfiguration& SnappingConfiguration);
	UFUNCTION(BlueprintCallable, Category = "SKGMapEditor | Gizmo")
	FSKGMLESnappingConfiguration GetSnappingConfiguration() const { return CurrentSnappingConfiguration; }
	UFUNCTION(BlueprintCallable, Category = "SKGMapEditor | Gizmo")
	void SetGizmoType(ESKGMLEGizmoType GizmoType);
	UFUNCTION(BlueprintCallable, Category = "SKGMapEditor | Gizmo")
	ESKGMLEGizmoType GetGizmoType() const;

	UFUNCTION(BlueprintCallable, Category = "SKGMapEditor | Actions")
	void Undo();
	UFUNCTION(BlueprintCallable, Category = "SKGMapEditor | Actions")
	void Redo();
	UFUNCTION(BlueprintCallable, Category = "SKGMapEditor | Actions")
	void DeleteSelection(bool bDeleteOnlyOwned = false);
	UFUNCTION(BlueprintCallable, Category = "SKGMapEditor | Actions")
	void CopySelection();
	UFUNCTION(BlueprintCallable, Category = "SKGMapEditor | Actions")
	void Paste(bool bPasteAtCopiedLocation = true);

	UFUNCTION(BlueprintCallable, Category = "SKGMapEditor | World")
	void SetWorldTimeOfDay(float TimeOfDay);
	UFUNCTION(BlueprintCallable, Category = "SKGMapEditor | World")
	void SetWorldBrightness(float Brightness);
	UFUNCTION(BlueprintPure, Category = "SKGMapEditor | World")
	FSKGDirectionalLightSettings GetDirectionalLightSettings();
	
	bool HasAuthority() const { return GetOwner() ? GetOwner()->HasAuthority() : false; }

	UFUNCTION(BlueprintCallable, Category = "SKGMapEditor | Properties")
	void SetBoolPropertyValue(AActor* Actor, FSKGMLEBoolProperty Property, bool Value);
	UFUNCTION(BlueprintCallable, Category = "SKGMapEditor | Properties")
	void SetFloatPropertyValue(AActor* Actor, FSKGMLEFloatProperty Property, double Value);
	UFUNCTION(BlueprintCallable, Category = "SKGMapEditor | Properties")
	void SetIntPropertyValue(AActor* Actor, FSKGMLEIntProperty Property, int32 Value);
	UFUNCTION(BlueprintCallable, Category = "SKGMapEditor | Properties")
	void SetTextPropertyValue(AActor* Actor, FSKGMLETextProperty Property, FText Value);
	UFUNCTION(BlueprintCallable, Category = "SKGMapEditor | Properties")
	void SetNamePropertyValue(AActor* Actor, FSKGMLENameProperty Property, FName Value);
	UFUNCTION(BlueprintCallable, Category = "SKGMapEditor | Properties")
	void SetStringPropertyValue(AActor* Actor, FSKGMLEStringProperty Property, FString Value);

	UFUNCTION(BlueprintCallable, Category = "SKGMapEditor | Test")
	void ForceGarbageCollection();
};
