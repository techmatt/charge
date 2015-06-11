
class GameUtil
{
public:
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

    static vec2f canonicalToWindow(const vec2f &canonicalDims, const vec2f &v)
    {
        return coordinateRemap(params().canonicalDims, canonicalDims, v);
    }

	static rect2f canonicalToWindow(const vec2f &canonicalDims, const rect2f &r)
	{
		return rect2f(canonicalToWindow(canonicalDims, r.min()), canonicalToWindow(canonicalDims, r.max()));
	}

	static rect2f canonicalToWindow(const vec2i &canonicalDims, const rect2i &r)
	{
		return rect2f(canonicalToWindow(canonicalDims, r.min()), canonicalToWindow(canonicalDims, r.max()));
	}

    static vec2f windowToCanonical(const vec2f &canonicalDims, const vec2f &v)
    {
        return coordinateRemap(canonicalDims, params().canonicalDims, v);
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

    static vec2f windowToBoard(const vec2f &canonicalDims, const vec2f &v)
    {
        const vec2f canonical = windowToCanonical(canonicalDims, v);
        return canonicalToBoard(canonical);
    }


    static vec2f windowToCircuit(const vec2f &canonicalDims, const vec2f &v)
    {
        const vec2f canonical = windowToCanonical(canonicalDims, v);
        return canonicalToCircuit(canonical);
    }

    static rect2f boardToWindowRect(const vec2f &canonicalDims, const vec2i &boardCoord, int size)
    {
        const vec2i canonicalBase = params().boardCanonicalStart + boardCoord * params().canonicalCellSize;
        const rect2f canonicalRect = rect2f(canonicalBase, canonicalBase + size * vec2i(params().canonicalCellSize, params().canonicalCellSize));
        const rect2f screenRect = GameUtil::canonicalToWindow(canonicalDims, canonicalRect);
        return screenRect;
    }

    static rect2f circuitToWindowRect(const vec2f &canonicalDims, const vec2i &circuitCoord, int size)
    {
        const vec2i canonicalBase = params().circuitCanonicalStart + circuitCoord * params().canonicalCellSize;
        const rect2f canonicalRect = rect2f(canonicalBase, canonicalBase + size * vec2i(params().canonicalCellSize, params().canonicalCellSize));
        const rect2f screenRect = GameUtil::canonicalToWindow(canonicalDims, canonicalRect);
        return screenRect;
    }

    static rect2f locationToWindowRect(const vec2f &canonicalDims, const GameLocation &location, int size);
	static rect2f locationInLocationToWindowRect(const vec2f &canonicalDims, const GameLocation &location, const GameLocation &containingLocation, int size);
    
    static vec2f boardToWindow(const vec2f &canonicalDims, const vec2i &boardCoord)
    {
        const vec2f canonical = params().boardCanonicalStart + boardCoord * params().canonicalCellSize;
        return GameUtil::canonicalToWindow(canonicalDims, canonical);
    }

    static vec2f miniBoardToWindow(const vec2f &canonicalDims, const GameLocation &location);

    static vec2f circuitToWindow(const vec2f &canonicalDims, const vec2i &circuitCoord)
    {
        const vec2f canonical = params().circuitCanonicalStart + circuitCoord * params().canonicalCellSize;
        return GameUtil::canonicalToWindow(canonicalDims, canonical);
    }

    static float windowScaleFactor(const vec2f &canonicalDims)
    {
        return (float)canonicalDims.x / (float)params().canonicalDims.x;
    }

	static RGBColor chargeColor(ChargeType type)
	{
		switch (type)
		{
		case ChargeType::Gray:
            return RGBColor(150, 150, 150);
		case ChargeType::Red:
			return RGBColor(255, 0, 0);
		case ChargeType::Orange:
			return RGBColor(255, 128, 0);
		case ChargeType::Yellow:
			return RGBColor(255, 255, 0);
		case ChargeType::Green:
			return RGBColor(0, 200, 0);
		case ChargeType::Blue:
			return RGBColor(0, 0, 255);
		case ChargeType::None:
			return RGBColor(255, 255, 255);
		}
		return RGBColor(0, 0, 0);
	}

    static string suffixFromCharge(ChargeType type)
    {
        switch (type)
        {
        case ChargeType::Gray:
            return "Gray";
        case ChargeType::Red:
            return "Red";
        case ChargeType::Orange:
            return "Orange";
        case ChargeType::Yellow:
            return "Yellow";
        case ChargeType::Green:
            return "Green";
        case ChargeType::Blue:
            return "Blue";
        case ChargeType::None:
            return "";
        }
        return "Invalid";
    }

    static string suffixFromSpeed(WireType speed)
    {
        switch (speed)
        {
        case WireType::MajorDelay:
            return "MajorDelay";
        case WireType::MinorDelay:
            return "MinorDelay";
        case WireType::Standard:
            return "";
        case WireType::MinorAccelerator:
            return "MinorAccelerator";
        case WireType::MajorAccelerator:
            return "MajorAccelerator";
        }
        return "Invalid";
    }

    static string speedToTextureName(WireType speed)
    {
        switch (speed)
        {
        case WireType::MajorDelay:
            return "WireMajorDelay";
        case WireType::MinorDelay:
            return "WireMinorDelay";
        case WireType::Standard:
            return "WireStandard";
        case WireType::MinorAccelerator:
            return "WireMinorAccelerator";
        case WireType::MajorAccelerator:
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

    static vec2f getCanonicalSize()
    {
        return params().canonicalDims;
    }

    static pair<vec2f, float> computeChargeScreenPos(const GameLocation &locationA, const GameLocation &locationB, float s, ChargeType level, const vec2f &canonicalDims);
    static pair<vec2f, float> computeChargeScreenPosCircuit(const GameLocation &locationA, const GameLocation &locationB, float s, ChargeType level, const vec2f &canonicalDims);

    static vector< map< string, string > > readTSVFile(const string &filename);
};

namespace math
{
    static rect2i round(const rect2f &r)
    {
        return rect2i(math::round(r.min()), math::round(r.max()));
    }
}