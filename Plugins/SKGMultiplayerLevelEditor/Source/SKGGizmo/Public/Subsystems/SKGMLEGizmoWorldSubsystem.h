// Copyright 2023, Dakota Dawe, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Tools/SKGSnappingManager.h"
#include "ToolContextInterfaces.h"
#include "BaseGizmos/CombinedTransformGizmo.h"
#include "BaseGizmos/GizmoBaseComponent.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "DataTypes/SKGToolsDataTypes.h"
#include "Runtime/Launch/Resources/Version.h"
#include "Subsystems/WorldSubsystem.h"
#include "SKGMLEGizmoWorldSubsystem.generated.h"

class USKGHistoryManager;
class UGizmoBaseComponent;
class FSKGToolsContextQueriesImpl;
class FSKGToolsContextTransactionImpl;
class UCombinedTransformGizmo;
class UTransformProxy;
class UInteractiveToolsContext;

UCLASS()
class SKGGIZMO_API USKGMLEGizmoWorldSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()
public:
	USKGMLEGizmoWorldSubsystem();

protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	virtual bool IsTickable() const override;
	virtual TStatId GetStatId() const override { RETURN_QUICK_DECLARE_CYCLE_STAT(USKGMLEGizmoWorldSubsystem, STATGROUP_Tickables); }
	
	TSharedPtr<FSKGToolsContextQueriesImpl> ContextQueriesAPI;
	TSharedPtr<FSKGToolsContextTransactionImpl> ToolsContextTransactionsAPI;
	UPROPERTY()
	UInteractiveToolsContext* ToolsContext;
	UPROPERTY()
	UTransformProxy* TransformProxy;
	UPROPERTY()
	USKGSnappingManager* SnappingManager;
	UPROPERTY()
	USKGHistoryManager* HistoryManager;
	UPROPERTY()
	UCombinedTransformGizmo* TransformGizmo;

	TWeakObjectPtr<APlayerController> PlayerController;
	TWeakObjectPtr<UWorld> World;
	bool bSelfTick;

	FInputDeviceState CurrentMouseState;
	FViewCameraState CurrentViewCameraState;
	FVector2D PrevMousePosition = FVector2D::ZeroVector;
	bool bPendingMouseStateChange = false;
	bool bInCameraControl = false;
	bool bHoveringGizmo = false;
	bool bAllowHighlightSelected = true;
	bool bGizmoContainsScale = false;
	ESKGMLECoordinate CurrentCoordinateSystem;
	ESKGMLECoordinate PreviousCoordinateSystem;
	ESKGMLEGizmoType CurrentGizmoType;
	FToolContextSnappingConfiguration CurrentSnappingConfiguration;

	UPROPERTY()
	TArray<AActor*> SelectedActors;
	FVector OldMouseOrigin;
	bool bMouseChanged;
	bool bSelectedActorsChanged;
	TArray<FSKGMLEItemTransform> CachedTransforms;

	// Gizmo Related
	void DestroyGizmo();
	void SetupGizmo(ESKGMLEGizmoType GizmoType);
	
	TFunction<void(UPrimitiveComponent*, bool)> UpdateHoverFunction = [this](UPrimitiveComponent* Component, bool bHovering)
	{
		bHoveringGizmo = bHovering;
		if (UGizmoBaseComponent* GizmoComponent = Cast<UGizmoBaseComponent>(Component))
		{
			GizmoComponent->UpdateHoverState(bHovering);
		}
	};
	// Actions related
	void SetupHistoryManager();
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnChange, ESKGMLEActionType)
	FOnChange OnChangeDelegate;
	
	// Selections related
	void HighlightActor(AActor* Actor);
	void HighlightAllSelectedActors();
	void DeHighlightActor(AActor* Actor);
	void DeHighlightAllSelectedActors();
	
public:
	void Init(APlayerController* INPlayerController, UWorld* INWorld, const FSKGMLESnappingConfiguration& SnappingConfiguration, bool bEnableSelfTick);
	void CleanupMemory();
	virtual void Tick(float DeltaSeconds) override;
	void SetMovementMode(bool bMovementMode);
	USKGHistoryManager* GetHistoryManager();

	// Interaction Related
	void ClearSelectedKeepGizmo();
	void Grab(AActor* Actor, bool bAppendSelection);
	void LeftMouseDown();
	void LeftMouseUp();

	// Gizmo Related
	void SetCoordinateSystem(ESKGMLECoordinate NewCoordinateSystem, bool bOverwritePrevious = true);
	ESKGMLECoordinate GetCoordinateSystem() const { return CurrentCoordinateSystem; }
	bool IsHoveringGizmo() const { return bHoveringGizmo; }
	void ReloadGizmo(ESKGMLEGizmoType GizmoType);
	void SetGizmoType(ESKGMLEGizmoType GizmoType);
	ESKGMLEGizmoType GetGizmoType() const { return CurrentGizmoType; }
	
	// Selection related
	void EnableHighlightingOfSelection(bool bEnable);
	void SetSelectionAtTransform(const FTransform& Transform, bool bMoveSelectionToCenter);
	FTransform GetGizmoTransform() const { return IsValid(TransformProxy) ? TransformProxy->GetTransform() : FTransform(); }
	bool GetSelectedActorTransforms(TArray<FSKGMLEItemTransform>& OUTActorTransforms, bool bForceRecache = false);
	bool GetSelectedActorTransformsFromGizmo(TArray<FSKGMLEItemTransform>& OUTActorTransforms);
	const TArray<AActor*>& GetSelectedActors() const { return SelectedActors; }

	// Actions related
	void PerformAction(ESKGMLEHistoryActionType ActionType);
	void BindToOnHistoryChanged(UObject* Object, const FName& FunctionName);
	void BindToOnHistoryChangedMinMax(UObject* Object, const FName& FunctionName);
	void BindOnChange(UObject* Object, const FName& FunctionName);
	
	void UpdateSnappingConfiguration(const FSKGMLESnappingConfiguration& SnappingConfiguration);
};