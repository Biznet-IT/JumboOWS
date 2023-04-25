// Copyright 2023, Dakota Dawe, All rights reserved


#include "SKGMLECharacter.h"
#include "Components/SKGMLECharacterMovement.h"

#include "Camera/CameraComponent.h"
#include "Components/SkeletalMeshComponent.h"

// Sets default values
ASKGMLECharacter::ASKGMLECharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<USKGMLECharacterMovement>(CharacterMovementComponentName))
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(RootComponent);
	CameraComponent->bUsePawnControlRotation = true;
	GetMesh()->SetupAttachment(CameraComponent);
}

// Called when the game starts or when spawned
void ASKGMLECharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASKGMLECharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

}

// Called to bind functionality to input
void ASKGMLECharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}