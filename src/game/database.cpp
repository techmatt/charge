
#include "main.h"

void Database::init()
{
    for (auto &line : GameUtil::readCSVFile(params().assetDir + "database.csv"))
    {
        ComponentInfo *component = new ComponentInfo(line);
        int a = 5;
        components[component->name] = component;
        int b = 6;
    }
}

Texture& Database::getTexture(RendererSDL &renderer, const string &textureName, ChargeType charge)
{
    if (textures.count(textureName) == 0)
    {
        const string filename = params().assetDir + "textures/" + textureName + ".png";
        MLIB_ASSERT_STR(util::fileExists(filename), "File not found");
        Bitmap bmp = LodePNG::load(filename);

        bool isHalfAlphaTexture = (textureName == "Selector");

        if (isHalfAlphaTexture)
        {
            for (auto &p : bmp)
                p.value.a = 128;
        }
        else
        {
            const RGBColor chargeColor = GameUtil::chargeColor(charge);

            for (auto &p : bmp)
            {
                if (p.value == Colors::Cyan())
                    p.value.a = 255;
                else
                    p.value.a = 0;

                if (charge != ChargeNone && p.value == Colors::Magenta())
                {
                    p.value = chargeColor;
                }
            }

        }

        Texture *t = new Texture(renderer, bmp);

        //
        // configure alpha mode
        //

        bool isAlphaTexture = true;

        if (textureName == "Background") isAlphaTexture = false;

        if (isAlphaTexture)
        {
            SDL_SetTextureBlendMode(t->SDL(), SDL_BLENDMODE_BLEND);
        }
        else
        {
            SDL_SetTextureBlendMode(t->SDL(), SDL_BLENDMODE_NONE);
        }

        textures[textureName] = t;
    }
    return *textures.at(textureName);
}
