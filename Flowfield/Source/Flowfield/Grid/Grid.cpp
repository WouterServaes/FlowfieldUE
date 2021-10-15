// Fill out your copyright notice in the Description page of Project Settings.

#include "Grid.h"
#include "DrawDebugHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "../Dijkstra.h"
// Sets default values
AGrid::AGrid()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//adding root component
	//did this through code for no other reason than "I could"
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	GroundMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GroundMesh"));
	GroundMesh->AttachToComponent(Root, FAttachmentTransformRules::KeepRelativeTransform, TEXT("ground mesh"));
}

FVector2D AGrid::GetGridSize() const
{
	return SquareDimensions * FVector2D(Columns, Rows);
}

FVector2D AGrid::GetRandomPositionOnFlowfield(const int FlowfieldIdx) const
{
	//gives a random position on the field that is not on an Obstacle square
	//with fail-safe so we don't stay in the loop forever when unlucky
	FVector2D RandomLocation{};

	const int MaxTries{ 10 };
	int Tries{};
	do
	{
		Tries++;
		float DistFromEdge{ .1f };
		RandomLocation.X = UKismetMathLibrary::RandomFloatInRange(DistFromEdge, Columns * SquareDimensions.X - DistFromEdge);
		RandomLocation.Y = UKismetMathLibrary::RandomFloatInRange(DistFromEdge, Rows * SquareDimensions.Y - DistFromEdge);
	} while (Grid[GetSquareIndexAtPosition(FVector(RandomLocation, 0.f))].Type == Obstacle || Tries > MaxTries);

	if (Tries > MaxTries)
	{
		FVector2D GoodPosition{};
		for (const auto& GridSquare : Grid)
		{
			if (GridSquare.Type != Obstacle)
				GoodPosition = FVector2D(GridSquare.BottomLeft) + SquareDimensions;
			RandomLocation = GoodPosition;
			UE_LOG(LogTemp, Warning, TEXT("No suitable random position found on flowfield, first suitable square on field used!"));
			break;
		}
	}

	return RandomLocation;
}

FVector2D AGrid::GetSquareDimensions() const
{
	return SquareDimensions;
}

void AGrid::ReloadGroundMesh() const
{
	CreateGroundMesh();
}

FIntPoint AGrid::GetGridDimensions()
{
	return FIntPoint(Columns, Rows);
}

int AGrid::GetFlowfieldCount()
{
	return GoalIndexes.Num();
}

void AGrid::MakeFlowfield()
{
	TArray<int> Goals{};
	for (const auto& Square : Grid)
		if (Square.Type == Goal)
			Goals.Add(TwoDIndexToOneDIndex(Square.Column, Square.Row));

	auto Dijkstra{ FDijkstra(FIntPoint(Columns, Rows)) };

	const TArray<FIntPoint> FlowfieldFlowDirections = {
		{ 1, 0 }, { 1, 1 }, { 0, 1 }, { -1, 1 }, { -1, 0 }, { -1, -1 }, { 0, -1 }, { 1, -1 } }; //every square around a square (also diagonals)

	//for every goal: run algorithm and make flowfield
	for (int Idx{}; Idx < Goals.Num(); Idx++)
	{
		UE_LOG(LogTemp, Warning, TEXT("creating flowfield %d"), Idx);
		Dijkstra.RunAlgorithm(Goals[Idx], Grid);
		Dijkstra.MakeFlowfield(Idx, Grid, FlowfieldFlowDirections);
		UE_LOG(LogTemp, Warning, TEXT("flowfield %d made "), Idx);
	}
}

FVector AGrid::GetMiddleOfSquare(const int SquareIdx) const
{
	return Grid[SquareIdx].BottomLeft + FVector(SquareDimensions / 2.f, 0.f);
}

// Called when the game starts or when spawned
void AGrid::BeginPlay()
{
	Super::BeginPlay();

	CreateGrid();
	SetObstaclesAsSquares();
	SaveGoals();
}

// Called every frame
void AGrid::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (DrawFreeSquares || DrawObstacles)
		DrawGrid();

	if (DrawField)
		DrawFlowfield();
}

