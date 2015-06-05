
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

        background = true;

        holdsCharge = (name == "Hold" || name == "ChargeGoal");
        hasStoredChargeLayer = (name == "Amplifier" || name == "FilteredAmplifier" || holdsCharge);
    }

    ChargeType defaultPrimaryCharge() const
    {
        if (name == "FilteredAmplifier")
            return ChargeOrange;
        if (colorUpgrades)
            return ChargeRed;
        if (util::endsWith(name, "GrayProxy"))
            return ChargeGray;
        return ChargeNone;
    }

    string name;
    string description;
    string hotkey;
    vec2i menuCoordinate;

    bool colorUpgrades;
    bool background;

    bool holdsCharge;

    bool hasStoredChargeLayer;
};

struct PuzzleInfo
{
    PuzzleInfo() {}
    PuzzleInfo(const map<string, string> &m)
    {
        name = m.at("Puzzle name");
        filename = m.at("Puzzle file");
        index = convert::toInt(m.at("Number"));
    }

    int index;
    string name;
    string filename;
};

struct Database
{
    void init();
    void initTextures(Renderer &renderer);

    bool hasComponent(const string &componentName) const
    {
        return (components.count(componentName) > 0);
    }

    const ComponentInfo& getComponent(const string &componentName) const
    {
        MLIB_ASSERT_STR(components.count(componentName) > 0, "component not found");
        return *(components.at(componentName));
    }

    Texture& getTexture(Renderer &renderer, const string &textureName);
    Texture& getTexture(Renderer &renderer, const string &textureName, const ComponentModifiers &modifiers, bool getStoredChargeLayer = false);
    //Texture& getTexture(Renderer &renderer, const string &textureName, ChargeType chargePrimary, ChargeType chargeSecondary, WireSpeedType speed);

    map<string, ComponentInfo*> components;
    map<string, Texture*> textures;
    vector<PuzzleInfo> puzzles;

    //
    // commonly accessed textures
    //
    static const int boundaryTextureCount = 23;
    Texture* boundaryTextures[boundaryTextureCount];
    Texture* chargeTextures[constants::maxChargeLevel + 1];
    Texture* preferenceTextures[constants::maxPreferenceLevel];
    Texture* squareBlocked;
    Texture* squareOpen;

    
};
