
struct Component
{
    ComponentInfo *info;
    ChargeType charge;

    //
    // locations always specify the top-left coordinate.
    //
    BoardLocation location;
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
