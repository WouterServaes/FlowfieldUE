// Fill out your copyright notice in the Description page of Project Settings.

#include "AgentSpawner.h"
#include "../Grid/Grid.h"

#include "AgentHandler.h"
#include "Kismet/KismetMathLibrary.h"
// Sets default values
AAgentSpawner::AAgentSpawner()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AAgentSpawner::BeginPlay()
{
	Super::BeginPlay();
	if (Grid->IsInGrid(FVector2D(GetActorLocation())) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("Spawner is not in grid!"));
		OutsideGrid = true;
	}
}

void AAgentSpawner::SpawnAgentsOverTime(const float DeltaTime)
{
	ElapsedSpawnTime += DeltaTime;
	if (ElapsedSpawnTime >= TimeBetweenSpawns)
	{
		ElapsedSpawnTime = 0.f;

		if (OutsideGrid)
		{
			UE_LOG(LogTemp, Error, TEXT("SpawnAgentsOverTime() Spawner is not in grid!"));
			return;
		}

		const auto FlowfieldCount{ Grid->GetFlowfieldCount() };
		int FlowfieldIdx{ 0 };
		if (FlowfieldCount > 1)
			FlowfieldIdx = UKismetMathLibrary::RandomIntegerInRange(0, FlowfieldCount - 1);

		FVector SpawnLocation{ AActor::GetActorLocation() };
		SpawnLocation.Z = AgentSpawnZ;

		AgentHandler->SpawnAgent(SpawnLocation, FlowfieldIdx);
		SpawnCount++;
		if (SpawnCount >= AgentsAmountToSpawn)
			FinishedSpawning = true;
	}
}

void AAgentSpawner::SpawnAllAgentsNowOverWorld()
{
	const auto FlowfieldCount{ Grid->GetFlowfieldCount() };
	int FlowfieldIdx{ 0 };
	for (int Idx{}; Idx < AgentsAmountToSpawn - SpawnCount; Idx++)
	{
		if (FlowfieldCount > 1)
		{
			FlowfieldIdx = UKismetMathLibrary::RandomIntegerInRange(0, FlowfieldCount - 1);
		}

		const auto RandomPosition{ FVector(Grid->GetRandomPositionOnFlowfield(FlowfieldIdx), AgentSpawnZ) };

		AgentHandler->SpawnAgent(RandomPosition, FlowfieldIdx);
	}

	FinishedSpawning = true;
}

// Called every frame
void AAgentSpawner::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!FinishedSpawning)
	{
		switch (SpawnerType) {
		case NoSpawn: break;
		case SpawnOverTime:
			SpawnAgentsOverTime(DeltaTime);
			break;
		case SpawnImmediateOverWorld:
			SpawnAllAgentsNowOverWorld();
			break;
		default:;
		}
	}
}