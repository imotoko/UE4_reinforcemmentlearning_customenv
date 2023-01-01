// Fill out your copyright notice in the Description page of Project Settings.


#include "FBSCharacter.h"

// Sets default values
AFBSCharacter::AFBSCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AFBSCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFBSCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AFBSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	PlayerInputComponent->BindAxis("X", this, &AFBSCharacter::Move_XAxis);
}

void AFBSCharacter::Move_XAxis(float Value)
{
	//ValueÓ³ÉäÕý³£
	if (GEngine)
	{
		FString TheFloatStr = FString::SanitizeFloat(Value);
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, *TheFloatStr);
	}

	FVector Direction = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::X);
	AddMovementInput(Direction, Value);
}