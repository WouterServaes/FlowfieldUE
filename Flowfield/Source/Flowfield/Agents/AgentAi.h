// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AgentSettings.h"
#include "AgentAi.generated.h"

class AAgentHandler;
class AGrid;
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class FLOWFIELD_API UAgentAi : public UActorComponent
{
	GENERATED_BODY()

public:
	UAgentAi();
	void UpdateFlowDirection();
	void SetFlowfieldNr(int NewFlowfieldNr);
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void UpdateAgentSettings(const FAgentSettings& NewSettings);
	void SetGridPtr(AGrid* GridPtr);
	bool IsActiveAgent() const { return ActiveAgent; }
	void SetAgentHandler(AAgentHandler* Handler);

	//Resets the agents, like it just spawned
	void ResetAgent();
protected:
	virtual void BeginPlay() override;
private:
	void MoveInDirection() const;
	void DrawAgentDebug() const;
	//Called when agent arrived at its goal
	void AgentArrivedAtGoal();
	//true if agent has arrived at goal
	bool GetArrivedAtGoal() const;
	//timer for removing the agent after it arrived
	void RemoveTimer(float ElapsedSec);
	//Removes the agent from the handler and deactivates it. Stays in mem tho
	void RemoveAgent();
	//true if the agent's position is outside the grid
	bool IsOutsideGrid();

	AGrid* Grid{ nullptr };
	//Static mesh component on Agent actor
	UStaticMeshComponent* StaticMeshComponent{ nullptr };
	AAgentHandler* AgentHandler{ nullptr };

	FVector FlowDirection{};

	bool ArrivedAtGoal{ false };
	bool Move{ true };
	bool ActiveAgent{ true };

	int Flowfield{ 0 };

	UPROPERTY(VisibleAnywhere)
		FAgentSettings Settings {};

	const float RemoveTime{ 1.f };
	float ElapsedRemoveTime{};
};
