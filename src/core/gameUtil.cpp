
#include "main.h"

Bitmap GameUtil::processGlow(const Bitmap &bmpStart, float glowStrength, const vec3f &glowColor, const vec3f &textColor)
{
    const int border = 4;
    const float glowStrength2 = glowStrength * glowStrength;
    Bitmap bmpExpanded(bmpStart.dimX() + border * 2, bmpStart.dimY() + border * 2, bmpStart(0, 0));
    for (auto &p : bmpStart)
        bmpExpanded(p.x + border, p.y + border) = p.value;

    Grid2f bmpAdd(bmpExpanded.dimX(), bmpExpanded.dimY(), 0.0f);
    for (auto &p : bmpExpanded)
    {
        const int glowRadius = 4;

        const float glowScale = 0.5f * ((float)p.value.a / 255.0f);
        if (glowScale > 0.0f)
        {
            for (int xOffset = -glowRadius; xOffset <= glowRadius; xOffset++)
                for (int yOffset = -glowRadius; yOffset <= glowRadius; yOffset++)
                {
                    const int x = xOffset + (int)p.x;
                    const int y = yOffset + (int)p.y;
                    if (bmpAdd.coordValid(x, y))
                    {
                        const float radiusSq = float(xOffset * xOffset + yOffset * yOffset);
                        const float value = expf(-radiusSq / glowStrength2);
                        bmpAdd(x, y) += value * glowScale;
                    }
                }
        }
    }

    Bitmap bmpOut = bmpExpanded;
    for (auto &p : bmpOut)
    {
        if (p.value.a == 255)
            continue;

        const float v = math::clamp(bmpAdd(p.x, p.y), 0.0f, 1.0f);
        if (v == 0.0f)
            continue;

        const float baseAlpha = p.value.a / 255.0f;
        const vec3f baseColor = textColor * baseAlpha;

        p.value = RGBColor(vec4f(baseColor + (1.0f - baseAlpha) * glowColor, v));
    }

    //LodePNG::save(bmpOut, "C:/code/debug.png");
    //cin.get();

    return bmpOut;
}

rect2f GameUtil::locationInLocationToWindowRect(const vec2f &canonicalDims, const GameLocation &location, const GameLocation &containingLocation, int size)
{
    // this is really weird.  If containing location is not in a circuit, it's the same as locationToWindowRect
    // otherwise, it returns the screen rectangle corresponding to location assuming that location is in the displayed circuit at its boardlocation

    if (!containingLocation.inCircuit() && !location.inCircuit())
        return boardToWindowRect(canonicalDims, location.boardPos, size);

    if (!containingLocation.inCircuit() && location.inCircuit())
    {
        rect2f containingRect = boardToWindowRect(canonicalDims, location.boardPos, 2);
        // I know I've already inlemented something to do this, but I don't remember how it works
        vec2f subboxSize = containingRect.extent() / constants::circuitBoardSize;
        return rect2f(
            containingRect.min().x + subboxSize.x*location.circuitPos.x,
            containingRect.min().y + subboxSize.y*location.circuitPos.y,
            containingRect.min().x + subboxSize.x*(size + location.circuitPos.x),
            containingRect.min().y + subboxSize.y*(size + location.circuitPos.y)
            );
    }
    if (containingLocation.inCircuit() && !location.inCircuit())
        return circuitToWindowRect(canonicalDims, location.boardPos, size);

    if (containingLocation.inCircuit() && location.inCircuit())
    {
        rect2f containingRect = circuitToWindowRect(canonicalDims, location.boardPos, 2);
        // I know I've already inlemented something to do this, but I don't remember how it works
        vec2f subboxSize = containingRect.extent() / constants::circuitBoardSize;
        return rect2f(
            containingRect.min().x + subboxSize.x*location.circuitPos.x,
            containingRect.min().y + subboxSize.y*location.circuitPos.y,
            containingRect.min().x + subboxSize.x*(size + location.circuitPos.x),
            containingRect.min().y + subboxSize.y*(size + location.circuitPos.y)
            );
    }

    return rect2f();
}

rect2f GameUtil::locationToWindowRect(const vec2f &canonicalDims, const GameLocation &location, int size)
{
    if (location.inCircuit())
        return circuitToWindowRect(canonicalDims, location.circuitPos, size);
    else
        return boardToWindowRect(canonicalDims, location.boardPos, size);
}

vec2f GameUtil::miniBoardToWindow(const vec2f &canonicalDims, const GameLocation &location)
{
    const vec2f canonicalStart = params().boardCanonicalStart + location.boardPos * params().canonicalCellSize;
    const vec2f circuitOffset = vec2f(location.circuitPos) / float(params().circuitDims.x);

    const vec2f canonical = canonicalStart + circuitOffset * (float)params().canonicalCellSize * 2.0f;
    return GameUtil::canonicalToWindow(canonicalDims, canonical);
}

vector< map< string, string > > GameUtil::readTSVFile(const string &filename)
{
    const auto lines = util::getFileLines(filename);

    const auto header = util::split(lines[0], '\t');

    vector< map< string, string > > result;

    for (int lineIndex = 1; lineIndex < lines.size(); lineIndex++)
    {
        result.push_back(map< string, string >());
        auto &line = result.back();

        int wordIndex = 0;
        for (const string &word : util::split(util::remove(lines[lineIndex], "\""), '\t'))
        {
            if (wordIndex < header.size())
                line[header[wordIndex++]] = word;
        }
    }

    return result;
}

pair<vec2f, float> GameUtil::computeChargeScreenPos(const GameLocation &locationA, const GameLocation &locationB, float s, ChargeType level, const vec2f &canonicalDims)
{
    const float chargeStrengthFactor = 1.0f + (int)level * constants::chargeScaleWithLevelFactor;
    const float scaleFactorA = locationA.inCircuit() ? 0.35f : 1.0f;
    const float scaleFactorB = locationB.inCircuit() ? 0.35f : 1.0f;

    const vec2f screenA = locationA.toScreenCoordMainBoard(canonicalDims);
    const vec2f screenB = locationB.toScreenCoordMainBoard(canonicalDims);

    const vec2f screenPos = math::lerp(screenA, screenB, s);
    const float screenSize = chargeStrengthFactor * math::lerp(scaleFactorA, scaleFactorB, s) * constants::canonicalChargeSize * GameUtil::windowScaleFactor(canonicalDims);
    return make_pair(screenPos, screenSize);
}

pair<vec2f, float> GameUtil::computeChargeScreenPosCircuit(const GameLocation &locationA, const GameLocation &locationB, float s, ChargeType level, const vec2f &canonicalDims)
{
    const float chargeStrengthFactor = 1.0f + (int)level * constants::chargeScaleWithLevelFactor;

    const vec2f screenA = locationA.toScreenCoordCircuitBoard(canonicalDims);
    const vec2f screenB = locationB.toScreenCoordCircuitBoard(canonicalDims);

    const vec2f screenPos = math::lerp(screenA, screenB, s);
    const float screenSize = chargeStrengthFactor * constants::canonicalChargeSize * GameUtil::windowScaleFactor(canonicalDims);
    return make_pair(screenPos, screenSize);
}

