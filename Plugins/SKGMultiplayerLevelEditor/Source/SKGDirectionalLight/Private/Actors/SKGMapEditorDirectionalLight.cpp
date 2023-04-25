// Copyright 2023, Dakota Dawe, All rights reserved


#include "Actors/SKGMapEditorDirectionalLight.h"

#include "Components/ArrowComponent.h"
#include "Components/DirectionalLightComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ASKGMapEditorDirectionalLight::ASKGMapEditorDirectionalLight(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UDirectionalLightComponent>(TEXT("LightComponent0")))
{
#pragma region Defaults
	// Structure to hold one-time initialization
	struct FConstructorStatics
	{
		FName ID_Lighting;
		FText NAME_Lighting;
		FConstructorStatics()
			: ID_Lighting(TEXT("Lighting"))
			, NAME_Lighting(NSLOCTEXT( "SpriteCategory", "Lighting", "Lighting" ))
		{
		}
	};
	static FConstructorStatics ConstructorStatics;

#if !WITH_EDITORONLY_DATA
	UDirectionalLightComponent* DirectionalLightComponent;
#endif
	DirectionalLightComponent = CastChecked<UDirectionalLightComponent>(GetLightComponent());
	DirectionalLightComponent->Mobility = EComponentMobility::Stationary;
	DirectionalLightComponent->SetRelativeRotation(FRotator(-50.0f, 0.0f, 0.0f));
	// Make directional light icons big since they tend to be important
	// This is the root component so its scale affects all other components
	DirectionalLightComponent->SetRelativeScale3D(FVector(2.5f, 2.5f, 2.5f));

	RootComponent = DirectionalLightComponent;
	RootComponent->SetMobility(EComponentMobility::Movable);

#if WITH_EDITORONLY_DATA
	ArrowComponent = CreateEditorOnlyDefaultSubobject<UArrowComponent>(TEXT("ArrowComponent0"));
	if (ArrowComponent)
	{
		ArrowComponent->ArrowColor = FColor(150, 200, 255);

		ArrowComponent->bTreatAsASprite = true;
		ArrowComponent->SpriteInfo.Category = ConstructorStatics.ID_Lighting;
		ArrowComponent->SpriteInfo.DisplayName = ConstructorStatics.NAME_Lighting;
		ArrowComponent->SetupAttachment(DirectionalLightComponent);
		ArrowComponent->bLightAttachment = true;
		ArrowComponent->bIsScreenSizeScaled = true;
	}

	bIsSpatiallyLoaded = false;
#endif // WITH_EDITORONLY_DATA
#pragma endregion Defaults
	
	bReplicates = true;
	bAllowClientToChange = false;
}
#pragma region DefaultFuncs
void ASKGMapEditorDirectionalLight::PostLoad()
{
	Super::PostLoad();

	if (GetLightComponent()->Mobility == EComponentMobility::Static)
	{
		GetLightComponent()->LightFunctionMaterial = NULL;
	}
#if WITH_EDITORONLY_DATA
	if(ArrowComponent != nullptr)
	{
		ArrowComponent->ArrowColor = GetLightColor().ToFColor(true);
	}
#endif
}

#if WITH_EDITOR
void ASKGMapEditorDirectionalLight::LoadedFromAnotherClass(const FName& OldClassName)
{
	Super::LoadedFromAnotherClass(OldClassName);

	if(GetLinkerUEVersion() < VER_UE4_REMOVE_LIGHT_MOBILITY_CLASSES)
	{
		static FName DirectionalLightStatic_NAME(TEXT("DirectionalLightStatic"));
		static FName DirectionalLightMovable_NAME(TEXT("DirectionalLightMovable"));
		static FName DirectionalLightStationary_NAME(TEXT("DirectionalLightStationary"));

		UDirectionalLightComponent* DirLightComp = CastChecked<UDirectionalLightComponent>(GetLightComponent());

		if(OldClassName == DirectionalLightStatic_NAME)
		{
			DirLightComp->Mobility = EComponentMobility::Static;
		}
		else if(OldClassName == DirectionalLightMovable_NAME)
		{
			DirLightComp->Mobility = EComponentMobility::Movable;
			DirLightComp->DynamicShadowDistanceMovableLight = DirLightComp->WholeSceneDynamicShadowRadius_DEPRECATED; 
		}
		else if(OldClassName == DirectionalLightStationary_NAME)
		{
			DirLightComp->Mobility = EComponentMobility::Stationary;

			// copy radius to correct var, if it had been changed
			if(DirLightComp->WholeSceneDynamicShadowRadius_DEPRECATED != 20000.f)
			{
				DirLightComp->DynamicShadowDistanceStationaryLight = DirLightComp->WholeSceneDynamicShadowRadius_DEPRECATED;  // copy radius to correct var
			}
		}
	}
}

void ASKGMapEditorDirectionalLight::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if(ArrowComponent != nullptr)
	{
		ArrowComponent->ArrowColor = GetLightColor().ToFColor(true);
	}
}
#endif // WITH_EDITOR
#pragma endregion DefaultFuncs

void ASKGMapEditorDirectionalLight::BeginPlay()
{
	Super::BeginPlay();
	DirectionalLightSettings.TimeOfDay = (GetActorRotation().Pitch);
	DirectionalLightSettings.Brightness = GetBrightness();
}

void ASKGMapEditorDirectionalLight::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ASKGMapEditorDirectionalLight, DirectionalLightSettings);
}

void ASKGMapEditorDirectionalLight::OnRep_DirectionalLightData()
{
	SetActorRotation(FRotator(DirectionalLightSettings.TimeOfDay, 0.0f, 0.0f));
	SetBrightness(DirectionalLightSettings.Brightness);

	OnWorldSettingsChanged.Broadcast();
}

void ASKGMapEditorDirectionalLight::BindOnWorldSettingsChanged(UObject* Object, const FName& FunctionName)
{
	OnWorldSettingsChanged.AddUFunction(Object, FunctionName);
}

void ASKGMapEditorDirectionalLight::SetWorldTimeOfDay(float Time)
{
	if (bAllowClientToChange || HasAuthority())
	{
		Time = FMath::Clamp(Time, 0.0f, 24.0f);
		Time = Time * (360.0f / 24.0f) + 90.0f;
		DirectionalLightSettings.TimeOfDay = Time;
		OnRep_DirectionalLightData();
	}
}

void ASKGMapEditorDirectionalLight::SetWorldBrightness(float Brightness)
{
	if (bAllowClientToChange || HasAuthority())
	{
		DirectionalLightSettings.Brightness = Brightness;
		OnRep_DirectionalLightData();
	}
}

FSKGDirectionalLightSettings ASKGMapEditorDirectionalLight::GetDirectionalLightSettings() const
{
	FSKGDirectionalLightSettings Settings = DirectionalLightSettings;
	Settings.TimeOfDay = (Settings.TimeOfDay - 90.0f) * (24.0f / 360.0f);
	if (Settings.TimeOfDay < 0.0f)
	{
		Settings.TimeOfDay *= -1.0f;
	}
	return Settings;
}

void ASKGMapEditorDirectionalLight::SetDirectionalLightSettings(const FSKGDirectionalLightSettings& Settings)
{
	SetWorldBrightness(Settings.Brightness);
	SetWorldTimeOfDay(Settings.TimeOfDay);
}
