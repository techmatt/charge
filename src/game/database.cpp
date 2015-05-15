
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
