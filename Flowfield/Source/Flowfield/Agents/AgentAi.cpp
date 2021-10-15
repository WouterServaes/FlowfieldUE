// Fill out your copyright notice in the Description page of Project Settings.

#include "AgentAi.h"
#include "DrawDebugHelpers.h"
#include "Flowfield/Grid/Grid.h"
#include "AgentHandler.h"
// Sets default values for this component's properties
UAgentAi::UAgentAi()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void UAgentAi::UpdateFlowDirection()
{
	FVector NewFlowDirection{  };
	if(Grid->GetFlowDiretionForAgent(Flowfield, StaticMeshComponent->GetComponentLocation(), NewFlowDirection))
		FlowDirection = NewFlowDirection;
}

void UAgentAi::SetFlowfieldNr(const int NewFlowfieldNr)
{
	Flowfield = NewFlowfieldNr;
}

void UAgentAi::MoveInDirection() const
{
	const auto LinVel{ FlowDirection.GetSafeNormal() * Settings.Velocity };
	StaticMeshComponent->SetPhysicsLinearVelocity(LinVel);
}

void UAgentAi::DrawAgentDebug() const
{
	const float ArrowSize{ 25.f };
	DrawDebugDirectionalArrow(GetWorld(), StaticMeshComponent->GetComponentLocation(), StaticMeshComponent->GetComponentLocation() + FlowDirection, ArrowSize, FColor::Blue);
}

void UAgentAi::AgentArrivedAtGoal()
{
	Move = false;
	//stop agent n give it some damping
	StaticMeshComponent->SetPhysicsLinearVelocity(FVector::ZeroVector);
	const float NewLinDamping{ 10.f };
	StaticMeshComponent->SetLinearDamping(NewLinDamping);
	const float NewAngDamping{ 10.f };
	StaticMeshComponent->SetAngularDamping(NewAngDamping);
}

bool UAgentAi::GetArrivedAtGoal() const
{
	return Grid->IsOnGoal(StaticMeshComponent->GetComponentLocation(), Flowfield);
}

void UAgentAi::RemoveTimer(const float ElapsedSec)
{
	ElapsedRemoveTime += ElapsedSec;
	if (ElapsedRemoveTime >= RemoveTime)
		RemoveAgent();
}

void UAgentAi::RemoveAgent()
{
	AgentHandler->RemoveAgent(GetOwner());
	ActiveAgent = false;
	Move = false;
}

bool UAgentAi::IsOutsideGrid()
{
	const auto CurrentLocation{ StaticMeshComponent->GetComponentLocation() };
	const auto InGrid{ Grid->IsInGrid(FVector2D(CurrentLocation)) };
	if (!InGrid)
		RemoveAgent();
	return !InGrid;
}

void UAgentAi::SetAgentHandler(AAgentHandler* Handler)
{
	AgentHandler = Handler;
}

void UAgentAi::ResetAgent()
{
	ActiveAgent = true;
	ElapsedRemoveTime = 0.f;
	ArrivedAtGoal = false;
	Move = true;
}

// Called when the game starts
void UAgentAi::BeginPlay()
{
	Super::BeginPlay();
	// ...
	const auto Comp{ GetOwner()->GetComponentByClass(UStaticMeshComponent::StaticClass()) };
	if (Comp != nullptr)
		StaticMeshComponent = Cast<UStaticMeshComponent>(Comp);
	else
		UE_LOG(LogTemp, Error, TEXT("NO STATIC MESH COMPONENT FOUND ON AGENT"));
}

// Called every frame
void UAgentAi::TickComponent(const float DeltaTime, const ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (ActiveAgent)
	{
		if (!IsOutsideGrid())
		{
			if (Move)
			{
				UpdateFlowDirection();
				MoveInDirection();
			}

			if (!ArrivedAtGoal && GetArrivedAtGoal())
			{
				ArrivedAtGoal = true;
				AgentArrivedAtGoal();
			}

			if (Settings.DrawDebug)
				DrawAgentDebug();

			if (ArrivedAtGoal)
				RemoveTimer(DeltaTime);
		}
	}
}

void UAgentAi::UpdateAgentSettings(const FAgentSettings& NewSettings)
{
	Settings = NewSettings;
}

void UAgentAi::SetGridPtr(AGrid* GridPtr)
{
	Grid = GridPtr;
}