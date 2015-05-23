
class GameUtil
{
public:

    /*
    coordinate frame proposal:

    enum CoordinateFrame
    {
        FrameWindow, // 0, 0 to windowDims
        FrameNormalizedWindow, // normalized device coordiantes, 0,0 to 1,1
        FrameCanonical, // 0, 0 to params().canonicalDims
        FrameMainBoard, // 0, 0 to 24, 24
        FrameCircuitBoard, // 0,0 to 7, 7
        FrameComponentMenu, // 0, 0 to 6, 3 
        FramePreferenceMenu, // 0, 0, to 5, 0
        etc.
    };

    vec2f transform(const vec2f &in, CoordinateFrame start, CoordianteFrame end, const vec2i &windowDims)
    rect2f transform(const rect2f &in, CoordinateFrame start, CoordianteFrame end, const vec2i &windowDims)
    rect2f getMenuRect(const vec2i &in, CoordinateFrame menu, CoordianteFrame end, const vec2i &windowDims)

    // if start is FrameMainBoard, this refers to the coordinate on the main board, including the circuitPos mini-board offset.
    // if start is FrameCircuitBoard, this refers to the coordinate on the active circuit board, and ignores boardPos
    // I think this should transform the top-left of "in", but different options are viable.
    vec2f transform(const GameLocation &in, CoordinateFrame start, CoordianteFrame end, const vec2i &windowDims)

    // transforms a rectangle starting at the top-left of "in" and of size (cellSize, cellSize) into the target coordinate frame.
    rect2f transform(const GameLocation &in, int cellSize, CoordinateFrame start, CoordianteFrame end, const vec2i &windowDims)

    */
	

    static vec2f coordinateRemap(const vec2f &dimStart, const vec2f &dimEnd, const vec2f &v)
    {
        vec2f result;
        result.x = math::linearMap(0.0f, dimStart.x, 0.0f, dimEnd.x, v.x);
        result.y = math::linearMap(0.0f, dimStart.y, 0.0f, dimEnd.y, v.y);
        return result;
    }

    static vec2f coordinateRemap(const rect2f &start, const rect2f &end, const vec2f &v)
    {
        vec2f result;
        result.x = math::linearMap(start.min().x, start.max().x, end.min().x, end.max().x, v.x);
        result.y = math::linearMap(start.min().y, start.max().y, end.min().y, end.max().y, v.y);
        return result;
    }

    static vec2f canonicalToWindow(const vec2f &windowDims, const vec2f &v)
    {
        return coordinateRemap(params().canonicalDims, windowDims, v);
    }

	static rect2f canonicalToWindow(const vec2f &windowDims, const rect2f &r)
	{
		return rect2f(canonicalToWindow(windowDims, r.min()), canonicalToWindow(windowDims, r.max()));
	}

	static rect2f canonicalToWindow(const vec2i &windowDims, const rect2i &r)
	{
		return rect2f(canonicalToWindow(windowDims, r.min()), canonicalToWindow(windowDims, r.max()));
	}

    static vec2f windowToCanonical(const vec2f &windowDims, const vec2f &v)
    {
        return coordinateRemap(windowDims, params().canonicalDims, v);
    }

    static vec2f boardToCanonical(const vec2f &v)
    {
        return coordinateRemap(rect2f(vec2f::origin(), params().boardDims), params().boardCanonicalRect, v);
    }

    static vec2f canonicalToBoard(const vec2f &v)
    {
        return coordinateRemap(params().boardCanonicalRect, rect2f(vec2f::origin(), params().boardDims), v);
    }

    static vec2f canonicalToCircuit(const vec2f &v)
    {
        return coordinateRemap(params().circuitCanonicalRect, rect2f(vec2f::origin(), params().circuitDims), v);
    }

    static vec2f windowToBoard(const vec2f &windowDims, const vec2f &v)
    {
        const vec2f canonical = windowToCanonical(windowDims, v);
        return canonicalToBoard(canonical);
    }

    static vec2f windowToCircuit(const vec2f &windowDims, const vec2f &v)
    {
        const vec2f canonical = windowToCanonical(windowDims, v);
        return canonicalToCircuit(canonical);
    }

