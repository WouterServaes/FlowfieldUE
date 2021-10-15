#pragma once

#include "CoreMinimal.h"
#include "AgentSettings.generated.h"


USTRUCT()
struct FAgentSettings
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	float Velocity{50};
	UPROPERTY(VisibleAnywhere)
	float ArriveRadius{1};
	UPROPERTY(VisibleAnywhere)
	bool DrawDebug{ true };

	bool operator!=(const FAgentSettings& OtherSettings) const
	{
		return Velocity != OtherSettings.Velocity
			|| ArriveRadius != OtherSettings.ArriveRadius
			|| DrawDebug != OtherSettings.DrawDebug;
	}

	
};
