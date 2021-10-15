#pragma once

#include "CoreMinimal.h"
#include "SquareType.h"
#include "GridSquare.generated.h"

USTRUCT()
struct FGridSquare
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	int Row{};
	UPROPERTY(VisibleAnywhere)
	int Column{};
	UPROPERTY(VisibleAnywhere)
	FVector BottomLeft{};
	UPROPERTY(VisibleAnywhere)
	TArray<FVector2D> FlowDirections;
	UPROPERTY(VisibleAnywhere)
	TEnumAsByte<ESquareType> Type{ ESquareType::Path };
};