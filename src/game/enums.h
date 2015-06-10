
enum class ComponentPuzzleType
{
    User,
    PuzzlePiece,
    CopiedCircuit,
};

enum class WireType
{
    MajorDelay,
    MinorDelay,
    Standard,
    MinorAccelerator,
    MajorAccelerator,
};

enum class CircuitBoundaryType
{
    Invalid,
    Open,
    Closed,
};

enum class ChargeType
{
    None = 0,
    Red = 1,
    Orange = 2,
    Yellow = 3,
    Green = 4,
    Blue = 5,
    
    //
    // Gray is a special color that may be used by certain types of components:
    // * MegaHold -> a goal-mega-hold that accepts any type of charge
    //
    
    Gray = 6,
};
