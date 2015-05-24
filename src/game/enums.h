
enum ComponentPuzzleType
{
    ComponentUser,
    ComponentPuzzlePiece,
    ComponentCopiedCircuit,
};

enum WireSpeedType
{
    WireMajorDelay,
    WireMinorDelay,
    WireStandard,
    WireMinorAccelerator,
    WireMajorAccelerator,
};

enum CircuitBoundaryType
{
    CircuitBoundaryInvalid,
    CircuitBoundaryOpen,
    CircuitBoundaryClosed,
};

enum ChargeType
{
    ChargeNone = 0,
    ChargeRed = 1,
    ChargeOrange = 2,
    ChargeYellow = 3,
    ChargeGreen = 4,
    ChargeBlue = 5,
    
    //
    // Gray is a special color that may be used by certain types of components:
    // * MegaHold -> a goal-mega-hold that accepts any type of charge
    //
    
    ChargeGray = 6,
};
