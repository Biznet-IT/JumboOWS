// Copyright 2023, Dakota Dawe, All rights reserved


#include "Subsystems/SKGMLEGizmoWorldSubsystem.h"
#include "Tools/SKGToolsContext.h"
#include "Tools/SKGHistoryManager.h"
#include "Tools/SKGToolsFunctionsLibrary.h"
#include "InteractiveToolsContext.h"

#include "DrawDebugHelpers.h"
#include "Slate/SGameLayerManager.h"
#include "Framework/Application/SlateApplication.h"
#include "BaseGizmos/GizmoViewContext.h"
#include "ContextObjectStore.h"
#include "GameFramework/PlayerController.h"
#include "Engine/LocalPlayer.h"

const FName SKGMLE_COMPONENT_NAME = FName("SKGMapEditor");

USKGMLEGizmoWorldSubsystem::USKGMLEGizmoWorldSubsystem()
{
	bSelfTick = false;
	
	CurrentGizmoType = ESKGMLEGizmoType::StandardTranslateRotate;
	CurrentCoordinateSystem = ESKGMLECoordinate::World;
	PreviousCoordinateSystem = CurrentCoordinateSystem;
	bMouseChanged = true;
	bSelectedActorsChanged = true;
	OldMouseOrigin = FVector::ZeroVector;
}

void USKGMLEGizmoWorldSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void USKGMLEGizmoWorldSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

bool USKGMLEGizmoWorldSubsystem::IsTickable() const
{
	return IsInitialized() && bSelfTick;
}

void USKGMLEGizmoWorldSubsystem::Init(APlayerController* INPlayerController, UWorld* INWorld, const FSKGMLESnappingConfiguration& SnappingConfiguration, bool bEnableSelfTick)
{
	PlayerController = INPlayerController;
	World = INWorld;
	bSelfTick = bEnableSelfTick;
	
	if (!IsValid(ToolsContext))
	{
		//GShouldVerifyGCAssumptions = false;
		
		ToolsContext = NewObject<UInteractiveToolsContext>();
		SnappingManager = NewObject<USKGSnappingManager>();
		ToolsContextTransactionsAPI = MakeShared<FSKGToolsContextTransactionImpl>(this);
		ContextQueriesAPI = MakeShared<FSKGToolsContextQueriesImpl>(ToolsContext, World.Get());

		SnappingManager->SetToolsContextQuery(ContextQueriesAPI.Get());
		ToolsContext->Initialize(ContextQueriesAPI.Get(), ToolsContextTransactionsAPI.Get());
		SetupHistoryManager();

		ToolsContext->GizmoManager->GetContextObjectStore()->AddContextObject(SnappingManager);

		UpdateSnappingConfiguration(SnappingConfiguration);

		if (bSelfTick)
		{
			
		}
	}
}

void USKGMLEGizmoWorldSubsystem::CleanupMemory()
{
	ContextQueriesAPI = nullptr;
	ToolsContextTransactionsAPI = nullptr;
	ToolsContext = nullptr;
	TransformProxy = nullptr;
	SnappingManager = nullptr;
	HistoryManager = nullptr;
	TransformGizmo = nullptr;
	SelectedActors.Empty();
}

void USKGMLEGizmoWorldSubsystem::SetupHistoryManager()
{
	if (!IsValid(HistoryManager))
	{
		HistoryManager = NewObject<USKGHistoryManager>(this);
	}
}

