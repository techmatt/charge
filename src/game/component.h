
struct ComponentModifiers
{
    ComponentModifiers(ChargeType _color = ChargeNone, ChargeType _secondaryColor = ChargeNone, int _chargePreference = 2, WireSpeedType _speed = WireStandard)
    {
        color = _color;
        secondaryColor = _secondaryColor;
        chargePreference = _chargePreference;
        speed = _speed;
        puzzleType = ComponentUser;
    }
    ComponentModifiers(const ComponentInfo &info)
    {
        color = info.defaultPrimaryCharge();
        secondaryColor = info.defaultSecondaryCharge();
        chargePreference = 2;
        speed = WireStandard;
    }
    ChargeType color;
    ChargeType secondaryColor;
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
        modifiers.speed = WireStandard;
        circuitBoard = nullptr;

        resetPowerSource();
        
        modifiers.chargePreference = 2;
    }

    void tick();
    void resetPuzzle();
    void resetPowerSource();
    bool willAcceptCharge(GameState &state, const Charge &charge);

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
    // power sources emit at a certain rate
    //
    int timeUntilEmission;
    int totalChargesRemaining;
};