bool AGrid::GetFlowDiretionForAgent(const int AgentFlowfieldNr, const FVector& AgentPosition, FVector& FlowDirection) const
{
	const int SquareIdx{ GetSquareIndexAtPosition(AgentPosition) };
	if (Grid[SquareIdx].FlowDirections.Num() == 0) return false;

	FlowDirection = FVector(Grid[SquareIdx].FlowDirections[AgentFlowfieldNr], 0.f);
	return true;
}

bool AGrid::IsOnGoal(FVector Position, const int Flowfield)
{
	const auto SquareIdx{ GetSquareIndexAtPosition(Position) };
	return Grid[SquareIdx].Type == Goal && Grid[SquareIdx].FlowDirections[Flowfield] == FVector2D::ZeroVector;
}

void AGrid::CreateGrid()
{
	Grid.Empty();
	UE_LOG(LogTemp, Warning, TEXT("creating grid"));

	const auto Loc{ GetActorLocation() };

	for (int r{}; r < Rows; r++)
		for (int c{}; c < Columns; c++)
		{
			const FVector BottomLeft{ Loc.X + (SquareDimensions.X * c), Loc.Y + (SquareDimensions.Y * r), Loc.Z };
			FGridSquare GridSquare{ r, c, BottomLeft };

			Grid.Add(GridSquare);
		}

	UE_LOG(LogTemp, Warning, TEXT("created grid"));
	CreateGroundMesh();
}

void AGrid::CreateGroundMesh() const
{
	//rescale to fit grid
	const auto ScaleX{ Columns * (SquareDimensions.X / 100.f) }
	, ScaleY{ Rows * (SquareDimensions.Y / 100.f) };
	GroundMesh->SetRelativeScale3D(FVector(ScaleX, ScaleY, .1f));

	//reposition to middle of grid
	const auto GridWidth{ Columns * SquareDimensions.X }
	, GridLength{ Rows * SquareDimensions.Y };
	const auto ActorLoc{ AActor::GetActorLocation() };
	const auto PosX{ ActorLoc.X + GridWidth / 2.f }
	, PosY{ ActorLoc.Y + GridLength / 2.f };
	GroundMesh->SetRelativeLocation(FVector(PosX, PosY, ActorLoc.Z));
}

void AGrid::DrawGrid() const
{
	FColor GridColor{ FColor::Red };
	const auto World{ AActor::GetWorld() };
	const auto SquareExtent{ FVector(SquareDimensions / 2.f, SquareDimensions.X / 2.f) };
	for (const auto& GridSquare : Grid)
	{
		const auto SquareCenter{ GridSquare.BottomLeft + FVector(SquareDimensions, 0) / 2 };

		switch (GridSquare.Type)
		{
		case ESquareType::Path:
			GridColor = FColor::Red;
			break;
		case ESquareType::Goal:
			GridColor = FColor::Green;
			break;

		case ESquareType::Obstacle:
			GridColor = FColor::Black;
			break;
		default:continue;
		}
		if(GridSquare.Type == Path && DrawFreeSquares
			|| GridSquare.Type == Obstacle && DrawObstacles)
		DrawDebugBox(World, SquareCenter, SquareExtent, GridColor);
	}
}

void AGrid::DrawFlowfield()
{
	if (FieldToDraw > GoalIndexes.Num() - 1 || FieldToDraw < 0)
	{
		if (!ShownDrawFieldError)
		{
			UE_LOG(LogTemp, Warning, TEXT("Cant draw flowfield"));
			ShownDrawFieldError = true;
		}
		return;
	}
	ShownDrawFieldError = false;
	const auto World{ AActor::GetWorld() };
	for (const auto& GridSquare : Grid)
	{
		if (GridSquare.Type != Path) 
			continue;
		auto SquareCenter{ GridSquare.BottomLeft + FVector(SquareDimensions, 0.f) / 2 };
		SquareCenter.Z = ArrowDrawingZ;
		DrawDebugDirectionalArrow(World, SquareCenter, SquareCenter + FVector(GridSquare.FlowDirections[FieldToDraw].GetSafeNormal(), 0.f) * ArrowLength, ArrowSize, ArrowColor);
	}
}

