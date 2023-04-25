// Copyright 2023, Dakota Dawe, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DataTypes/SKGLEDataTypes.h"
#include "SKGMLEInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class USKGMLEInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SKGMULTIPLAYERLEVELEDITOR_API ISKGMLEInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "SKGMapEditor | Interface")
	void OnGrabbed();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "SKGMapEditor | Interface")
	void OnRelease();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "SKGMapEditor | Interface")
	void OnDeleted();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "SKGMapEditor | Interface")
	void OnMaterialLoaded(const FSKGMLEItemMaterialData& MapEditorItemMaterial);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "SKGMapEditor | Interface")
	void OnScaleChanged(FVector NewScale);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "SKGMapEditor | Interface")
	void OnPropertyChanged(FSKGMLEProperty Property);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "SKGMapEditor | Interface")
	void OnBoolPropertyChanged(FSKGMLEBoolProperty Property);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "SKGMapEditor | Interface")
	void OnFloatPropertyChanged(FSKGMLEFloatProperty Property);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "SKGMapEditor | Interface")
	void OnIntPropertyChanged(FSKGMLEIntProperty Property);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "SKGMapEditor | Interface")
	void OnTextPropertyChanged(FSKGMLETextProperty Property);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "SKGMapEditor | Interface")
	void OnNamePropertyChanged(FSKGMLENameProperty Property);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "SKGMapEditor | Interface")
	void OnStringPropertyChanged(FSKGMLEStringProperty Property);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "SKGMapEditor | Interface")
	TArray<FSKGMLEBoolProperty> GetAllBoolProperties();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "SKGMapEditor | Interface")
	TArray<FSKGMLEFloatProperty> GetAllFloatProperties();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "SKGMapEditor | Interface")
	TArray<FSKGMLEIntProperty> GetAllIntProperties();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "SKGMapEditor | Interface")
	TArray<FSKGMLETextProperty> GetAllTextProperties();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "SKGMapEditor | Interface")
	TArray<FSKGMLENameProperty> GetAllNameProperties();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "SKGMapEditor | Interface")
	TArray<FSKGMLEStringProperty> GetAllStringProperties();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "SKGMapEditor | Interface")
	void SetCanSelect(bool bCanSelect);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "SKGMapEditor | Interface")
	bool CanSelect();
	virtual bool CanSelect_Implementation() { return true; }
};
