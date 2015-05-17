
struct ComponentInfo
{
    ComponentInfo() {}
    ComponentInfo(const map<string, string> &m)
    {
        name = m.at("Component name");
        description = m.at("Description");
        menuCoordinate.x = convert::toInt(m.at("Menu X"));
        menuCoordinate.y = convert::toInt(m.at("Menu Y"));
        hotkey = m.at("Hotkey");

        colorUpgrades = (m.at("Colored") == "yes");
        grayUpgrade = (m.at("Grayed") == "yes");

        background = true;
    }

    string name;
    string description;
    string hotkey;
    vec2i menuCoordinate;

    bool colorUpgrades;
    bool grayUpgrade;
    bool background;
};

enum ChargeType
{
    ChargeRed,
    ChargeOrange,
    ChargeYellow,
    ChargeGreen,
    ChargeBlue,

    //
    // Special colors used by certain types of buildings
    //
    ChargeNone,
    ChargeGray,
};

struct Database
{
    void init();

    const ComponentInfo& getComponent(const string &componentName) const
    {
        MLIB_ASSERT_STR(components.count(componentName) > 0, "component not found");
        return *(components.at(componentName));
    }

    Texture& getTexture(RendererSDL &renderer, const ComponentInfo &info, ChargeType charge = ChargeNone);
    Texture& getTexture(RendererSDL &renderer, const string &textureName, ChargeType charge = ChargeNone);

    map<string, ComponentInfo*> components;
    map<string, Texture*> textures;
};
