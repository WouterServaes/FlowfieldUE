// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "FlowfieldPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class FLOWFIELD_API AFlowfieldPlayerController : public APlayerController
{
	GENERATED_BODY()


protected:
	virtual void BeginPlay() override;
private:
	
	UFUNCTION(BlueprintCallable)
	void LeftClick();
	UFUNCTION(BlueprintCallable)
	void RightClick();
	UFUNCTION(BlueprintCallable)
	void SwitchMode();
	bool IsMouseCursorMode{ false };
	void ProcessClick(const FHitResult& HitResult, bool LeftClick);

};
