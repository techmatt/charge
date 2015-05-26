
struct ComponentModifiers
{
    ComponentModifiers(ChargeType _color = ChargeNone, ChargeType _storedColor = ChargeNone, int _chargePreference = 2, WireSpeedType _speed = WireStandard, CircuitBoundaryType _boundary = CircuitBoundaryInvalid)
    {
        init();
        color = _color;
        storedColor = _storedColor;
        chargePreference = _chargePreference;
        speed = _speed;
        boundary = _boundary;
    }
    ComponentModifiers(const ComponentInfo &info)
    {
        init();
        color = info.defaultPrimaryCharge();
        storedColor = info.defaultSecondaryCharge();
    }

    void init()
    {
        color = ChargeNone;
        storedColor = ChargeNone;
        boundary = CircuitBoundaryInvalid;
        chargePreference = 2;
        speed = WireStandard;
        puzzleType = ComponentUser;
    }

    ChargeType color;
    ChargeType storedColor;
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
		
        resetPowerSource();
        
        if (name == "CircuitBoundary")
        {
            modifiers.boundary = CircuitBoundaryOpen;
        }
    }
    ~Component();

    void tick();
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
    // When a charge blows up, other charges currently travelling to the tower also die.
    //
    int deathTrapTimeLeft;

    //
    // If this is a circuit, it will need its own board.
    //
    Board *circuitBoard;

    //
    // some components can emit charges
    //
    vector<ChargeType> chargesToEmit;

    //
    // power sources emit at a certain rate
    //
    int timeUntilEmission;
    int totalChargesRemaining;

    //
    // The component on the other end of a circuit boundary connection. This can be derived
    // from the board state, but is accessed often enough that it is convenient
    // to store it here.
    //
    Component *circuitBoundaryNeighbor;
};
