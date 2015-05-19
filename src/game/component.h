
struct Component
{
    Component(const string &name, ChargeType _color, const GameLocation &_location)
    {
        info = &database().getComponent(name);
        baseInfo = info;
        color = _color;
        location = _location;
        lastChargeVisit = 0;
        deathTrapTimeLeft = 0;
        puzzleType = ComponentUser;
        circuitBoard = nullptr;

        resetPowerSource();
        
        chargePreference = 2;
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
    ChargeType color;

    //
    // locations always specify the top-left coordinate.
    //
    GameLocation location;
    int lastChargeVisit;
    int chargePreference; // value ranges from 0 to 4; default is 2
    ComponentPuzzleType puzzleType;

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
