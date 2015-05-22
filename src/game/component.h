
struct ComponentModifiers
{
    ComponentModifiers(ChargeType _color = ChargeNone, ChargeType _storedColor = ChargeNone, int _chargePreference = 2, WireSpeedType _speed = WireStandard)
    {
        init();
        color = _color;
        storedColor = _storedColor;
        chargePreference = _chargePreference;
        speed = _speed;
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
};
