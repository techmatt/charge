
#include "main.h"

void Database::init()
{
    for (auto &line : GameUtil::readCSVFile(params().assetDir + "database.csv"))
    {
        ComponentInfo *component = new ComponentInfo(line);
        components[component->name] = component;
    }
}

Texture& Database::getTexture(RendererSDL &renderer, const string &textureName, ChargeType chargePrimary, ChargeType chargeSecondary)
{
    const string fullTextureName = textureName + GameUtil::suffixFromCharge(chargePrimary) + GameUtil::suffixFromCharge(chargeSecondary);
    if (textures.count(fullTextureName) == 0)
    {
        const string filename = params().assetDir + "textures/" + textureName + GameUtil::suffixFromCharge(chargePrimary) + ".png";
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
            const RGBColor chargeSecondaryColor = GameUtil::chargeColor(chargeSecondary);

            for (auto &p : bmp)
            {
                if (p.value == Colors::Cyan())
                    p.value = RGBColor(255, 255, 255, 0);
                else
                    p.value.a = 255;

                if (p.value == Colors::Magenta())
                {
                    p.value = chargeSecondaryColor;
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

        textures[fullTextureName] = t;
    }
    return *textures.at(fullTextureName);
}
