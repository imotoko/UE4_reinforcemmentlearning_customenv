// Fill out your copyright notice in the Description page of Project Settings.


#include "testPawn.h"

// Sets default values
AtestPawn::AtestPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AtestPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AtestPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!MovementDirection.IsZero())
	{
		const FVector NewLocation = GetActorLocation() + (MovementDirection * DeltaTime * MovementSpeed);
		SetActorLocation(NewLocation);
	}

}

// Called to bind functionality to input
void AtestPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	//PlayerInputComponent 来自character，这里采用InputComponent
	InputComponent->BindAxis("X", this, &AtestPawn::Move_XAxis);
	
}

void AtestPawn::Move_XAxis(float Value)
{
	//对于pawn，AddMovementInput不可用
	//FVector Direction = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::X);
	//AddMovementInput(Direction, Value);

	//-1.0f,1.0f
	MovementDirection.X = FMath::Clamp(Value, -1.0f, 1.0f);
}