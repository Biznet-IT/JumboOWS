// Copyright 2023, Dakota Dawe, All rights reserved


#include "Components/SKGMLECharacterMovement.h"

#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Character.h"

USKGMLECharacterMovement::USKGMLECharacterMovement(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	MaxSpeedMultiplier = 4.0f;
	MinSpeedMultiplier = 0.1f;
	RequestedSpeedMultiplier = 1.0f;
	bCenterMouseOnStopMovementMode = false;
	
	bInMovementMode = false;
	MovementMode = EMovementMode::MOVE_Flying;
	DefaultLandMovementMode = EMovementMode::MOVE_Flying;
	DefaultWaterMovementMode = EMovementMode::MOVE_Flying;
	BrakingFrictionFactor = 80.0f;
	MaxAcceleration = 25000.0f;
}

void USKGMLECharacterMovement::BeginPlay()
{
	Super::BeginPlay();

	Init();
}

void USKGMLECharacterMovement::Init()
{
	EnterMovementMode(bInMovementMode);
	if (PawnOwner)
	{
		if (const APlayerController* PC = PawnOwner->GetController<APlayerController>())
		{
			PC->GetViewportSize(ScreenCenterX, ScreenCenterY);
			ScreenCenterX /= 2;
			ScreenCenterY /= 2;
		}
	}
}

void USKGMLECharacterMovement::EnterMovementMode(bool Enter)
{
	bInMovementMode = Enter;

	if (PawnOwner)
	{
		if (APlayerController* PC = PawnOwner->GetController<APlayerController>())
		{
			PC->SetShowMouseCursor(!bInMovementMode);
			if (bInMovementMode)
			{
				PC->GetMousePosition(MouseStartX, MouseStartY);
			}
			
			if (bCenterMouseOnStopMovementMode)
			{
				if (!bInMovementMode)
				{
					PC->SetMouseLocation(ScreenCenterX, ScreenCenterY);
				}
			}
		}
	}
}

bool USKGMLECharacterMovement::Server_SetSpeedMultiplier_Validate(float SpeedMultiplier)
{
	return true;
}

void USKGMLECharacterMovement::Server_SetSpeedMultiplier_Implementation(float SpeedMultiplier)
{
	RequestedSpeedMultiplier = SpeedMultiplier;
}

void USKGMLECharacterMovement::IncreaseSpeedMultiplier(float IncreaseAmount)
{
	RequestedSpeedMultiplier += IncreaseAmount;
	if (RequestedSpeedMultiplier > MaxSpeedMultiplier)
	{
		RequestedSpeedMultiplier = MaxSpeedMultiplier;
	}

	if (GetOwnerRole() < ROLE_Authority)
	{
		Server_SetSpeedMultiplier(RequestedSpeedMultiplier);
	}
}

void USKGMLECharacterMovement::DecreaseSpeedMultiplier(float DecreaseAmount)
{
	RequestedSpeedMultiplier -= DecreaseAmount;
	if (RequestedSpeedMultiplier < MinSpeedMultiplier)
	{
		RequestedSpeedMultiplier = MinSpeedMultiplier;
	}

	if (GetOwnerRole() < ROLE_Authority)
	{
		Server_SetSpeedMultiplier(RequestedSpeedMultiplier);
	}
}

void USKGMLECharacterMovement::MoveForward(float Value)
{
	if (!bInMovementMode || Value == 0.0f) return;
	const FVector Movement = PawnOwner->GetActorForwardVector() * Value;
	//AddInputVector(Movement);
	PawnOwner->AddMovementInput(Movement);
}

void USKGMLECharacterMovement::MoveRight(float Value)
{
	if (!bInMovementMode || Value == 0.0f) return;
	const FVector Movement = PawnOwner->GetActorRightVector() * Value;
	AddInputVector(Movement);
}

void USKGMLECharacterMovement::MoveUp(float Value)
{
	if (!bInMovementMode || Value == 0.0f) return;
	const FVector Movement = PawnOwner->GetActorUpVector() * Value;
	AddInputVector(Movement);
}

void USKGMLECharacterMovement::LookUp(float Value)
{
	if (!bInMovementMode || !PawnOwner || Value == 0.0f) return;
	PawnOwner->AddControllerPitchInput(Value);
}

void USKGMLECharacterMovement::Turn(float Value)
{
	if (!bInMovementMode || !PawnOwner || Value == 0.0f) return;
	PawnOwner->AddControllerYawInput(Value);
}



FNetworkPredictionData_Client* USKGMLECharacterMovement::GetPredictionData_Client() const
{
	if (!ClientPredictionData)
	{
		USKGMLECharacterMovement* MutableThis = const_cast<USKGMLECharacterMovement*>(this);
		MutableThis->ClientPredictionData = new FNetworkPredictionData_Client_MapEditor(*this);
	}
	return ClientPredictionData;
}

float USKGMLECharacterMovement::GetMaxSpeed() const
{
	return Super::GetMaxSpeed() * RequestedSpeedMultiplier;
}

void USKGMLECharacterMovement::OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation,
	const FVector& OldVelocity)
{
	Super::OnMovementUpdated(DeltaSeconds, OldLocation, OldVelocity);
	if (bInMovementMode && PawnOwner)
	{
		if (APlayerController* PC = PawnOwner->GetController<APlayerController>())
		{
			PC->SetMouseLocation(MouseStartX, MouseStartY);
		}
	}
}

void FSavedMove_MapEditor::Clear()
{
	Super::Clear();
	SavedRequestedSpeedMultiplier = 1.0f;
}

bool FSavedMove_MapEditor::CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* Character, float MaxDelta) const
{
	if (SavedRequestedSpeedMultiplier != ((FSavedMove_MapEditor*)&NewMove)->SavedRequestedSpeedMultiplier)
	{
		return false;
	}
	return FSavedMove_Character::CanCombineWith(NewMove, Character, MaxDelta);
}

void FSavedMove_MapEditor::SetMoveFor(ACharacter* C, float InDeltaTime, FVector const& NewAccel,
	FNetworkPredictionData_Client_Character& ClientData)
{
	FSavedMove_Character::SetMoveFor(C, InDeltaTime, NewAccel, ClientData);
	if (const USKGMLECharacterMovement* CustomMovementComponent = Cast<USKGMLECharacterMovement>(C->GetCharacterMovement()))
	{
		SavedRequestedSpeedMultiplier = CustomMovementComponent->RequestedSpeedMultiplier;
	}
}

void FSavedMove_MapEditor::PrepMoveFor(ACharacter* Character)
{
	FSavedMove_Character::PrepMoveFor(Character);

	if (USKGMLECharacterMovement* CustomMovementComponent = Cast<USKGMLECharacterMovement>(Character->GetCharacterMovement()))
	{
		CustomMovementComponent->RequestedSpeedMultiplier = SavedRequestedSpeedMultiplier;
	}
}

FSavedMovePtr FNetworkPredictionData_Client_MapEditor::AllocateNewMove()
{
	return FNetworkPredictionData_Client_Character::AllocateNewMove();
}