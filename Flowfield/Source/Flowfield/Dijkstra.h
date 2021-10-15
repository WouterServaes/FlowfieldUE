// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

struct FGridSquare;
/**
 * 
 */
class FLOWFIELD_API FDijkstra
{
public:
	explicit FDijkstra(const FIntPoint& GridResolution);
	~FDijkstra() = default;

	void RunAlgorithm(int GoalGridIdx, TArray<FGridSquare>& PGrid);
	void MakeFlowfield(int GoalIdx, TArray<FGridSquare>& PGrid, const TArray<FIntPoint>& FlowfieldFlowDirections);
private:

	bool GoesDiagonallyThroughWalls(const TArray<FGridSquare>& PGrid, const TArray<FIntPoint>& FlowfieldFlowDirections
		, int NeighborLoopIdx, int NeighborColumn, int NeighborRow) const;
	const FIntPoint GridResolution;
	struct IdxToVisit
	{
		int Idx{};
		int Distance{};
	};
	TArray<int> DistancesGrid{};
};
