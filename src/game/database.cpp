
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

Texture& Database::getTexture(RendererSDL &renderer, const string &textureName)
{
    if (textures.count(textureName) == 0)
    {
        const string filename = params().assetDir + "textures/" + textureName + ".png";
        textures[textureName] = new Texture(renderer, filename);
    }
    return *textures.at(textureName);
}
