// Copyright 2023, Dakota Dawe, All rights reserved

#pragma once

#include "GameFramework/Actor.h"
#include "Materials/MaterialInterface.h"
#include "Components/MeshComponent.h"
#include "DataTypes/SKGDirectionalLightDataTypes.h"
#include "SKGLEDataTypes.generated.h"

extern const FName SKGMULTIPLAYERLEVELEDITOR_API SKGMLE_HISTORYCHANGED_FUNCTION;
extern const FName SKGMULTIPLAYERLEVELEDITOR_API SKGMLE_HISTORYCHANGED_FUNCTIONMINMAX;
extern const FName SKGMULTIPLAYERLEVELEDITOR_API SKGMLE_CHANGE_FUNCTION;

UENUM(BlueprintType)
enum class ESKGMLEMoveAxis : uint8
{
	None	UMETA(DisplayName = "None"),
	XAxis	UMETA(DisplayName = "XAxis"),
	YAxis	UMETA(DisplayName = "Yxis"),
	ZAxis	UMETA(DisplayName = "ZAxis")
};

UENUM(BlueprintType)
enum class ESKGMLERotationAxis : uint8
{
	None	UMETA(DisplayName = "None"),
	Yaw		UMETA(DisplayName = "Yaw"),
	Roll	UMETA(DisplayName = "Roll"),
	Pitch	UMETA(DisplayName = "Pitch")
};

UENUM(BlueprintType)
enum class ESKGMLEScaleAxis : uint8
{
	None	UMETA(DisplayName = "None"),
	ScaleX	UMETA(DisplayName = "ScaleX"),
	ScaleY	UMETA(DisplayName = "ScaleY"),
	ScaleZ	UMETA(DisplayName = "ScaleZ")
};

USTRUCT(BlueprintType)
struct FSKGMLECopiedActor
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly, Category = "SKGMapEditor")
	TSubclassOf<AActor> ActorClass = nullptr;
	UPROPERTY(BlueprintReadOnly, Category = "SKGMapEditor")
	FTransform RelativeTransform = FTransform();
};

USTRUCT(BlueprintType)
struct FSKGMLECopiedActors
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly, Category = "SKGMapEditor")
	TArray<FSKGMLECopiedActor> CopiedActors = {};
	UPROPERTY(BlueprintReadOnly, Category = "SKGMapEditor")
	FTransform CopiedTransform = FTransform();
};

UENUM(BlueprintType)
enum class ESKGMLEVariableType : uint8
{
	Bool	UMETA(DisplayName = "Bool"),
	Float	UMETA(DisplayName = "Float"),
	Integer	UMETA(DisplayName = "Integer"),
	Name	UMETA(DisplayName = "Name"),
	String	UMETA(DisplayName = "String"),
	Text	UMETA(DisplayName = "Text")
};

USTRUCT(BlueprintType)
struct FSKGMLEProperty
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SKGMapEditor")
	FName PropertyName = NAME_None;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SKGMapEditor")
	ESKGMLEVariableType PropertyType = ESKGMLEVariableType::Integer;
};

USTRUCT(BlueprintType)
struct FSKGMLEBoolProperty : public FSKGMLEProperty
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SKGMapEditor")
	bool CurrentValue = false;
	FSKGMLEBoolProperty(){ PropertyType = ESKGMLEVariableType::Bool; }
};

USTRUCT(BlueprintType)
struct FSKGMLEFloatProperty : public FSKGMLEProperty
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SKGMapEditor")
	double CurrentValue = 0;
	FSKGMLEFloatProperty(){ PropertyType = ESKGMLEVariableType::Float; }
};

USTRUCT(BlueprintType)
struct FSKGMLEIntProperty : public FSKGMLEProperty
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SKGMapEditor")
	int32 CurrentValue = 0;
	FSKGMLEIntProperty(){ PropertyType = ESKGMLEVariableType::Integer; }
};

USTRUCT(BlueprintType)
struct FSKGMLETextProperty : public FSKGMLEProperty
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SKGMapEditor")
	FText CurrentValue = FText();
	FSKGMLETextProperty(){ PropertyType = ESKGMLEVariableType::Text; }
};

USTRUCT(BlueprintType)
struct FSKGMLENameProperty : public FSKGMLEProperty
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SKGMapEditor")
	FName CurrentValue = FName();
	FSKGMLENameProperty(){ PropertyType = ESKGMLEVariableType::Name; }
};

USTRUCT(BlueprintType)
struct FSKGMLEStringProperty : public FSKGMLEProperty
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SKGMapEditor")
	FString CurrentValue = FString();
	FSKGMLEStringProperty(){ PropertyType = ESKGMLEVariableType::String; }
};

USTRUCT(BlueprintType)
struct FSKGMLEMaterial
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SKGMapEditor")
	UMaterialInterface* Material = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SKGMapEditor")
	int32 MaterialIndex = 0;

	bool operator == (const FSKGMLEMaterial& MapEditorMaterial) const
	{
		return Material == MapEditorMaterial.Material && MaterialIndex == MapEditorMaterial.MaterialIndex;
	}
};

USTRUCT(BlueprintType)
struct FSKGMLEMeshComponent
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SKGMapEditor")
	UMeshComponent* MeshComponent = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SKGMapEditor")
	TArray<FSKGMLEMaterial> Materials = TArray<FSKGMLEMaterial>();
};

USTRUCT(BlueprintType)
struct FSKGMLEItemMaterialData
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SKGMapEditor")
	AActor* Actor = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SKGMapEditor")
	TArray<FSKGMLEMeshComponent> MeshComponents = TArray<FSKGMLEMeshComponent>();
};

USTRUCT(BlueprintType)
struct FSKGMLEItemSave
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly, Category = "SKGMapEditor")
	TSubclassOf<AActor> ActorToSpawn;
	UPROPERTY(BlueprintReadOnly, Category = "SKGMapEditor")
	FTransform ItemTransform;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SKGMapEditor")
	TArray<FSKGMLEMeshComponent> MeshComponents = TArray<FSKGMLEMeshComponent>();

	UPROPERTY(BlueprintReadOnly, Category = "SKGMapEditor")
	bool bIsBaseItem = false;
	UPROPERTY(BlueprintReadOnly, Category = "SKGMapEditor")
	TArray<FSKGMLEBoolProperty> BoolProperties;
	UPROPERTY(BlueprintReadOnly, Category = "SKGMapEditor")
	TArray<FSKGMLEFloatProperty> FloatProperties;
	UPROPERTY(BlueprintReadOnly, Category = "SKGMapEditor")
	TArray<FSKGMLEIntProperty> IntProperties;
	UPROPERTY(BlueprintReadOnly, Category = "SKGMapEditor")
	TArray<FSKGMLETextProperty> TextProperties;
	UPROPERTY(BlueprintReadOnly, Category = "SKGMapEditor")
	TArray<FSKGMLENameProperty> NameProperties;
	UPROPERTY(BlueprintReadOnly, Category = "SKGMapEditor")
	TArray<FSKGMLEStringProperty> StringProperties;
};

USTRUCT(BlueprintType)
struct FSKGMLEWorldSettings
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly, Category = "SKGMapEditor")
	FSKGDirectionalLightSettings DirectionalLight;
};

USTRUCT(BlueprintType)
struct FSKGMLESave
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SKGMapEditor")
	FSKGMLEWorldSettings WorldSettings;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SKGMapEditor")
	TArray<FSKGMLEItemSave> Items;
};