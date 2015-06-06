
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

	rect2f boardFrameLocation() const {
		if (!(this->inCircuit())) return rect2f(vec2f(this->boardPos), this->boardPos + vec2f(2.0f, 2.0f));
		CoordinateFrame frame = CoordinateFrame(this->circuitPos, this->circuitPos + vec2f(2.0f, 2.0f), vec2i(constants::circuitBoardSize, constants::circuitBoardSize));
		return frame.toContainer(rect2f(this->circuitPos, this->circuitPos + vec2f(2.0f, 2.0f)));
	}

	vec2f boardFrameLocationOfCenter() const {
		if (!(this->inCircuit())) return (this->boardPos + vec2f(1.0f, 1.0f));
		CoordinateFrame frame = CoordinateFrame(this->boardPos, this->boardPos + vec2f(2.0f, 2.0f), vec2i(constants::circuitBoardSize, constants::circuitBoardSize));
		return frame.toContainer(this->circuitPos + vec2f(1.0f, 1.0f));
	}



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


	// returns a distance measurement which is used for teleporters and such
	double modifiedDistanceTo(const GameLocation loc) {
		if (*this == loc) return 100000.0;

		double distance = 0.0;

		if (boardPos != loc.boardPos)
			distance += 100; //prioritize things on the same square

		// use the actual length
		vec2d diff = loc.boardFrameLocationOfCenter() - this->boardFrameLocationOfCenter();
		distance += diff.length();

		// add in an additional bit for the angle
		double angle = math::radiansToDegrees(atan2f((float)diff.y, (float) diff.x));
		if (angle + .00001 > 360.0) angle += .00001 - 360.0;

		distance += angle * 0.000000001;

		return distance;
	}
};

inline bool operator == (const GameLocation &a, const GameLocation &b)
{
    return a.boardPos == b.boardPos && a.circuitPos == b.circuitPos;
}

// this is the only operator we currently need to define.
inline bool operator < (const GameLocation &a, const GameLocation &b)
{
	if (a.boardPos.x < b.boardPos.x) return true;
	else if (a.boardPos.x > b.boardPos.x) return false;
	
	if (a.boardPos.y < b.boardPos.y) return true;
	else if (a.boardPos.y > b.boardPos.y) return false;

	if (a.circuitPos.x < b.circuitPos.x) return true;
    else if (a.circuitPos.x > b.circuitPos.x) return false;

	if (a.circuitPos.y < b.circuitPos.y) return true;
	else if (a.circuitPos.y > b.circuitPos.y) return false;

	return false; //equal
}
