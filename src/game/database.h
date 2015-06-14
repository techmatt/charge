
enum class FontType
{
    LevelName,
    TooltipName,
    TooltipDescriptionA,
    TooltipDescriptionB,
    TooltipKeyA,
    TooltipKeyB,
    TooltipHotkey,
    ComponentAttribute,
    Count,
};

struct FontInfo
{
    FontInfo() {}
    FontInfo(const string &_name, float _height, RGBColor _color)
    {
        name = _name;
        height = _height;
        color = _color;
    }
    string name;
    float height;
    RGBColor color;
};

struct ComponentInfo
{
    ComponentInfo() {}
    ComponentInfo(const map<string, string> &m)
    {
        name = m.at("Component name");
        semanticName = m.at("Semantic name");
        description = util::replace(m.at("Description"), '|', '\n');
        menuCoordinate.x = convert::toInt(m.at("Menu X"));
        menuCoordinate.y = convert::toInt(m.at("Menu Y"));
        hotkey = m.at("Hotkey");

        colorUpgrades = (m.at("Colored") == "yes");
        grayUpgrade = (m.at("Grayed") == "yes");

        background = true;

        holdsCharge = (name == "Hold" || name == "ChargeGoal");
        hasStoredChargeLayer = (name == "Amplifier" || name == "FilteredAmplifier" || holdsCharge);
    }

    ChargeType defaultPrimaryCharge() const
    {
        if (name == "FilteredAmplifier")
            return ChargeType::Orange;
        if (colorUpgrades)
            return ChargeType::Red;
        if (util::endsWith(name, "GrayProxy"))
            return ChargeType::Gray;
        return ChargeType::None;
    }

    string name;
    string semanticName;
    string description;
    string hotkey;
    vec2i menuCoordinate;

    bool grayUpgrade;
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
        tip = m.at("Tip");
    }

    int index;
    string name;
    string filename;
    string tip;
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

    pair<const ComponentInfo*, ChargeType> componentFromKey(SDL_Keycode key) const;

    Texture& getTexture(Renderer &renderer, const string &textureName);
    Texture& getTexture(Renderer &renderer, const string &textureName, const ComponentModifiers &modifiers, bool getStoredChargeLayer = false);
    //Texture& getTexture(Renderer &renderer, const string &textureName, ChargeType chargePrimary, ChargeType chargeSecondary, WireType speed);

    map<string, ComponentInfo*> components;

    vector<PuzzleInfo> puzzles;

    //
    // commonly accessed textures
    //
    static const int boundaryTextureCount = 23;
    Texture* boundaryTextures[boundaryTextureCount];
    Texture* chargeTextures[constants::maxChargeLevel + 2];
    Texture* preferenceTextures[constants::maxPreferenceLevel];
    Texture* squareBlocked;
    Texture* squareOpen;

    FontInfo fonts[(int)FontType::Count];

private:
    
    map<string, Texture*> textures;

    // map from key to component, goes from 0 to 25
    vector<ComponentInfo*> alphabetKeyToComponent;
};
