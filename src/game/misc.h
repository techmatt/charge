
struct GameLocation
{
    GameLocation() {}
    GameLocation(const vec2i &_boardPos)
    {
        boardPos = _boardPos;
        circuitPos = constants::invalidCoord;
    }
    bool inCircuit() const
    {
        return (circuitPos != constants::invalidCoord);
    }

    //
    // board is the position on the 24 x 24 game board.
    // if we are talking about a circuit location, circuit is the position on the circuit at boardPos
    //
    vec2i boardPos;
    vec2i circuitPos;

    vec2f toScreenCoord(const vec2f &windowDims) const
    {
        return GameUtil::boardToWindow(windowDims, boardPos + vec2i(1, 1));
    }
};

inline bool operator == (const GameLocation &a, const GameLocation &b)
{
    return a.boardPos == b.boardPos && a.circuitPos == b.circuitPos;
}

enum ComponentPuzzleType
{
    ComponentUser,
    ComponentPuzzlePiece,
    ComponentCopiedCircuit,
};