void USKGMLEGizmoWorldSubsystem::Tick(float DeltaSeconds)
{
	if (SelectedActors.Num() && PlayerController.IsValid() && World.IsValid() && IsValid(ToolsContext))
	{
		if (UGameViewportClient* ViewportClient = World->GetGameViewport())
		{
			FInputDeviceState InputState = CurrentMouseState;
			InputState.InputDevice = EInputDevices::Mouse;
			FVector2D MousePosition = FSlateApplication::Get().GetCursorPos();
			FModifierKeysState ModifierState = FSlateApplication::Get().GetModifierKeys();
		
			TSharedPtr<IGameLayerManager> LayerManager = ViewportClient->GetGameLayerManager();
			FGeometry ViewportGeometry;
			if (ensure(LayerManager.IsValid()))
			{
				ViewportGeometry = LayerManager->GetViewportWidgetHostGeometry();
			}
			// why do we need this scale here? what is it for?
			FVector2D ViewportMousePos = ViewportGeometry.Scale * ViewportGeometry.AbsoluteToLocal(MousePosition);

			// update modifier keys
			InputState.SetModifierKeyStates(
				ModifierState.IsLeftShiftDown(),
				ModifierState.IsAltDown(),
				ModifierState.IsControlDown(),
				ModifierState.IsCommandDown());

			FEngineShowFlags* ShowFlags = ViewportClient->GetEngineShowFlags();
			FSceneViewFamilyContext ViewFamily(FSceneViewFamily::ConstructionValues(
			ViewportClient->Viewport,
			World->Scene,
			*ShowFlags)
			.SetRealtimeUpdate(true));
			
			ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(PlayerController->Player);
			FVector ViewLocation;
			FRotator ViewRotation;
			FSceneView* SceneView = LocalPlayer->CalcSceneView(&ViewFamily,  /*out*/ ViewLocation, /*out*/ ViewRotation, LocalPlayer->ViewportClient->Viewport);
			if (SceneView == nullptr)
			{ 
				return;
			}
			UGizmoViewContext* GizmoViewContext = ToolsContext->ContextObjectStore->FindContext<UGizmoViewContext>();
			if (GizmoViewContext)
			{
				GizmoViewContext->ResetFromSceneView(*SceneView);
			}
			
			FVector MouseOrigin, MouseDirection;
			PlayerController->DeprojectMousePositionToWorld(MouseOrigin, MouseDirection);
			// Move start position infront of ourselves by 10cm
			MouseOrigin = MouseOrigin + MouseDirection * 10.0f;
			
			InputState.Mouse.Position2D = ViewportMousePos;
			InputState.Mouse.Delta2D = CurrentMouseState.Mouse.Position2D - PrevMousePosition;
			PrevMousePosition = InputState.Mouse.Position2D;
			InputState.Mouse.WorldRay = FRay(MouseOrigin, MouseDirection);

			bool bSentEvent = true;
			bool bIsHoldingGizmo = false;
			if (IsValid(ToolsContext->InputRouter))
			{
				if (bInCameraControl)
				{
					ensure(bPendingMouseStateChange == false);
					ensure(ToolsContext->InputRouter->HasActiveMouseCapture() == false);
					bSentEvent = false;
				}
				else if (bPendingMouseStateChange || ToolsContext->InputRouter->HasActiveMouseCapture())
				{
					bIsHoldingGizmo = ToolsContext->InputRouter->PostInputEvent(InputState);
				}
				else
				{
					ToolsContext->InputRouter->PostHoverInputEvent(InputState);
				}
			}
			if (bIsHoldingGizmo)
			{
				if (OldMouseOrigin.Equals(MouseOrigin))
				{
					bMouseChanged = false;
				}
				else
				{
					bMouseChanged = true;
					OldMouseOrigin = MouseOrigin;
				}
			}
			else
			{
				bMouseChanged = false;
			}
			
			if (bSentEvent && bPendingMouseStateChange)
			{
				bPendingMouseStateChange = false;
				if (CurrentMouseState.Mouse.Left.bDown)
				{
					CurrentMouseState.Mouse.Left.SetStates(false, true, false);
				}
				else
				{
					CurrentMouseState.Mouse.Left.SetStates(false, false, false);
				}
			}
			

			if (ToolsContext->ToolManager)
			{
				ToolsContext->ToolManager->Tick(DeltaSeconds);
			}
			if (ToolsContext->GizmoManager)
			{
				ToolsContext->GizmoManager->Tick(DeltaSeconds);
			}
		}
	}
}

