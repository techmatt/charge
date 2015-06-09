
#include "main.h"

void Database::init()
{
    alphabetKeyToComponent.resize('Z' - 'A');

    for (auto &line : GameUtil::readTSVFile(params().assetDir + "components.txt"))
    {
        ComponentInfo *component = new ComponentInfo(line);
        if (component->hotkey[0] >= 'A' && component->hotkey[0] <= 'Z')
        {
            alphabetKeyToComponent[component->hotkey[0] - 'A'] = component;
        }
        components[component->name] = component;
    }

    for (auto &line : GameUtil::readTSVFile(params().assetDir + "levels.txt"))
    {
        puzzles.push_back(PuzzleInfo(line));
    }

    fonts[FontLevelName] = FontInfo("arial", 36.0f, RGBColor(0, 0, 0));
    fonts[FontTooltipName] = FontInfo("trebuc", 36.0f, RGBColor(247, 222, 83));
    fonts[FontTooltipDescription] = FontInfo("trebuc", 36.0f, RGBColor(200, 150, 200));
    fonts[FontTooltipHotkey] = FontInfo("arial", 36.0f, RGBColor(50, 200, 50));
    fonts[FontComponentAttribute] = FontInfo("trebuc", 36.0f, RGBColor(0, 0, 0));
}

void Database::initTextures(Renderer &renderer)
{
    for (int chargeLevel = ChargeRed; chargeLevel <= ChargeBlue; chargeLevel++)
    {
        chargeTextures[chargeLevel] = &getTexture(renderer, "ChargeTexture" + to_string(chargeLevel - 1));
    }

    for (int boundary = 0; boundary < boundaryTextureCount; boundary++)
    {
        boundaryTextures[boundary] = &getTexture(renderer, "BoundaryTextures" + to_string(boundary));
    }

    for (int preferenceLevel = 0; preferenceLevel < constants::maxPreferenceLevel; preferenceLevel++)
    {
        preferenceTextures[preferenceLevel] = &getTexture(renderer, "PreferenceMask" + to_string(preferenceLevel));
    }

    squareBlocked = &getTexture(renderer, "SquareBlocked");
    squareOpen = &getTexture(renderer, "SquareOpen");
}

Texture& Database::getTexture(Renderer &renderer, const string &textureName)
{
    return getTexture(renderer, textureName, ComponentModifiers());
}

Texture& Database::getTexture(Renderer &renderer, const string &componentName, const ComponentModifiers &modifiers, bool getStoredChargeLayer)
{
    if (modifiers.speed != WireStandard)
    {
        return getTexture(renderer, GameUtil::speedToTextureName(modifiers.speed), ComponentModifiers());
    }

    string baseTextureName = componentName + GameUtil::suffixFromCharge(modifiers.color);

    if (modifiers.boundary == CircuitBoundaryInvalid && componentName == "CircuitBoundary") // this is when the boundary is rendered as part of a UI button
        baseTextureName += "Open";
    if (modifiers.boundary == CircuitBoundaryOpen)
        baseTextureName += "Open";
    if (modifiers.boundary == CircuitBoundaryClosed)
        baseTextureName += "Closed";

    const string fullTextureName = baseTextureName + (getStoredChargeLayer ? "StoredCharge" : "");

    if (textures.count(fullTextureName) == 0)
    {
        const string filename = params().assetDir + "textures/" + baseTextureName + ".png";
        MLIB_ASSERT_STR(util::fileExists(filename), "File not found");
        Bitmap bmp = LodePNG::load(filename);

        const string alphaFilename = util::replace(filename, ".png", "Alpha.png");

        bool isHalfAlphaTexture = (componentName == "Selector" || componentName == "PuzzleSelector" || componentName == "SquareBlocked" || componentName == "SquareOpen");

        if (isHalfAlphaTexture)
        {
            for (const auto &p : bmp)
                p.value.a = 105;
        }
        else if (util::startsWith(componentName, "ChargeTexture"))
        {
            GameUtil::overlayAlpha(bmp, params().assetDir + "textures/ChargeTextureAlpha.png");
        }
        else if (util::fileExists(alphaFilename))
        {
            GameUtil::overlayAlpha(bmp, alphaFilename);
        }
        else if (util::startsWith(componentName, "BoundaryTextures"))
        {
            Texture &border = getTexture(renderer, "Border");
            for (const auto &p : bmp)
            {
                if (p.value == Colors::Magenta())
                    p.value = border.bmp()(p.x, p.y);

                if (p.value == Colors::Cyan())
                    p.value = RGBColor(255, 255, 255, 0);
                else
                    p.value.a = 255;
            }
        }
        else if (getStoredChargeLayer)
        {
            for (const auto &p : bmp)
            {
                if (p.value == Colors::Magenta())
                    p.value = RGBColor(255, 255, 255, 255);
                else
                    p.value = RGBColor(0, 0, 0, 0);
            }
        }
        else
        {
            for (const auto &p : bmp)
            {
                if (p.value == Colors::Cyan())
                    p.value = RGBColor(255, 255, 255, 0);
                else
                    p.value.a = 255;

                if (p.value == Colors::Magenta())
                {
                    p.value = RGBColor(0, 0, 0, 255);
                }
            }
        }

        Texture *t = new Texture(renderer, bmp);

        //
        // configure alpha mode
        //

        bool isAlphaTexture = true;

        if (componentName == "Background") isAlphaTexture = false;

        if (isAlphaTexture)
        {
            SDL_SetTextureBlendMode(t->SDL(), SDL_BLENDMODE_BLEND);
        }
        else
        {
            SDL_SetTextureBlendMode(t->SDL(), SDL_BLENDMODE_NONE);
        }

        textures[fullTextureName] = t;
    }
    return *textures.at(fullTextureName);
}

const ComponentInfo* Database::componentFromKey(SDL_Keycode key) const
{
    if (key >= SDLK_a && key <= SDLK_z)
    {
        return alphabetKeyToComponent[key - SDLK_a];
    }
    return nullptr;
}
