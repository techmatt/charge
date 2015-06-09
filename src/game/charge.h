
struct Charge
{
    Charge(const GameLocation &originLocation, ChargeType _level);

    //
    // advance the charge towards its destination
    //
    void advance(GameState &state);

    //
    // if the charge has reached its destination, process the component
    //
    void interactWithDestination(GameState &state, AppData &app);

    //
    // if the charge has reached its destination, find a new component or destroy the charge if none can be found
    //
    void updateDestination(GameState &state);

    bool findBestDestination(GameState &state);
    //void setNewDestination(GameState &state, Component &newDestination);
	void setNewDestination(GameState &state, Component &newDestination, bool teleport = false);

    double computePreference(GameState &state, Component &targetComponent);

    float interpolation() const
    {
        if (totalTransitTime == 1) return 1.0f;
        return (float)timeInTransit / (float)totalTransitTime;
    }

    int scanlinePriority() const
    {
        const int boardLine = source.boardPos.x + source.boardPos.y;
        const int circuitLine = source.inCircuit() ? source.circuitPos.x + source.circuitPos.y : 0;
        const int circuitY = source.inCircuit() ? source.circuitPos.y : 0;
        return boardLine * 1000000 + source.boardPos.y * 10000 + circuitLine * 100 + circuitY;
    }

    ChargeType level;
    GameLocation source, destination;

    int timeInTransit, totalTransitTime;
    float randomRotationOffset;

    //
    // when the update kills a charge, we mark it for deletion so the game's tick function knows to remove it.
    //
    bool markedForDeletion;
    bool showDeathAnimation;

	//
	// whether the charge has already been processed this tick
	//
	bool resolvedThisTick;
	bool notMovingBecauseOfDesiredConnection;
	//
	// whether the charge was held last tick
	//
	bool held;

	//
	// will interact with destination
	//
	//bool willInteract;

	//
	// an intermediate step in calculating charge destinations
	//
	Component* intendedDestination;
	int intendedConnectionIndex;
};