void USKGMLEGizmoWorldSubsystem::SetMovementMode(bool bMovementMode)
{
	bInCameraControl = bMovementMode;
	OnChangeDelegate.Broadcast(ESKGMLEActionType::MovementMode);
}

USKGHistoryManager* USKGMLEGizmoWorldSubsystem::GetHistoryManager()
{
	if (!IsValid(HistoryManager))
	{
		SetupHistoryManager();
	}
	return HistoryManager;
}

void USKGMLEGizmoWorldSubsystem::ClearSelectedKeepGizmo()
{
	if (IsValid(TransformGizmo) && IsValid(TransformProxy))
	{
		DeHighlightAllSelectedActors();
		const FTransform GizmoTransform = GetGizmoTransform();
		SelectedActors.Empty();
		TransformProxy = nullptr;
		TransformProxy = NewObject<UTransformProxy>();
		TransformGizmo->SetActiveTarget(TransformProxy);
		TransformGizmo->SetNewGizmoTransform(GizmoTransform);
	}
}

void USKGMLEGizmoWorldSubsystem::DestroyGizmo()
{
	if (IsValid(TransformGizmo) && IsValid(ToolsContext) && ToolsContext->GizmoManager)
	{
		ToolsContext->GizmoManager->DestroyAllGizmosByOwner(this);
		TransformProxy = nullptr;
	}
}

void USKGMLEGizmoWorldSubsystem::SetupGizmo(ESKGMLEGizmoType GizmoType)
{
	if (IsValid(ToolsContext))
	{
		const ETransformGizmoSubElements GizmoSubElements = USKGToolsFunctionsLibrary::ConvertGizmoType(GizmoType, bGizmoContainsScale);
		if (bGizmoContainsScale && CurrentCoordinateSystem != ESKGMLECoordinate::Relative)
		{
			PreviousCoordinateSystem = CurrentCoordinateSystem;
			SetCoordinateSystem(ESKGMLECoordinate::Relative, false);
		}
		else
		{
			SetCoordinateSystem(PreviousCoordinateSystem, false);
		}
		
		ToolsContext->ToolManager->ActivateTool(EToolSide::Mouse);
		TransformGizmo = ToolsContext->GizmoManager->CreateCustomTransformGizmo(GizmoSubElements, this, "TestInstanceIdentifier");
		TransformGizmo->SetActiveTarget(TransformProxy);
		TransformGizmo->SetUpdateHoverFunction(UpdateHoverFunction);
		TransformGizmo->bSnapToWorldGrid = true;
		TransformGizmo->bSnapToWorldRotGrid = true;
	}
}

void USKGMLEGizmoWorldSubsystem::ReloadGizmo(ESKGMLEGizmoType GizmoType)
{
	DestroyGizmo();
	TransformProxy = NewObject<UTransformProxy>();
	for (const AActor* SelectedActor : SelectedActors)
	{
		TransformProxy->AddComponent(SelectedActor->GetRootComponent());
	}
	SetupGizmo(GizmoType);
	OnChangeDelegate.Broadcast(ESKGMLEActionType::Refresh);
}

void USKGMLEGizmoWorldSubsystem::Grab(AActor* Actor, bool bAppendSelection)
{
	if (IsValid(Actor) && !SelectedActors.Contains(Actor))
	{
		HighlightActor(Actor);
		DestroyGizmo();
		TransformProxy = NewObject<UTransformProxy>();
		if (bAppendSelection)
		{
			SelectedActors.Add(Actor);
			for (const AActor* SelectedActor : SelectedActors)
			{
				TransformProxy->AddComponent(SelectedActor->GetRootComponent());
			}
		}
		else
		{
			DeHighlightAllSelectedActors();
			SelectedActors.Reset();
			SelectedActors.Add(Actor);
			TransformProxy->AddComponent(Actor->GetRootComponent());
		}
		SetupGizmo(CurrentGizmoType);
		OnChangeDelegate.Broadcast(ESKGMLEActionType::Grab);
	}
	else
	{
		DeHighlightAllSelectedActors();
		SelectedActors.Reset();
		DestroyGizmo();
		OnChangeDelegate.Broadcast(ESKGMLEActionType::ClearSelection);
	}
	bSelectedActorsChanged = true;
}

