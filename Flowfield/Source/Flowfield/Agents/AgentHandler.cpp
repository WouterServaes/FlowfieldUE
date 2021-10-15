// Fill out your copyright notice in the Description page of Project Settings.

#include "AgentHandler.h"

//#include "AgentAiController.h"
#include "AgentAi.h"
#include "Misc/MapErrors.h"
// Sets default values
AAgentHandler::AAgentHandler()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	if (AgentActor == nullptr)
		UE_LOG(LogTemp, Error, TEXT("\"Agent\" not set!"));
}

// Called when the game starts or when spawned
void AAgentHandler::BeginPlay()
{
	Super::BeginPlay();
}

void AAgentHandler::UpdateAgentSettings()
{
	if (AgentSettings != CurrentAgentSettings)
	{
		CurrentAgentSettings = AgentSettings;
		for (auto&& Agent : Agents)
			Agent->UpdateAgentSettings(CurrentAgentSettings);
	}
}

// Called every frame
void AAgentHandler::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateAgentSettings();
}

void AAgentHandler::SpawnAgent(const FVector SpawnPosition, const int FlowfieldNr = 0)
{
	if (AgentActor == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Spawning failed: \"Agent\" not set!"));
		return;
	}
	
	if(SpawnInactiveAgents)
	{
		RespawnAgent(SpawnPosition, FlowfieldNr);
		return;
	}
	
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	
	const auto SpawnAgent{ GetWorld()->SpawnActor<AActor>(AgentActor, SpawnPosition, FRotator(0.f), SpawnParams) };
	const auto AgentAi{ Cast<UAgentAi>(SpawnAgent->GetComponentByClass(UAgentAi::StaticClass())) };
	Agents.Add(AgentAi);
	AgentAi->UpdateAgentSettings(CurrentAgentSettings);
	AgentAi->SetFlowfieldNr(FlowfieldNr);
	AgentAi->SetGridPtr(Grid);
	AgentAi->SetAgentHandler(this);
	TotalAgentCount++;
}

void AAgentHandler::RespawnAgent(const FVector SpawnPosition, const int FlowfieldNr)
{
	auto* AgentToRespawn{ InactiveAgents[0] };
	InactiveAgents.RemoveAt(0);
	SpawnInactiveAgents = InactiveAgents.Num() > 0;
	
	const auto AgentAi{ Cast<UAgentAi>(AgentToRespawn->GetComponentByClass(UAgentAi::StaticClass())) };
	AgentAi->ResetAgent();
	AgentAi->SetFlowfieldNr(FlowfieldNr);
	AgentAi->SetGridPtr(Grid);
	auto* StaticMeshComp{ Cast< UStaticMeshComponent>(AgentToRespawn->GetComponentByClass(UStaticMeshComponent::StaticClass())) };
	StaticMeshComp->SetWorldLocation(SpawnPosition);
	StaticMeshComp->SetVisibility(true);
	StaticMeshComp->SetSimulatePhysics(true);
	StaticMeshComp->SetCollisionProfileName(FName("BlockAll"), true);
	TotalAgentCount++;
	
}

void AAgentHandler::RemoveAgent(AActor* AgentToRemove)
{
	auto* StaticMeshComp{ Cast< UStaticMeshComponent>(AgentToRemove->GetComponentByClass(UStaticMeshComponent::StaticClass())) };
	StaticMeshComp->SetVisibility(false);
	StaticMeshComp->SetSimulatePhysics(false);
	StaticMeshComp->SetCollisionProfileName(FName("NoCollision"), true);
	InactiveAgents.Add(AgentToRemove);
	SpawnInactiveAgents = true;
}

void AAgentHandler::DeleteAllAgents()
{
	for (auto* Agent : Agents)
		Agent->GetOwner()->Destroy();
	Agents.Empty();
	InactiveAgents.Empty();
}

int AAgentHandler::GetInactiveAgentCount()
{
	return InactiveAgents.Num();
}

int AAgentHandler::GetActiveAgentCount()
{
	return Agents.Num() - InactiveAgents.Num();
}

int AAgentHandler::GetTotalAgentCount()
{
	return TotalAgentCount;
}


