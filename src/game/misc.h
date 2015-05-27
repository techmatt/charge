
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

	bool operator== (const GameLocation &target)
	{
		if (inCircuit() != target.inCircuit())
			return false;

		if (boardPos != target.boardPos)
			return false;

		if (inCircuit() && circuitPos != target.circuitPos)
			return false;

		return true;
	}
	bool operator!=(const GameLocation target)
	{
		return !(*this == target);
	}

    //
    // board is the position on the 24 x 24 game board.
    // if we are talking about a circuit location, circuit is the position on the circuit at boardPos
    //
    vec2i boardPos;
    vec2i circuitPos;

    vec2f toScreenCoordMainBoard(const vec2f &canonicalDims) const
    {
        if (inCircuit())
            return GameUtil::miniBoardToWindow(canonicalDims, GameLocation(boardPos, circuitPos + vec2i(1, 1)));
        else
            return GameUtil::boardToWindow(canonicalDims, boardPos + vec2i(1, 1));
    }

    vec2f toScreenCoordCircuitBoard(const vec2f &canonicalDims) const
    {
        return GameUtil::circuitToWindow(canonicalDims, circuitPos + vec2i(1, 1));
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
