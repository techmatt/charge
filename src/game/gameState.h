
struct GameVictoryInfo
{
    GameVictoryInfo()
    {
        stepCount = -1;
        componentCost = -1;
    }
    bool valid() const
    {
        return (stepCount > 0);
    }
    int stepCount;
    int componentCost;
};

struct GameState
{
	GameState(AppData &_app) : app(_app) { init(); }

	void init();
    
    void savePuzzle(const string &filename);
    void loadPuzzle(const string &filename, const string &puzzleName);
	void rezeroFirstEmission();
	void disableEditing();

    void clearComponents();
    void clearAndResetBoard();
    void resetPuzzle();
    
    void updateComponentConnections();
    void updateCircuitBoundaries();
    void updateAll();

    void addNewComponent(Component *component, bool addCircuitComponents = true, bool updateConnections = true);
    void removeComponent(Component *component, bool updateConnections = true);
    void step(AppData &app);

    void setPuzzlePieceState(GameState &basePuzzle);

    int componentCost() const;

	AppData &app;

    // given a circuit boundary component, find its neighbor on the board, if any.
    // this may give a component on either the board or on another circuit.
    Component* findCircuitBoundaryNeighbor(Component &component);

    // given a circuit boundary component, find its neighbor on the aligned circuit board, if any.
    Component* findCircuitBoundaryNeighborAligned(Component &component);

    Component& getCircuit(const GameLocation &pos);

    Component* getComponent(const GameLocation &pos, bool skipInactiveBoundary = true);

    Component* connectableComponentAtRelativePosition(Component* component, vec2i relativePosition);

    Component* findClosestMatch(Component *start);
 
    // an uglier implementation of the same because I don't understand how to access Matt's
    vec2i BoardToCircuitTargetLocation(vec2i displacement);
    vec2i CircuitToCircuitTargetLocation(vec2i displacement, vec2i circuitPosition);

    Board board;

    //
    // The set of components that can be built. Stored in GameState because it is saved and loaded with each puzzle.
    //
    ComponentBuildState buildableComponents;
    
    vector<Component*> components;
    vector<Charge> charges;
    vector<ExplodingCharge> explodingCharges;

    vector< pair<Charge*, int> > chargePriorityStorage;

    int stepCount;
    float globalRotationOffset;
    bool victory;

    GameVictoryInfo victoryInfo;

    float victoryChargeScaleFactorPositive;
    float victoryChargeScaleFactorNegative;

    //
    // semantic puzzle name. Not saved in the file.
    //
    string puzzleName;

	// BasePuzzle,ProvidedSolution,UserProgress,UserSolution,Custom
	string puzzleFileType;
    
	// Campaign,<anything>,Custom
	string levelPack;
	string levelPackPuzzleName;
	int levelPackPuzzleIndex;

    // map from 7x7 circuit coordinate to the offset from the circuit's origin
    // to the neighboring component. Aligned refers to the case where two circuits are perfectly aligned.
    Grid2<vec2i> circuitBoundaryNeighborOffsetTable;
    Grid2<vec2i> circuitBoundaryNeighborOffsetTableAligned;
};
