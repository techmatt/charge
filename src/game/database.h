
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
    }

    string name;
    string description;
    string hotkey;
    vec2i menuCoordinate;

    bool colorUpgrades;
    bool grayUpgrade;
};

struct Database
{
    void init();

    const ComponentInfo& getComponent(const string &componentName) const
    {
        MLIB_ASSERT_STR(components.count(componentName) > 0, "component not found");
        return *(components.at(componentName));
    }

    map<string, ComponentInfo*> components;
};
