
struct Component
{
    Component(const string &name, ChargeType _charge, const GameLocation &_location)
    {
        info = &database().getComponent(name);
        charge = _charge;
        location = _location;
        lastChargeVisit = 0;
        puzzleType = ComponentUser;
        circuitBoard = nullptr;
        absorbedCharge = ChargeNone;
        timeUntilEmission = 0;
        totalChargesRemaining = 0;
    }
    const ComponentInfo *info;
    ChargeType charge;

    //
    // locations always specify the top-left coordinate.
    //
    GameLocation location;
    int lastChargeVisit;
    int chargePreference;
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