void USKGMLEGizmoWorldSubsystem::LeftMouseDown()
{
	CurrentMouseState.Mouse.Left.SetStates(true, false, false);
	bPendingMouseStateChange = true;
}

void USKGMLEGizmoWorldSubsystem::LeftMouseUp()
{
	CurrentMouseState.Mouse.Left.SetStates(false, false, true);
	bPendingMouseStateChange = true;
	OnChangeDelegate.Broadcast(ESKGMLEActionType::Release);
}

void USKGMLEGizmoWorldSubsystem::HighlightActor(AActor* Actor)
{
	if (bAllowHighlightSelected && IsValid(Actor))
	{
		TArray<UActorComponent*> ActorMeshComponents = Actor->GetComponentsByTag(UMeshComponent::StaticClass(), SKGMLE_COMPONENT_NAME);
		for (UActorComponent* ActorMeshComponent : ActorMeshComponents)
		{
			if (UMeshComponent* MeshComponent = Cast<UMeshComponent>(ActorMeshComponent))
			{
				if (!MeshComponent->bRenderCustomDepth)
				{
					MeshComponent->SetRenderCustomDepth(true);
				}
			}
		}
	}
}

void USKGMLEGizmoWorldSubsystem::HighlightAllSelectedActors()
{
	for (AActor* SelectedActor : SelectedActors)
	{
		HighlightActor(SelectedActor);
	}
}

void USKGMLEGizmoWorldSubsystem::DeHighlightActor(AActor* Actor)
{
	if (IsValid(Actor))
	{
		TArray<UActorComponent*> ActorMeshComponents = Actor->GetComponentsByTag(UMeshComponent::StaticClass(), SKGMLE_COMPONENT_NAME);
		for (UActorComponent* ActorMeshComponent : ActorMeshComponents)
		{
			if (UMeshComponent* MeshComponent = Cast<UMeshComponent>(ActorMeshComponent))
			{
				if (MeshComponent->bRenderCustomDepth)
				{
					MeshComponent->SetRenderCustomDepth(false);
				}
			}
		}
	}
}

void USKGMLEGizmoWorldSubsystem::DeHighlightAllSelectedActors()
{
	for (AActor* SelectedActor : SelectedActors)
	{
		DeHighlightActor(SelectedActor);
	}
}

void USKGMLEGizmoWorldSubsystem::SetCoordinateSystem(ESKGMLECoordinate NewCoordinateSystem, bool bOverwritePrevious)
{
	if (IsValid(TransformGizmo))
	{
		if (bGizmoContainsScale && NewCoordinateSystem == ESKGMLECoordinate::World)
		{
			return;
		}
		const EToolContextCoordinateSystem CoordinateSystem = NewCoordinateSystem == ESKGMLECoordinate::World ? EToolContextCoordinateSystem::World : EToolContextCoordinateSystem::Local;
		CurrentCoordinateSystem = NewCoordinateSystem;
		if (bOverwritePrevious)
		{
			PreviousCoordinateSystem = CurrentCoordinateSystem;
		}
		ContextQueriesAPI->SetCoordinateSystem(CoordinateSystem);
	}
}

void USKGMLEGizmoWorldSubsystem::SetGizmoType(ESKGMLEGizmoType GizmoType)
{
	CurrentGizmoType = GizmoType;
	if (IsValid(TransformGizmo))
	{
		ReloadGizmo(GizmoType);
	}
}

void USKGMLEGizmoWorldSubsystem::EnableHighlightingOfSelection(bool bEnable)
{
	bAllowHighlightSelected = bEnable;
	if (bAllowHighlightSelected)
	{
		for (AActor* SelectedActor : SelectedActors)
		{
			HighlightActor(SelectedActor);
		}
	}
	else
	{
		for (AActor* SelectedActor : SelectedActors)
		{
			DeHighlightActor(SelectedActor);
		}
	}
}

