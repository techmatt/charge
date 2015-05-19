
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

    static vec2f windowToBoard(const vec2f &windowDims, const vec2f &v)
    {
        const vec2f canonical = windowToCanonical(windowDims, v);
        return canonicalToBoard(canonical);
        //return coordinateRemap(rect2f(vec2f::origin(), params().boardDims), params().boardCanonicalRect, v);
    }

    static rect2f boardToWindowRect(const vec2f &windowDims, const vec2i &boardCoord, int size)
    {
        const vec2i canonicalBase = params().boardCanonicalStart + boardCoord * params().boardCanonicalCellSize;
        const rect2f canonicalRect = rect2f(canonicalBase, canonicalBase + size * vec2i(params().boardCanonicalCellSize, params().boardCanonicalCellSize));
        const rect2f screenRect = GameUtil::canonicalToWindow(windowDims, canonicalRect);
        return screenRect;
    }

    static vec2f boardToWindow(const vec2f &windowDims, const vec2i &boardCoord)
    {
        const vec2f canonical = params().boardCanonicalStart + boardCoord * params().boardCanonicalCellSize;
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

    static pair<vec2f, float> computeChargeScreenPos(const GameLocation &locationA, const GameLocation &locationB, float s, ChargeType level, const vec2f &windowDims);

    static vector< map< string, string > > readCSVFile(const string &filename);
};

namespace math
{
    static rect2i round(const rect2f &r)
    {
        return rect2i(math::round(r.min()), math::round(r.max()));
    }
}