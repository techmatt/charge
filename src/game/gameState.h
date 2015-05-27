
struct GameState
{
	void init();
    
    void savePuzzle(const string &filename);
    void loadPuzzle(const string &filename);

    void clearBoard();
    void resetPuzzle();
    
    void addNewComponent(Component *component, bool addCircuitComponents = true);
    void removeComponent(Component *component);
    void step();

    void updateCircuitBoundaries();

    // given a circuit boundary component, find its neighbor on the board, if any.
    // this may give a component on either the board or on another circuit.
    Component* findCircuitBoundaryNeighbor(Component &component);

    // given a circuit boundary component, find its neighbor on the aligned circuit board, if any.
    Component* findCircuitBoundaryNeighborAligned(Component &component);

    Component& getCircuit(const GameLocation &pos);

    Component* getComponent(const GameLocation &pos, bool skipInactiveBoundary = true);

    //
    // returns the number of neighbors.
    //
    int findNeighboringComponents(Component &component, Component *neighbors[6]);
 
    Board board;
    vector<Component*> components;
    vector<Charge> charges;
    vector<ExplodingCharge> explodingCharges;

    int stepCount;
    float globalRotationOffset;

    // map from 7x7 circuit coordinate to the offset from the circuit's origin
    // to the neighboring component. Aligned refers to the case where two circuits are perfectly aligned.
    Grid2<vec2i> circuitBoundaryNeighborOffsetTable;
    Grid2<vec2i> circuitBoundaryNeighborOffsetTableAligned;
	// an uglier implementation of the same because I don't understand how to access Matt's
	vec2i BoardToCircuitTargetLocation(vec2i displacement);
	vec2i CircuitToCircuitTargetLocation(vec2i displacement, vec2i circuitPosition);



	Component* connectableComponentAtRelativePosition(Component* component, vec2i relativePosition);
	void updateComponentConnections();

};
