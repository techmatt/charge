
enum class FontType
{
    LevelName,
    TooltipName,
    TooltipDescriptionA,
    TooltipDescriptionB,
    TooltipKeyA,
    TooltipKeyB,
    ComponentAttribute,
    MenuTitle,
    LevelSelectIndex,
	VictoryPanelStandard,
	VictoryPanelGood,
	VictoryPanelBad,
	Tutorial,
    Count,
};

struct FontInfo
{
    FontInfo() {}
    FontInfo(const string &_name, float _height, RGBColor _color, float _glowStrength = 0.0f, RGBColor _glowColor = RGBColor(0, 0, 0))
    {
        name = _name;
        height = _height;
        color = _color;
        glowStrength = _glowStrength;
        glowColor = _glowColor;
    }
    string name;
    float height;
    RGBColor color;
    float glowStrength;
    RGBColor glowColor;
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
    PuzzleInfo(const map<string, string> &m, int _index)
    {
        index = _index;
        name = m.at("Puzzle name");
        //filename = m.at("Puzzle file");
		filename = purifiedName();
        stepCountPar = convert::toInt(m.at("Step count par"));
        componentCostPar = convert::toInt(m.at("Component cost par"));
        baseComponentCost = convert::toInt(m.at("Base component cost"));
		solutionCount = convert::toInt(m.at("Solution count"));
        rawTip = m.at("Tip");
        tip = util::replace(m.at("Tip"), '|', '\n');
    }

	string purifiedName() const
	{
		string result;
		for (char c : name)
		{
			if ((c >= 'A' && c <= 'Z') ||
				(c >= 'a' && c <= 'z') ||
				(c >= '0' && c <= '9'))
				result += c;
		}
		return result;
	}

    int index;
    string name;
    string filename;
    string tip;
    string rawTip;

    int stepCountPar;
    int componentCostPar;
    int baseComponentCost;
	
	int solutionCount;
};

struct Database
{
    void init();
    void initTextures(Renderer &renderer);

	void processAllCampaignLevels(AppData &app);

    bool hasComponent(const string &componentName) const
    {
        return (components.count(componentName) > 0);
    }

    const ComponentInfo& getComponent(const string &componentName) const
    {
        MLIB_ASSERT_STR(components.count(componentName) > 0, "component not found");
        return *(components.at(componentName));
    }

	const PuzzleInfo& getPuzzleInfo(const string &levelPack, int levelPackIndex) const
	{
		if (levelPack != "Campaign")
		{
			cout << "Accessing non-campaign not yet supported" << endl;
			cin.get();
		}
		if (levelPackIndex < 0 || levelPackIndex >= allPuzzles.size())
		{
			cout << "Out of bounds" << endl;
			cin.get();
		}
		return allPuzzles[levelPackIndex];
	}

    Texture& getTexture(Renderer &renderer, const string &textureName);
    Texture& getTexture(Renderer &renderer, const string &textureName, const ComponentModifiers &modifiers, bool getStoredChargeLayer = false);
    //Texture& getTexture(Renderer &renderer, const string &textureName, ChargeType chargePrimary, ChargeType chargeSecondary, WireType speed);

    TTF_Font* getFont(const string &fontName);

    map<string, ComponentInfo*> components;

    map<string, TTF_Font*> _fonts;

    vector<PuzzleInfo> allPuzzles;

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
