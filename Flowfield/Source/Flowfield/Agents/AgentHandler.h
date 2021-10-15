// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "AgentAi.h"
#include "GameFramework/Actor.h"
#include "AgentSettings.h"
#include "AgentHandler.generated.h"

class UAgentAi;
class AGrid;
UCLASS()
class FLOWFIELD_API AAgentHandler : public AActor
{
	GENERATED_BODY()

public:
	AAgentHandler();
	virtual void Tick(float DeltaTime) override;

	//spawn agent, uses inactive agent if possible
	void SpawnAgent(FVector SpawnPosition, int FlowfieldNr);

	//respawn previously removed agent
	void RespawnAgent(FVector SpawnPosition, int FlowfieldNr);

	//Hide and deactivate agent but keep it in memory
	void RemoveAgent(AActor* AgentToRemove);

	//delete all agent
	void DeleteAllAgents();

	UFUNCTION(BlueprintCallable)
		int GetInactiveAgentCount();
	UFUNCTION(BlueprintCallable)
		int GetActiveAgentCount();
	UFUNCTION(BlueprintCallable)
		int GetTotalAgentCount();
protected:
	virtual void BeginPlay() override;

private:
	void UpdateAgentSettings();
	UPROPERTY(EditAnywhere)
		AGrid* Grid {nullptr};

	//All active agents
	TArray<UAgentAi*> Agents{};

	//The blueprint of the agent actor
	UPROPERTY(EditAnywhere)
		TSubclassOf<class AActor> AgentActor;

	TArray< AActor*> InactiveAgents{};
	FAgentSettings AgentSettings{};
	FAgentSettings CurrentAgentSettings{};

	int TotalAgentCount{ 0 };

	//true if there's inactive agents
	bool SpawnInactiveAgents{ false };
};
