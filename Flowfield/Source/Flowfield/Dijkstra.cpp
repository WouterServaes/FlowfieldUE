// Fill out your copyright notice in the Description page of Project Settings.

#include "Dijkstra.h"

#include "Grid/GridSquare.h"

FDijkstra::FDijkstra(const FIntPoint& GridResolution)
	:GridResolution(GridResolution)
{
}

void FDijkstra::RunAlgorithm(const int GoalGridIdx, TArray<FGridSquare>& PGrid)
{
	DistancesGrid.Empty();

	TArray<IdxToVisit> ToVisit{};

	//max distance + 1, used for unexplored indexes
	const int MaxDistance{ PGrid.Num() + 1 };

	for (int Idx{}; Idx < PGrid.Num(); Idx++)
		if (PGrid[Idx].Type != ESquareType::Obstacle)
			DistancesGrid.Add(MaxDistance);
		else
			DistancesGrid.Add(INT_MAX);//int max used to mark obstacles

	DistancesGrid[GoalGridIdx] = 0;//distance of the goal index is 0 as we start there

	TArray<FIntPoint> Neighbors{ { 1, 0 }, { -1, 0 }, { 0, 1 }, { 0, -1 } };//used to calculate the neighbor of an index, no diagonals (x = column, y = row)
	const int NeighborCount{ Neighbors.Num() };
	//===========================================================================
	//================== Initial start point ====================================
	//===========================================================================
	//setting the initial index around the start node

	for (int Idx{}; Idx < NeighborCount; Idx++)
	{
		const int Column{ PGrid[GoalGridIdx].Column + Neighbors[Idx].X };
		const int Row{ PGrid[GoalGridIdx].Row + Neighbors[Idx].Y };

		if (Column >= GridResolution.X) continue;
		if (Row >= GridResolution.Y) continue;

		const int NeighborIndex{ Column + Row * GridResolution.X };

		if (PGrid[NeighborIndex].Type != ESquareType::Obstacle)
		{
			DistancesGrid[NeighborIndex] = 1;
			ToVisit.Add(IdxToVisit{ NeighborIndex, 1 });
		}
	}

	//===========================================================================
	//================= Dijkstra algorithm ======================================
	//===========================================================================
	//looping over all the to visit indixes

	for (int ToVisitLoopIdx{}; ToVisitLoopIdx < ToVisit.Num(); ToVisitLoopIdx++) //for all indixes to visit
	{
		const int ToVisitIdx{ ToVisit[ToVisitLoopIdx].Idx };
		const int ToVisitColumn{ PGrid[ToVisitIdx].Column };
		const int ToVisitRow{ PGrid[ToVisitIdx].Row };

		for (size_t NeighborLoopIdx{}; NeighborLoopIdx < NeighborCount; NeighborLoopIdx++) //for all neighbors of this index
		{
			const auto& Neighbor{ Neighbors[NeighborLoopIdx] };
			const int NeighborColumnolumn{ ToVisitColumn + Neighbor.X };
			const int NeighborRow{ ToVisitRow + Neighbor.Y };

			//checking if this "neighbor" is on the grid
			if (NeighborColumnolumn >= GridResolution.X || NeighborColumnolumn < 0) continue;
			if (NeighborRow >= GridResolution.Y || NeighborRow < 0) continue;

			//index of this neighbor in the grid
			const int NeighborIndex{ NeighborColumnolumn + NeighborRow * GridResolution.X };

			if (DistancesGrid[NeighborIndex] == MaxDistance) //check if the this index has been explored
			{
				DistancesGrid[NeighborIndex] = ToVisit[ToVisitLoopIdx].Distance + 1; //change the distance to "visit index distance + 1"
				ToVisit.Add(IdxToVisit{ NeighborIndex, ToVisit[ToVisitLoopIdx].Distance + 1 }); //add this neighbor to the visit vector
			}
		}
	}
}

