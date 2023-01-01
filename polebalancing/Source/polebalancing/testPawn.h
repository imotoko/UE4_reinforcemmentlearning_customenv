// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "testPawn.generated.h"

UCLASS()
class POLEBALANCING_API AtestPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AtestPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//定义轴映射时间
	UFUNCTION()
		void Move_XAxis(float Value);

private:
	//运动方向.X = FMath::clamp(x,Min,Max)
	FVector MovementDirection;
	UPROPERTY(EditAnywhere, Category = "Movement")
		float MovementSpeed = 500.0f;

};
