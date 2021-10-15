

UENUM()
enum ESquareType
{
	Path UMETA(DisplayName="Path"), //agents can be here
	Obstacle UMETA(DisplayName = "Obstacle"), //wall for the agents
	Goal UMETA(DisplayName = "Goal"), //goal for agents
	Unreachable UMETA(DisplayName = "Unreachable") //squares where the flowfield doesn't reach
};