void FDijkstra::MakeFlowfield(const int GoalIdx, TArray<FGridSquare>& PGrid,
	const TArray<FIntPoint>& FlowfieldFlowDirections)
{
	//max distance + 1, used for unexplored indexes
	const int MaxDistance{ PGrid.Num() + 1 };
	for (int Idx{}; Idx < PGrid.Num(); Idx++)
	{
		if (DistancesGrid[Idx] >= MaxDistance)
		{
			if (PGrid[Idx].Type == Path)
			{
				PGrid[Idx].Type = Unreachable;
			}
			continue;
		}

		if (DistancesGrid[Idx] == 0)
		{
			PGrid[Idx].FlowDirections.Add(FIntPoint(0, 0));
			continue;
		}

		const auto Column{ PGrid[Idx].Column };
		const auto Row{ PGrid[Idx].Row };

		int LowestDistance{ MaxDistance };
		int LowestDistanceNeighborIdx{};

		for (int NeighborLoopIdx{}; NeighborLoopIdx < FlowfieldFlowDirections.Num(); NeighborLoopIdx++)
		{
			const int NeighborColumn{ Column + FlowfieldFlowDirections[NeighborLoopIdx].X },
				NeighborRow{ Row + FlowfieldFlowDirections[NeighborLoopIdx].Y };

			if (NeighborColumn >= GridResolution.X || NeighborColumn < 0) continue; //checking if this "neighbor" is on the grid
			if (NeighborRow >= GridResolution.Y || NeighborRow < 0) continue;

			const int NeighborIndex{ NeighborColumn + NeighborRow * GridResolution.X };//index of this neighbor in the grid

			if (DistancesGrid[NeighborIndex] <= LowestDistance)
			{
				if (FlowfieldFlowDirections[NeighborLoopIdx].Y != 0 && FlowfieldFlowDirections[NeighborLoopIdx].X != 0)
					if (GoesDiagonallyThroughWalls(PGrid, FlowfieldFlowDirections, NeighborLoopIdx, NeighborColumn, NeighborRow))
						continue;
				LowestDistanceNeighborIdx = NeighborLoopIdx;
				LowestDistance = DistancesGrid[NeighborIndex];
			}
		}
		PGrid[Idx].FlowDirections.Add(FlowfieldFlowDirections[LowestDistanceNeighborIdx]);
	}
}

bool FDijkstra::GoesDiagonallyThroughWalls(const TArray<FGridSquare>& PGrid, const TArray<FIntPoint>& FlowfieldFlowDirections,
	const int NeighborLoopIdx, const int NeighborColumn, const int NeighborRow) const
{
	//check if the flowfield doesn't go through a wall corner
	//
	//	xxxxx/---------      => "x" are walls, "-" is path and "/" is a flow field path.
	//	----/xxxxxxxxxx		 => instead of doing this, the path should go around the walls

	int NeighborXNeighbor{}, NeighborYNeighbor{};

	//there's probably a better way of doing this hm
	if (FlowfieldFlowDirections[NeighborLoopIdx].Y > 0)
	{
		NeighborYNeighbor = NeighborColumn + size_t((NeighborRow - 1) * GridResolution.X);

		if (FlowfieldFlowDirections[NeighborLoopIdx].X > 0)
		{
			NeighborXNeighbor = (NeighborColumn - 1) + size_t(NeighborRow * GridResolution.X);
		}
		else if (FlowfieldFlowDirections[NeighborLoopIdx].X < 0)
		{
			NeighborXNeighbor = (NeighborColumn + 1) + size_t(NeighborRow * GridResolution.X);
		}
	}
	else if (FlowfieldFlowDirections[NeighborLoopIdx].Y < 0)
	{
		NeighborYNeighbor = NeighborColumn + size_t((NeighborRow + 1) * GridResolution.X);

		if (FlowfieldFlowDirections[NeighborLoopIdx].X > 0)
		{
			NeighborXNeighbor = (NeighborColumn - 1) + size_t(NeighborRow * GridResolution.X);
		}
		else if (FlowfieldFlowDirections[NeighborLoopIdx].X < 0)
		{
			NeighborXNeighbor = (NeighborColumn + 1) + size_t(NeighborRow * GridResolution.X);
		}
	}

	return PGrid[NeighborXNeighbor].Type == ESquareType::Obstacle &&
		PGrid[NeighborYNeighbor].Type == ESquareType::Obstacle;
}