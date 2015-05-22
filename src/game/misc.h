
struct GameLocation
{
    GameLocation()
    {
        boardPos = constants::invalidCoord;
        circuitPos = constants::invalidCoord;
    }
    explicit GameLocation(const vec2i &_boardPos)
    {
        boardPos = _boardPos;
        circuitPos = constants::invalidCoord;
    }
    explicit GameLocation(const vec2i &_boardPos, const vec2i &_circuitPos)
    {
        boardPos = _boardPos;
        circuitPos = _circuitPos;
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

    vec2f toScreenCoordMainBoard(const vec2f &windowDims) const
    {
        if (inCircuit())
            return GameUtil::miniBoardToWindow(windowDims, GameLocation(boardPos, circuitPos + vec2i(1, 1)));
        else
            return GameUtil::boardToWindow(windowDims, boardPos + vec2i(1, 1));
    }

    vec2f toScreenCoordCircuitBoard(const vec2f &windowDims) const
    {
        return GameUtil::circuitToWindow(windowDims, circuitPos + vec2i(1, 1));
    }

    bool valid() const
    {
        return boardPos != constants::invalidCoord;
    }
};

inline bool operator == (const GameLocation &a, const GameLocation &b)
{
    return a.boardPos == b.boardPos && a.circuitPos == b.circuitPos;
}