int AGrid::TwoDIndexToOneDIndex(const int Column, const int Row) const
{
	return Columns * Row + Column;
}

int AGrid::GetSquareIndexAtPosition(FVector Position) const
{
	const auto GridSquare = Grid.FindByPredicate([this, Position](const FGridSquare& Sqr)-> bool
		{
			const auto& SqrPos = Sqr.BottomLeft;

			return Position.X >= SqrPos.X && Position.X <= SqrPos.X + SquareDimensions.X
				&& Position.Y >= SqrPos.Y && Position.Y <= SqrPos.Y + SquareDimensions.Y;
		});

	if (GridSquare != nullptr)
		return GridSquare->Row * Columns + GridSquare->Column;

	UE_LOG(LogTemp, Error, TEXT("GetSquareIndexAtPosition -> did not find a square at given position"))
		return -1;
}

bool AGrid::IsInGrid(const FVector2D Position) const
{
	const auto GridLocation{ GetActorLocation() };
	return Position.X > GridLocation.X && Position.X < GridLocation.X + Columns * SquareDimensions.X
		&& Position.Y > GridLocation.Y && Position.Y < GridLocation.Y + Rows * SquareDimensions.Y;
}

void AGrid::SaveGoals()
{
	TArray<AActor*> FoundGoals{};
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), GoalActor, FoundGoals);

	for (const auto* FoundGoal : FoundGoals)
	{
		const auto SquareIdx{ GetSquareIndexAtPosition(FoundGoal->GetActorLocation()) };
		if (SquareIdx > 0)
		{
			Grid[SquareIdx].Type = ESquareType::Goal;
			GoalIndexes.Add(SquareIdx);
		}
	}
}

void AGrid::SetObstaclesAsSquares()
{
	const auto SquareDim{ FVector(SquareDimensions, SquareDimensions.X) };
	const auto GridZ{ GetActorLocation().Z };
	const auto BoxExtends{ SquareDim / 2.f };
	const TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes{};
	const TArray<AActor*> ToIgnore{};
	TArray<AActor*> OutActors{};


	//Go over grid, for every square check if any object of type "ObstacleActors" are in the square.
	// if so, check if the distance from the center of that square to the closest point on the mesh of the Obstacle is small enough for it to be considered an Obstacle
	//(if only a tiny area of a square has an obstacle object on it, don't block the whole square).
	//the maximum distance is the width of a square multiplied with a multiplier
	
	const float ValidObstacleDistanceMultiplier{ .4f }; //idk what to call this
	for (auto& GridSquare : Grid)
	{
		const auto BoxOrigin{ FVector(FVector2D(GridSquare.BottomLeft) + SquareDimensions / 2.f, GridZ) };

		bool AnyOverlaps{ false };
		for(auto& ObstacleActor: ObstacleActors)
		{
			AnyOverlaps = UKismetSystemLibrary::BoxOverlapActors(this, BoxOrigin, BoxExtends, ObjectTypes, ObstacleActor, ToIgnore, OutActors);
			if (AnyOverlaps)
				break;
		}

		if (AnyOverlaps)
		{
			bool ValidObstacleSquare{ true };
			for (const auto& Actor : OutActors)
			{
				auto MeshComponents = Actor->GetComponentsByClass(UMeshComponent::StaticClass());
				for(auto& MeshComp:MeshComponents)
				{
				FVector OutPointOnBody{};
				const auto DistToMesh{ Cast<UStaticMeshComponent>(MeshComp)->GetClosestPointOnCollision(BoxOrigin, OutPointOnBody) };
				ValidObstacleSquare = DistToMesh < SquareDim.X * ValidObstacleDistanceMultiplier;
					
				}
			}

			if (ValidObstacleSquare)
				GridSquare.Type = ESquareType::Obstacle;
		}
	}
}

void AGrid::ToggleSquareObstacle(const FVector Position)
{
	const int Idx{ GetSquareIndexAtPosition(Position) };
	auto& SquareType{ Grid[Idx].Type };

	if (SquareType == ESquareType::Path || SquareType == Unreachable)
		SquareType = ESquareType::Obstacle;
	else if (SquareType == ESquareType::Obstacle)
		SquareType = ESquareType::Path;
	
}
