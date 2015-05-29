
struct ComponentModifiers
{
    ComponentModifiers(ChargeType _color = ChargeNone, int _chargePreference = 2, WireSpeedType _speed = WireStandard, CircuitBoundaryType _boundary = CircuitBoundaryInvalid)
    {
        init();
        color = _color;
        chargePreference = _chargePreference;
        speed = _speed;
        boundary = _boundary;
    }
    ComponentModifiers(const ComponentInfo &info)
    {
        init();
        color = info.defaultPrimaryCharge();
    }

    void init()
    {
        color = ChargeNone;
        storedChargeColor = GameUtil::chargeColor(ChargeGray);
        boundary = CircuitBoundaryInvalid;
        chargePreference = 2;
        speed = WireStandard;
        puzzleType = ComponentUser;
    }

    ChargeType color;
    vec4f storedChargeColor;
    CircuitBoundaryType boundary;
    int chargePreference; // value ranges from 0 to 4; default is 2

    //
    // At present, this is only planned for plain wires, but could in theory be supported for any type.
    //
    WireSpeedType speed;

    ComponentPuzzleType puzzleType;
};

struct Component
{
    Component(const string &name, ChargeType _color, const GameLocation &_location)
    {
        info = &database().getComponent(name);
        baseInfo = info;
        modifiers.color = _color;
        location = _location;
        lastChargeVisit = 0;
        deathTrapTimeLeft = 0;
        circuitBoard = nullptr;
        circuitBoundaryNeighbor = nullptr;

		// deal with the twelve connections.  I should really make a connection object, but fuck that.
		connections = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
		connectionBlocked = { false, false, false, false, false, false, false, false, false, false, false, false };
		connectionDesired = { false, false, false, false, false, false, false, false, false, false, false, false };

        //
        // default power source values
        //
        totalCharges = 0xFFFFFF;
        secondsPerEmission = 1;
        secondsBeforeFirstEmission = 1;

        if (name == "CircuitBoundary")
        {
            modifiers.boundary = CircuitBoundaryOpen;
        }

        resetPuzzle();
    }
    ~Component();

    void tick();
    void tickGraphics();
    void resetPuzzle();
    void resetPowerSource();
    bool willAcceptCharge(GameState &state, const Charge &charge);

    bool inactiveBoundary() const
    {
        return (info->name == "CircuitBoundary" && circuitBoundaryNeighbor == nullptr);
    }

    bool hasSpokes() const
    {
        return (info->name != "Blocker" && !inactiveBoundary());
    }

    ParameterTable toTable(const string &tableName) const;

    //
    // baseInfo denotes the object's starting type, after a puzzle reset ex. TrapOpen
    // info denotes to the object's current type, ex. TrapSprung
    //
    const ComponentInfo *baseInfo;
    const ComponentInfo *info;
    ComponentModifiers modifiers;
    
    //
    // locations always specify the top-left coordinate.
    //
    GameLocation location;
    int lastChargeVisit;

    //
    // when a charge blows up, other charges currently travelling to the tower also die.
    //
    int deathTrapTimeLeft;

    //
    // if this is a circuit, it will need its own board.
    //
    Board *circuitBoard;

    //
    // some components store charge
    //
    ChargeType storedCharge;

    //
    // some components can emit charges
    //
    vector<ChargeType> chargesToEmit;

	//
	// used in the ticks to deal with multiple charges approaching the same location
	// willTrigger indicates that the component will be processed at the end of the tick
	//
	int numChargesTargetingThisTick;
	bool willTrigger;

    //
    // power sources emit at a certain rate
    //
    int stepsUntilEmission;
    int totalChargesRemaining;

    //
    // power source properties
    //
    int totalCharges;
    int secondsPerEmission;
    int secondsBeforeFirstEmission;

    //
    // The component on the other end of a circuit boundary connection. This can be derived
    // from the board state, but is accessed often enough that it is convenient
    // to store it here.
    //
    Component *circuitBoundaryNeighbor;

	//
	// A list of things this component is connected to.  Updated every time a component is added or deleted.
	// connections should have 8 elements, corresponding to each of the 12 positions a component can be connected to it.
	// They are [above, above-right, right-above, right, right-below, below-right, below, below-left, left-bellow, left, left-above, above-left]
	//
	// this will override circuitBoundaryNeighbor.
	vector<Component*> connections;
	vector<bool> connectionBlocked;
	vector<bool> connectionDesired;

	// Gets the actual physical location of the circuit in the board frame.
	rect2f boardFrameLocation() const;
	rect2f circuitFrameLocation() const;

	//
	// type of held charge (or ChargeNone)
	//
	ChargeType heldCharge;

	//
	// this is needed to deal with splitters
	//
	GameLocation sourceOfLastChargeToAttemptToMoveHere;
};
