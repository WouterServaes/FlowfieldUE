# FlowfieldUE
Connect with me on [LinkedIn](https://www.linkedin.com/in/wouterservaes-dae/)! 

View my portfolio [here](https://wouterservaes.myportfolio.com/)!

## Table of contents
- [Info](#info)
- [Overview](#overview)
    - [Spawner](#spawner)
    - [Goal](#goal)
    - [Grid](#grid)
    - [Obstacles](#obstacles)
    - [Agent handler](#agent-handler)
    - [Agents](#agents)
- [Original project](#original)
- [Conclusion](#conclusion)
- [End](#end)
## Info <a name="info"></a>
I decided to make a second project surrounding flowfields, this time using a full engine. When I started this project, Unity seemed like the obvious choice. Development is fast and pretty easy, I have experience with it. In the end, I decided to make it in UE4 using c++. I did this to learn more about the engine and to improve my c++ in UE4. The UI and character controller use blueprints, I exposed some c++ functions to the editor for this reason.
![Gif of project](https://github.com/WouterServaes/FlowfieldUE/blob/main/Images/UEFlowfield_Img02.gif) 
![Images of project](https://github.com/WouterServaes/FlowfieldUE/blob/main/Images/UEFlowfield_Img03.png) 
![Images of project](https://github.com/WouterServaes/FlowfieldUE/blob/main/Images/UEFlowfield_Img01.png) 
## Overview <a name="overview"></a>
### Spawner <a name="spawner"></a>
Agent spawner objects can be placed in the world. You can change some settings, like the amount of agent this spawner spawns, a reference to the grid it uses, a reference to the agent handler it should use, the agent spawn height and the time between each agent spawn. [Code](https://github.com/WouterServaes/FlowfieldUE/blob/main/Flowfield/Source/Flowfield/Agents/AgentSpawner.h)
### Goal <a name="goal"></a>
Goals can be placed in the world too. For every goal, a flowfield is generated. Every agent is assigned a random goal when spawned. Goals are simple objects with no functionality, the grid simply saves the position of each goal at the start.
### Grid <a name="grid"></a>
Grid objects are the playing field of the agents. All the squares of the grid know their column, row, bottom left position, all flow directions and the type of the square. The agents access the grid to know in what direction they have to go. The size of the grid can be set in the settings. A goal actor is given to the grid too, to find all the goals on the grid. This is the same for obstacles. [Code](https://github.com/WouterServaes/FlowfieldUE/blob/main/Flowfield/Source/Flowfield/Grid/Grid.h)
### Obstacles <a name="obstacles"></a>
The obstacles can be whatever actors with a mesh. Before the flowfield is generated, the grid detects what squares should be marked as obstacle by checking if an obstacle mesh is inside the square. [Code](https://github.com/WouterServaes/FlowfieldUE/blob/4256d764f97c81083849767d5b576e93d2f660e3/Flowfield/Source/Flowfield/Grid/Grid.cpp#L279)

### Agent handler <a name="agent-handler"></a>
The agent handler spawns and removes the agents. It only removes the agents once every agent has reached its goal. When an agent reaches its goal, the agents gets deactivated but remains in memory. When a new agents should be spawned, it checks if there are any removed agents and respawns one if this is the case. It spawns a new agent if there are no removed agents. [Code](https://github.com/WouterServaes/FlowfieldUE/blob/4256d764f97c81083849767d5b576e93d2f660e3/Flowfield/Source/Flowfield/Agents/AgentHandler.cpp#L40)

### Agents <a name="agents"></a>
The agent actor is a normal actor with a mesh and an AgentAI component. This component handles the movement of the agent using a reference to the grid. It also does some checks using the grid reference (whether it's outside the grid or if it arrived at a goal). [Code](https://github.com/WouterServaes/FlowfieldUE/blob/main/Flowfield/Source/Flowfield/Agents/AgentAi.cpp)
## Original project <a name="original"></a>
The original project can be found [here](https://github.com/WouterServaes/FlowfieldResearchProject). That was made in a private engine using c++.
## Conclusion <a name="conclusion"></a>
The project performs well when the agents are spawning slowly. It freezes for some time when they all spawn at once on random positions. The project is missing some handy features like spawning, moving and selecting agents, and placing and moving obstacles. 

## End <a name="end"></a>
Connect with me on [LinkedIn](https://www.linkedin.com/in/wouterservaes-dae/)!  
