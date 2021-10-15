// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AgentSpawner.generated.h"

UENUM()
enum ESpawnerType
{
	NoSpawn, SpawnOverTime, SpawnImmediateOverWorld
};

class AAgentHandler;
class AGrid;
UCLASS()
class FLOWFIELD_API AAgentSpawner : public AActor
{
	GENERATED_BODY()

public:
	AAgentSpawner();
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintReadWrite)
		TEnumAsByte<ESpawnerType> SpawnerType{ ESpawnerType::NoSpawn };

protected:
	virtual void BeginPlay() override;

private:
	void SpawnAgentsOverTime(float DeltaTime);

	//spawn all agents on the grid at the same time, might cause lag or crash
	void SpawnAllAgentsNowOverWorld();

	UPROPERTY(EditAnywhere, Category = "Settings")
		int AgentsAmountToSpawn{ 4 };

	UPROPERTY(EditAnywhere, Category = "Settings")
		AGrid* Grid { nullptr };

	UPROPERTY(EditAnywhere, Category = "Settings")
		AAgentHandler* AgentHandler { nullptr };

	UPROPERTY(EditAnywhere, Category = "Settings")
		float AgentSpawnZ{ .1f };

	UPROPERTY(EditAnywhere, Category = "Settings")
		float TimeBetweenSpawns{ .2f };

	float ElapsedSpawnTime{};
	bool FinishedSpawning{ false };

	int SpawnCount{};
	bool OutsideGrid{ false };
};
