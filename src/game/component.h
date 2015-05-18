
struct Component
{
    Component(const string &name, ChargeType _color, const GameLocation &_location)
    {
        info = &database().getComponent(name);
        baseInfo = info;
        color = _color;
        location = _location;
        lastChargeVisit = 0;
        puzzleType = ComponentUser;
        circuitBoard = nullptr;
        absorbedCharge = ChargeNone;

        timeUntilEmission = 60;
        totalChargesRemaining = 0xFFFFFF;
        
        chargePreference = 2;
    }

    void discharge()
    {
        absorbedCharge = ChargeNone;
    }
    
    bool willAcceptCharge(GameState &state, const Charge &charge);

    //
    // baseInfo denotes the object's starting type, after a puzzle reset ex. TrapOpen
    // info denotes to the object's current type, ex. TrapSprung
    //
    const ComponentInfo *baseInfo;
    const ComponentInfo *info;
    ChargeType color;

    //
    // locations always specify the top-left coordinate.
    //
    GameLocation location;
    int lastChargeVisit;
    int chargePreference; // value ranges from 0 to 4; default is 2
    ComponentPuzzleType puzzleType;

    //
    // If this is a circuit, it will need its own board.
    //
    Board *circuitBoard;

    //
    // Amplifiers have absorbed a certain amount of charge
    //
    ChargeType absorbedCharge;

    //
    // power sources emit at a certain rate
    //
    int timeUntilEmission;
    int totalChargesRemaining;
};
