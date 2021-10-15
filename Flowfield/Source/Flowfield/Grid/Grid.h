// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GridSquare.h"
#include "Grid.generated.h"

class AObstacleSpawner;
UCLASS()
class FLOWFIELD_API AGrid : public AActor
{
	GENERATED_BODY()

public:

	AGrid();
	virtual void Tick(float DeltaTime) override;
	bool GetFlowDiretionForAgent(const int AgentFlowfieldNr, const FVector& AgentPosition, FVector& FlowDirection) const;

	//true if given position is the goal of the flowfield
	bool IsOnGoal(FVector Position, const int Flowfield);

	UPROPERTY();
	USceneComponent* Root;

	/*Used for an experiment (I spawn the ground mesh through code in instructor)*/
	UPROPERTY(EditAnywhere, Category = "Ground Mesh");
	UStaticMeshComponent* GroundMesh;

	//returns the total grid size
	FVector2D GetGridSize() const;

	//returns a random valid position
	FVector2D GetRandomPositionOnFlowfield(int FlowfieldIdx) const;
	FVector2D GetSquareDimensions() const;

	//called in the Construction Script in the BP Editor of the Grid actor
	UFUNCTION(BlueprintCallable)
		void ReloadGroundMesh() const;
	UFUNCTION(BlueprintCallable)
		FIntPoint GetGridDimensions();
	UFUNCTION(BlueprintCallable)
		int GetFlowfieldCount();
	UFUNCTION(BlueprintCallable)
		void MakeFlowfield();


	FVector GetMiddleOfSquare(int SquareIdx) const;
	int GetSquareIndexAtPosition(FVector Position) const;
	bool IsInGrid(FVector2D Position) const;

	UPROPERTY(BlueprintReadWrite)
		bool DrawFreeSquares = true;
	UPROPERTY(BlueprintReadWrite)
		bool DrawObstacles = true;
	UPROPERTY(BlueprintReadWrite)
		bool DrawField = true;
	UPROPERTY(BlueprintReadWrite)
	int FieldToDraw = 0;

	void SetObstaclesAsSquares();

	//toggles the square between path or unreachable and obstacle
	void ToggleSquareObstacle(FVector Position);
protected:
	virtual void BeginPlay() override;

private:
	void CreateGrid();
	void CreateGroundMesh() const;
	void DrawGrid() const;
	void DrawFlowfield();
	int TwoDIndexToOneDIndex(int Column, int Row) const;
	//Finds all goals in the world and saves their positions on the grid
	void SaveGoals();
	UPROPERTY(EditAnywhere, Category = "Dimensions")
		int Rows{ 10 };
	UPROPERTY(EditAnywhere, Category = "Dimensions")
		int Columns{ 10 };
	UPROPERTY(EditAnywhere, Category = "Dimensions")
		FVector2D SquareDimensions { 100, 100 };

	TArray<FGridSquare> Grid;

	bool ShownDrawFieldError{ false };
	TArray<int> GoalIndexes{};

	/*Used to find the goals in the field*/
	UPROPERTY(EditAnywhere, Category = "goals")
		TSubclassOf<AActor> GoalActor{ nullptr };
	/*Used to find the goals in the field*/
	UPROPERTY(EditAnywhere, Category = "Obstacle")
		TArray<TSubclassOf<AActor>> ObstacleActors{};

	/*Used when drawing a flowfield*/
	UPROPERTY(EditAnywhere, Category = "Flowfield arrows")
	float ArrowLength{ 50.f };
	/*Used when drawing a flowfield*/
	UPROPERTY(EditAnywhere, Category = "Flowfield arrows")
	float ArrowSize{ 25.f };
	/*Used when drawing a flowfield*/
	UPROPERTY(EditAnywhere, Category = "Flowfield arrows")
	FColor ArrowColor{ FColor::Cyan };
	/*Used when drawing a flowfield*/
	UPROPERTY(EditAnywhere, Category = "Flowfield arrows")
	float ArrowDrawingZ{ 1.5f };
};
