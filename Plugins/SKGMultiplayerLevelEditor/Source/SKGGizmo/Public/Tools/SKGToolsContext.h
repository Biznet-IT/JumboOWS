#pragma once

#include "InteractiveToolsContext.h"
#include "SKGHistoryManager.h"
#include "ToolContextInterfaces.h"
#include "Materials/Material.h"
#include "Subsystems/SKGMLEGizmoWorldSubsystem.h"

class FSKGToolsContextQueriesImpl : public IToolsContextQueriesAPI
{
public:
	FSKGToolsContextQueriesImpl(UInteractiveToolsContext* InContext, UWorld* InWorld)
	{
		ToolsContext = InContext;
		World = InWorld;
	}

	virtual void GetCurrentSelectionState(FToolBuilderState& StateOut) const override
	{
		StateOut.ToolManager = ToolsContext->ToolManager;
		StateOut.TargetManager = ToolsContext->TargetManager;
		StateOut.GizmoManager = ToolsContext->GizmoManager;
		StateOut.World = World;

		USKGMLEGizmoWorldSubsystem* GizmoSubsystem = World->GetSubsystem<USKGMLEGizmoWorldSubsystem>();
		if (GizmoSubsystem)
		{
			const TArray<AActor*> Selection = GizmoSubsystem->GetSelectedActors();
			for (AActor* SO : Selection)
			{
				StateOut.SelectedActors.Add(SO);
				StateOut.SelectedComponents.Add(SO->GetRootComponent());
			}
		}
	}

	virtual void GetCurrentViewState(FViewCameraState& StateOut) const override
	{
		if (ContextActor)
		{
			bool bHasCamera = ContextActor->HasActiveCameraComponent();

			FVector Location;
			FRotator Rotation;
			ContextActor->GetActorEyesViewPoint(Location, Rotation);

			StateOut.Position = Location;
			StateOut.Orientation = Rotation.Quaternion();
			StateOut.HorizontalFOVDegrees = 90;
			StateOut.OrthoWorldCoordinateWidth = 1;
			StateOut.AspectRatio = 1.0;
			StateOut.bIsOrthographic = false;
			StateOut.bIsVR = false;
		}
	}

	virtual EToolContextCoordinateSystem GetCurrentCoordinateSystem() const override
	{
		return CurrentCoordinateSystem;
	}

	virtual UMaterialInterface* GetStandardMaterial(EStandardToolContextMaterials MaterialType) const override
	{
		return UMaterial::GetDefaultMaterial(MD_Surface);
	}

	virtual FToolContextSnappingConfiguration GetCurrentSnappingSettings() const override
	{
		return CurrentSnappingConfiguration;
	}

	virtual UWorld* GetCurrentEditingWorld() const override
	{
		return World;
	}
	
	virtual FViewport* GetHoveredViewport() const override
	{
		return ActiveViewport;
	}

	virtual FViewport* GetFocusedViewport() const override
	{
		return ActiveViewport;
	}
	
	void SetCoordinateSystem(EToolContextCoordinateSystem CoordinateSystem)
	{
		CurrentCoordinateSystem = CoordinateSystem;
	}
	void CycleCoordinateSystem()
	{
		CurrentCoordinateSystem = CurrentCoordinateSystem == EToolContextCoordinateSystem::World ? EToolContextCoordinateSystem::Local : EToolContextCoordinateSystem::World;
	}
	void SetSnappingConfiguration(const FToolContextSnappingConfiguration& SnappingConfiguration)
	{
		CurrentSnappingConfiguration = SnappingConfiguration;
	}

protected:
	FToolContextSnappingConfiguration CurrentSnappingConfiguration;
	EToolContextCoordinateSystem CurrentCoordinateSystem = EToolContextCoordinateSystem::World;
	UInteractiveToolsContext* ToolsContext = nullptr;
	UWorld* World = nullptr;
	AActor* ContextActor = nullptr;
	FViewport* ActiveViewport = nullptr;
};

class FSKGToolsContextTransactionImpl : public IToolsContextTransactionsAPI
{
public:
	FSKGToolsContextTransactionImpl(USKGMLEGizmoWorldSubsystem* INGizmoSubsystem)
	{
		GizmoSubystem = INGizmoSubsystem;
		HistoryManager = GizmoSubystem->GetHistoryManager();
	}
	
	virtual void DisplayMessage(const FText& Message, EToolMessageLevel Level) override
	{
		UE_LOG(LogTemp, Warning, TEXT("[ToolMessage] %s"), *Message.ToString());
	}

	virtual void PostInvalidation() override
	{
	}

	virtual void BeginUndoTransaction(const FText& Description) override
	{
		if (GizmoSubystem->GetHistoryManager())
		{
			GizmoSubystem->GetHistoryManager()->BeginTransaction(Description);
		}
	}

	virtual void EndUndoTransaction() override
	{
		if (GizmoSubystem->GetHistoryManager())
		{
			GizmoSubystem->GetHistoryManager()->EndTransaction();
		}
	}

	virtual void AppendChange(UObject* TargetObject, TUniquePtr<FToolCommandChange> Change, const FText& Description) override
	{
		if (GizmoSubystem->GetHistoryManager())
		{
			bool bCloseTransaction = false;
			if (!bInTransaction)
			{
				BeginUndoTransaction(Description);
				bCloseTransaction = true;
			}

			GizmoSubystem->GetHistoryManager()->AppendChange(TargetObject, MoveTemp(Change), Description);
			if (bCloseTransaction)
			{
				EndUndoTransaction();
			}
		}
	}

	virtual bool RequestSelectionChange(const FSelectedOjectsChangeList& SelectionChange) override
	{
		return false;
	}

protected:
	USKGMLEGizmoWorldSubsystem* GizmoSubystem;
	USKGHistoryManager* HistoryManager;

	bool bInTransaction = false;
};