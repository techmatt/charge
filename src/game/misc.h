
struct BoardLocation
{
    bool inCircuit() const
    {
        return (circuit != constants::invalidCoord);
    }

    //
    // if we are in a circuit, pos is the circuit-relative coordinate
    // otherwise, pos is the board position
    //
    vec2i pos;
    vec2i circuit;
};

enum ComponentPuzzleType
{
    ComponentUser,
    ComponentPuzzlePiece,
    ComponentCopiedCircuit,
};

enum ChargeColorType
{
    ChargeColorRed,
    ChargeColorOrange,
    ChargeColorYellow,
    ChargeColorGreen,
    ChargeColorBlue,

    //
    // Special colors used by certain types of buildings
    //
    ChargeColorNone,
    ChargeColorGray,
};