    static rect2f boardToWindowRect(const vec2f &windowDims, const vec2i &boardCoord, int size)
    {
        const vec2i canonicalBase = params().boardCanonicalStart + boardCoord * params().canonicalCellSize;
        const rect2f canonicalRect = rect2f(canonicalBase, canonicalBase + size * vec2i(params().canonicalCellSize, params().canonicalCellSize));
        const rect2f screenRect = GameUtil::canonicalToWindow(windowDims, canonicalRect);
        return screenRect;
    }

    static rect2f circuitToWindowRect(const vec2f &windowDims, const vec2i &circuitCoord, int size)
    {
        const vec2i canonicalBase = params().circuitCanonicalStart + circuitCoord * params().canonicalCellSize;
        const rect2f canonicalRect = rect2f(canonicalBase, canonicalBase + size * vec2i(params().canonicalCellSize, params().canonicalCellSize));
        const rect2f screenRect = GameUtil::canonicalToWindow(windowDims, canonicalRect);
        return screenRect;
    }

    static rect2f locationToWindowRect(const vec2f &windowDims, const GameLocation &location, int size);
    
    static vec2f boardToWindow(const vec2f &windowDims, const vec2i &boardCoord)
    {
        const vec2f canonical = params().boardCanonicalStart + boardCoord * params().canonicalCellSize;
        return GameUtil::canonicalToWindow(windowDims, canonical);
    }

    static vec2f miniBoardToWindow(const vec2f &windowDims, const GameLocation &location);

    static vec2f circuitToWindow(const vec2f &windowDims, const vec2i &circuitCoord)
    {
        const vec2f canonical = params().circuitCanonicalStart + circuitCoord * params().canonicalCellSize;
        return GameUtil::canonicalToWindow(windowDims, canonical);
    }

    static float windowScaleFactor(const vec2f &windowDims)
    {
        return (float)windowDims.x / (float)params().canonicalDims.x;
    }

	static RGBColor chargeColor(ChargeType type)
	{
		switch (type)
		{
		case ChargeGray:
			return RGBColor(128, 128, 128);
		case ChargeRed:
			return RGBColor(255, 0, 0);
		case ChargeOrange:
			return RGBColor(255, 128, 0);
		case ChargeYellow:
			return RGBColor(255, 255, 0);
		case ChargeGreen:
			return RGBColor(0, 200, 0);
		case ChargeBlue:
			return RGBColor(0, 0, 255);
		case ChargeNone:
			return RGBColor(255, 255, 255);
		}
		return RGBColor(0, 0, 0);
	}

    static string suffixFromCharge(ChargeType type)
    {
        switch (type)
        {
        case ChargeGray:
            return "Gray";
        case ChargeRed:
            return "Red";
        case ChargeOrange:
            return "Orange";
        case ChargeYellow:
            return "Yellow";
        case ChargeGreen:
            return "Green";
        case ChargeBlue:
            return "Blue";
        case ChargeNone:
            return "";
        }
        return "Invalid";
    }

    static string speedToTextureName(WireSpeedType speed)
    {
        switch (speed)
        {
        case WireMajorDelay:
            return "WireMajorDelay";
        case WireMinorDelay:
            return "WireMinorDelay";
        case WireMinorAccelerator:
            return "WireMinorAccelerator";
        case WireMajorAccelerator:
            return "WireMajorAccelerator";
        default:
            return "invalid";
        }
    }

    static void overlayAlpha(Bitmap &bmp, const string &filename)
    {
        Bitmap alpha = LodePNG::load(filename);
        for (const auto &p : alpha)
            bmp(p.x, p.y).a = p.value.r;
    }

    static pair<vec2f, float> computeChargeScreenPos(const GameLocation &locationA, const GameLocation &locationB, float s, ChargeType level, const vec2f &windowDims);
    static pair<vec2f, float> computeChargeScreenPosCircuit(const GameLocation &locationA, const GameLocation &locationB, float s, ChargeType level, const vec2f &windowDims);

    static vector< map< string, string > > readCSVFile(const string &filename);
};

namespace math
{
    static rect2i round(const rect2f &r)
    {
        return rect2i(math::round(r.min()), math::round(r.max()));
    }
}