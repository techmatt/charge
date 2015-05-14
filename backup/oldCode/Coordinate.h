struct Coordinate
{
	Coordinate() {}
	Coordinate(int _x, int _y) { x = _x; y = _y; }
	Coordinate(const Vec2 &V) { x = int(V.x); y = int(V.y); }

	__forceinline bool InRange(int MaxX, int MaxY)
	{
		return (x >= 0 && x <= MaxX && y >= 0 && y <= MaxY);
	}

	static UINT DistSq(const Coordinate &Left, const Coordinate &Right)
	{
		return (Math::Square(Left.x - Right.x) + Math::Square(Left.y - Right.y));
	}

	Vec2 AsVec2() const
	{
		return Vec2(float(x), float(y));
	}
	String CommaSperatedString() const
	{
		return String(x) + String(", ") + String(y);
	}

	int x, y;

	static const Coordinate Invalid;
};

struct WorldLocation
{
	Coordinate Circuit;
	Coordinate Location;
};

struct Rectangle2i
{
	Rectangle2i() {}
	Rectangle2i(int x1, int y1, int x2, int y2)
	{
		TopLeft.x = x1;
		TopLeft.y = y1;
		BottomRight.x = x1;
		BottomRight.y = y1;
	}
	Rectangle2i(const Coordinate &_TopLeft, const Coordinate &_BottomRight)
	{
		TopLeft = _TopLeft;
		BottomRight = _BottomRight;
	}

	bool CoordinateInside(const Coordinate &C) const
	{
		return (C.x >= TopLeft.x && C.x <= BottomRight.x &&
				C.y >= TopLeft.y && C.y <= BottomRight.y);
	}

	Coordinate TopLeft, BottomRight;
};

__forceinline bool operator == (const Coordinate &Left, const Coordinate &Right)
{
	return (Left.x == Right.x && Left.y == Right.y);
}

__forceinline bool operator != (const Coordinate &Left, const Coordinate &Right)
{
	return (Left.x != Right.x || Left.y != Right.y);
}

__forceinline Coordinate operator + (const Coordinate &Left, const Coordinate &Right)
{
	return Coordinate(Left.x + Right.x, Left.y + Right.y);
}

__forceinline Coordinate operator - (const Coordinate &Left, const Coordinate &Right)
{
	return Coordinate(Left.x - Right.x, Left.y - Right.y);
}