void USKGMLEGizmoWorldSubsystem::SetSelectionAtTransform(const FTransform& Transform, bool bMoveSelectionToCenter)
{
	if (bMoveSelectionToCenter)
	{
		for (AActor* Actor : SelectedActors)
		{
			if (IsValid(Actor))
			{
				Actor->SetActorTransform(Transform);
			}
		}
		ReloadGizmo(CurrentGizmoType);
	}
	else
	{
		if (IsValid(TransformGizmo))
		{
			TransformGizmo->SetNewGizmoTransform(Transform);
		}
	}
	OnChangeDelegate.Broadcast(ESKGMLEActionType::Transform);
}

bool USKGMLEGizmoWorldSubsystem::GetSelectedActorTransforms(TArray<FSKGMLEItemTransform>& OUTActorTransforms, bool bForceRecache)
{
	if (bSelectedActorsChanged || bForceRecache)
	{
		CachedTransforms.Empty();
		CachedTransforms.Reserve(SelectedActors.Num());

		for (AActor* Actor : SelectedActors)
		{
			if (IsValid(Actor))
			{
				CachedTransforms.Add(FSKGMLEItemTransform(Actor));
			}
		}
		CachedTransforms.Shrink();
		bSelectedActorsChanged = false;
	}
	else
	{
		for (FSKGMLEItemTransform& ActorTransform : CachedTransforms)
		{
			ActorTransform.Update();
		}
	}

	OUTActorTransforms = CachedTransforms;
	if (SelectedActors.Num())
	{
		return bMouseChanged;
	}
	return false;
}

bool USKGMLEGizmoWorldSubsystem::GetSelectedActorTransformsFromGizmo(TArray<FSKGMLEItemTransform>& OUTActorTransforms)
{
	if (IsValid(TransformGizmo))
	{
		TArray<FSKGMLEItemTransform> Transforms;
		Transforms.Reserve(SelectedActors.Num());

		for (AActor* Actor : SelectedActors)
		{
			if (IsValid(Actor))
			{
				Transforms.Add(FSKGMLEItemTransform(Actor, TransformGizmo->GetGizmoTransform().GetRelativeTransform(Actor->GetActorTransform())));
			}
		}

		OUTActorTransforms = Transforms;
		return true;
	}
	return false;
}

void USKGMLEGizmoWorldSubsystem::PerformAction(ESKGMLEHistoryActionType ActionType)
{
	if (GetHistoryManager())
	{
		switch (ActionType)
		{
		case ESKGMLEHistoryActionType::Undo : HistoryManager->Undo(); break;
		case ESKGMLEHistoryActionType::Redo : HistoryManager->Redo(); break;
		}
	}
}

void USKGMLEGizmoWorldSubsystem::BindToOnHistoryChanged(UObject* Object, const FName& FunctionName)
{
	if (GetHistoryManager())
	{
		HistoryManager->OnHistoryChangedDelegate.AddUFunction(Object, FunctionName);
	}
}

void USKGMLEGizmoWorldSubsystem::BindToOnHistoryChangedMinMax(UObject* Object, const FName& FunctionName)
{
	if (GetHistoryManager())
	{
		HistoryManager->OnHistoryChangedMinMaxDelegate.AddUFunction(Object, FunctionName);
	}
}

void USKGMLEGizmoWorldSubsystem::BindOnChange(UObject* Object, const FName& FunctionName)
{
	OnChangeDelegate.AddUFunction(Object, FunctionName);
}

void USKGMLEGizmoWorldSubsystem::UpdateSnappingConfiguration(const FSKGMLESnappingConfiguration& SnappingConfiguration)
{
	CurrentSnappingConfiguration = USKGToolsFunctionsLibrary::ConvertSnappingConfiguration(SnappingConfiguration);
	ContextQueriesAPI->SetSnappingConfiguration(CurrentSnappingConfiguration);
}
