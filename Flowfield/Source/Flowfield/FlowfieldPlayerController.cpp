// Fill out your copyright notice in the Description page of Project Settings.


#include "FlowfieldPlayerController.h"
#include "Grid/Grid.h"
#include "Agents/AgentAi.h"
#include "Engine/Classes/Kismet/GameplayStatics.h"

void AFlowfieldPlayerController::BeginPlay()
{
	SwitchMode();
}

void AFlowfieldPlayerController::LeftClick()
{
	if(IsMouseCursorMode)
	{
		FHitResult HitResult{};
		GetHitResultUnderCursorByChannel(ETraceTypeQuery::TraceTypeQuery1, true, HitResult);
		ProcessClick(HitResult, true);
	}
}

void AFlowfieldPlayerController::RightClick()
{
	if(IsMouseCursorMode)
	{
		FHitResult HitResult{};
		GetHitResultUnderCursorByChannel(ETraceTypeQuery::TraceTypeQuery1, true, HitResult);
		ProcessClick(HitResult, false);
	}
}

void AFlowfieldPlayerController::SwitchMode()
{
	auto PlayerController{ UGameplayStatics::GetPlayerController(GetWorld(), 0) };
	IsMouseCursorMode = !PlayerController->bShowMouseCursor;
	PlayerController->SetShowMouseCursor(IsMouseCursorMode);
	
}

void AFlowfieldPlayerController::ProcessClick(const FHitResult& HitResult,const bool LeftClick)
{
	const auto HitActor{HitResult.GetActor()};
	if (!HitActor) return;
	
	const auto Grid{ Cast<AGrid>(HitActor) };
	if(Grid)
	{
		if(LeftClick)
			Grid->ToggleSquareObstacle(HitResult.ImpactPoint);
		
		return;
	}
	
	const auto Agent{ Cast<UAgentAi>(HitActor->GetComponentByClass(UAgentAi::StaticClass())) };
	if(Agent)
	{
		UE_LOG(LogTemp, Warning, TEXT("CLICKED AGENT"));
		
		return;
	}
}

