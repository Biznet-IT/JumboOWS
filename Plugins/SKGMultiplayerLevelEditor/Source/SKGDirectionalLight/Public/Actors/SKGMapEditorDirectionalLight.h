// Copyright 2023, Dakota Dawe, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "DataTypes/SKGDirectionalLightDataTypes.h"
#include "Engine/Light.h"
#include "SKGMapEditorDirectionalLight.generated.h"

class UArrowComponent;
class UDirectionalLightComponent;

DECLARE_MULTICAST_DELEGATE(FOnWorldSetingsChanged);

UCLASS()
class SKGDIRECTIONALLIGHT_API ASKGMapEditorDirectionalLight : public ALight
{
	GENERATED_BODY()

public:
	ASKGMapEditorDirectionalLight(const FObjectInitializer& ObjectInitializer);

#pragma region Defaults
	virtual void PostLoad() override;
#if WITH_EDITOR
	virtual void LoadedFromAnotherClass(const FName& OldClassName) override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual bool CanChangeIsSpatiallyLoadedFlag() const override { return false; }
#endif

#if WITH_EDITORONLY_DATA
private:
	UPROPERTY()
	TObjectPtr<UArrowComponent> ArrowComponent;
	/* EditorOnly reference to the light component to allow it to be displayed in the details panel correctly */
	UPROPERTY(VisibleAnywhere, Category="Light")
	TObjectPtr<UDirectionalLightComponent> DirectionalLightComponent;
#endif
	
public:
#if WITH_EDITORONLY_DATA
	UArrowComponent* GetArrowComponent() const { return ArrowComponent; }
	UDirectionalLightComponent* GetComponent() const { return DirectionalLightComponent; }
#endif
#pragma endregion Defaults
protected:
	UPROPERTY(EditAnywhere, Category = "SKGMapEditor")
	bool bAllowClientToChange;
	UPROPERTY(ReplicatedUsing = OnRep_DirectionalLightData)
	FSKGDirectionalLightSettings DirectionalLightSettings;
	UFUNCTION()
	void OnRep_DirectionalLightData();

	FOnWorldSetingsChanged OnWorldSettingsChanged;
	
	virtual void BeginPlay() override;

public:
	void BindOnWorldSettingsChanged(UObject* Object, const FName& FunctionName);
	bool AllowClientAuthorativeChanges() const { return bAllowClientToChange; }
	void SetWorldTimeOfDay(float Time);
	void SetWorldBrightness(float Brightness);
	FSKGDirectionalLightSettings GetDirectionalLightSettings() const;
	void SetDirectionalLightSettings(const FSKGDirectionalLightSettings& Settings);
};
