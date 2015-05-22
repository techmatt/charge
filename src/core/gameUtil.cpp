
#include "main.h"

rect2f GameUtil::locationToWindowRect(const vec2f &windowDims, const GameLocation &location, int size)
{
    if (location.inCircuit())
        return circuitToWindowRect(windowDims, location.circuitPos, size);
    else
        return boardToWindowRect(windowDims, location.boardPos, size);
}

vec2f GameUtil::miniBoardToWindow(const vec2f &windowDims, const GameLocation &location)
{
    const vec2f canonicalStart = params().boardCanonicalStart + location.boardPos * params().canonicalCellSize;
    const vec2f circuitOffset = vec2f(location.circuitPos) / float(params().circuitDims.x);

    const vec2f canonical = canonicalStart + circuitOffset * (float)params().canonicalCellSize * 2.0f;
    return GameUtil::canonicalToWindow(windowDims, canonical);
}

vector< map< string, string > > GameUtil::readCSVFile(const string &filename)
{
    const auto lines = util::getFileLines(filename);

    const auto header = util::split(lines[0], ',');

    vector< map< string, string > > result;

    for (int lineIndex = 1; lineIndex < lines.size(); lineIndex++)
    {
        result.push_back(map< string, string >());
        auto &line = result.back();

        int wordIndex = 0;
        for (const string &word : util::split(lines[lineIndex], ','))
        {
            if (wordIndex < header.size())
                line[header[wordIndex++]] = word;
        }
    }

    //
    // TODO: deal with reading csv's with commas in description
    //

    return result;
}

pair<vec2f, float> GameUtil::computeChargeScreenPos(const GameLocation &locationA, const GameLocation &locationB, float s, ChargeType level, const vec2f &windowDims)
{
    const float scaleFactorA = 1.0f + (int)level * constants::chargeScaleWithLevelFactor;
    const float scaleFactorB = 1.0f + (int)level * constants::chargeScaleWithLevelFactor;

    const vec2f screenA = locationA.toScreenCoordMainBoard(windowDims);
    const vec2f screenB = locationB.toScreenCoordMainBoard(windowDims);

    const vec2f screenPos = math::lerp(screenA, screenB, s);
    const float screenSize = math::lerp(scaleFactorA, scaleFactorB, s) * constants::canonicalChargeSize * GameUtil::windowScaleFactor(windowDims);
    return make_pair(screenPos, screenSize);
}

pair<vec2f, float> GameUtil::computeChargeScreenPosCircuit(const GameLocation &locationA, const GameLocation &locationB, float s, ChargeType level, const vec2f &windowDims)
{
    const vec2f screenA = locationA.toScreenCoordCircuitBoard(windowDims);
    const vec2f screenB = locationB.toScreenCoordCircuitBoard(windowDims);

    const vec2f screenPos = math::lerp(screenA, screenB, s);
    const float screenSize = 1.0f + (int)level * constants::chargeScaleWithLevelFactor;
    return make_pair(screenPos, screenSize);
}