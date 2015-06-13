
struct ComponentModifiers
{
    ComponentModifiers(ChargeType _color = ChargeType::None, int _chargePreference = 2, WireType _speed = WireType::Standard, CircuitBoundaryType _boundary = CircuitBoundaryType::Invalid)
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
        color = ChargeType::None;
        storedChargeColor = GameUtil::chargeColor(ChargeType::Gray);
        boundary = CircuitBoundaryType::Invalid;
        chargePreference = 2;
        speed = WireType::Standard;
        puzzleType = ComponentPuzzleType::User;
    }

    ChargeType color;
    vec4f storedChargeColor;
    CircuitBoundaryType boundary;
    int chargePreference; // value ranges from 0 to 4; default is 2

    //
    // At present, this is only planned for plain wires, but could in theory be supported for any type.
    //
    WireType speed;

    ComponentPuzzleType puzzleType;
};

//
// These are per-component properties, such as the total number of charges on a power source.
// They do not change during the course of a puzzle.
//
struct ComponentIntrinsics
{
    ComponentIntrinsics()
    {
        //
        // default power source values
        //
        totalCharges = 0xFFFFFF;
        secondsPerEmission = 1;
        secondsBeforeFirstEmission = 1;
        
        //
        // default mega hold properties
        //
        ticksPerDischarge = 60;
        chargesLostPerDischarge = 1;
        totalChargeRequired = 10;
    }

    //
    // power source properties
    //
    int totalCharges;
    int secondsPerEmission;
    int secondsBeforeFirstEmission;

    //
    // mega hold properties
    //
    int ticksPerDischarge;
    int chargesLostPerDischarge;
    int totalChargeRequired;
};

struct Connection
{
    Connection()
    {
        component = nullptr;
        blocked = false;
        desired = false;
    }
    Component *component;
    bool blocked;
    bool desired;
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
        randomRotationOffset = 360.0f * (float)rand() / (float)RAND_MAX;

        connections.resize(12);
		
        if (name == "CircuitBoundary")
        {
            modifiers.boundary = CircuitBoundaryType::Open;
        }

        resetPuzzle();
    }
    ~Component();

    void tick(AppData &app);
    void tickGraphics();
    void resetPuzzle();
    void resetPowerSource();
    void resetMegaHold();
    bool willAcceptCharge(GameState &state, const Charge &charge);

    bool inactiveBoundary() const
    {
        return info->name == "CircuitBoundary" && (circuitBoundaryNeighbor == nullptr || circuitBoundaryNeighbor->info->name == "Blocker");
    }

    bool circuitCorner() const
    {
        const int xBoundary = (location.circuitPos.x == 0 || location.circuitPos.x == constants::circuitBoardSize - 2);
        const int yBoundary = (location.circuitPos.y == 0 || location.circuitPos.y == constants::circuitBoardSize - 2);
        return (xBoundary && yBoundary);
    }

    bool hasSpokes() const
    {
        return (info->name != "Blocker" && !inactiveBoundary());
    }

    bool isCircuit() const
    {
        return info->name == "Circuit";
    }

    bool isCircuitContainingMegaHold() const;

    //
    // saving and loading
    //
    ParameterTable toTable(const string &tableName) const;
    static Component* fromTable(const ParameterTable &table);

    //
    // baseInfo denotes the object's starting type, after a puzzle reset ex. TrapOpen
    // info denotes to the object's current type, ex. TrapSprung
    //
    const ComponentInfo *baseInfo;
    const ComponentInfo *info;
    ComponentModifiers modifiers;
    ComponentIntrinsics intrinsics;
    
    //
    // locations always specify the top-left coordinate.
    //
    GameLocation location;
    int lastChargeVisit;

    //
    // when a charge blows up, other charges currently travelling to the component also die.
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
    int megaHoldTotalCharge;
    int ticksBeforeDischarge;

    //
    // some components can emit charges. These are pairs of the charge to emit, and the
    // direction the generated charge came from.
    //
    vector< pair<ChargeType, GameLocation> > chargesToEmit;

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
    int chargesRemaining;

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
    vector<Connection> connections;

	// Gets the actual physical location of the circuit in the board frame.
	rect2f boardFrameLocation() const;
	rect2f circuitFrameLocation() const;

	//
	// type of held charge (or ChargeType::None)
	//
	ChargeType heldCharge;

	//
	// this is needed to deal with splitters
	//
	GameLocation sourceOfLastChargeToAttemptToMoveHere;

	//
	// the location that a teleporter/switch/gear will target
	//
	Component* target;

    //
    // an aesthetic rotation offset used for mega holds
    //
    float randomRotationOffset;
};
