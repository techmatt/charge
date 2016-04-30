
#include "main.h"

void Database::init()
{
    alphabetKeyToComponent.resize(26);

    for (auto &line : GameUtil::readTSVFile(params().assetDir + "components.txt"))
    {
        ComponentInfo *component = new ComponentInfo(line);
        if (component->hotkey[0] >= 'A' && component->hotkey[0] <= 'Z')
        {
            alphabetKeyToComponent[component->hotkey[0] - 'A'] = component;
        }
        components[component->name] = component;
    }

    auto lines = GameUtil::readTSVFile(params().assetDir + "levels.txt");
    for (int puzzleIndex = 0; puzzleIndex < (int)lines.size(); puzzleIndex++)
    {
        allPuzzles.push_back(PuzzleInfo(lines[puzzleIndex], puzzleIndex));
    }

    fonts[(int)FontType::LevelName] = FontInfo("21st", 36.0f, RGBColor(0, 0, 0));
    fonts[(int)FontType::TooltipName] = FontInfo("trebuc", 36.0f, RGBColor(247, 222, 83));
    fonts[(int)FontType::TooltipDescriptionA] = FontInfo("trebuc", 36.0f, RGBColor(250, 250, 250));
    fonts[(int)FontType::TooltipDescriptionB] = FontInfo("trebuc", 36.0f, RGBColor(255, 201, 147));
    fonts[(int)FontType::TooltipKeyA] = FontInfo("trebuc", 36.0f, RGBColor(200, 200, 200));
    fonts[(int)FontType::TooltipKeyB] = FontInfo("trebuc", 36.0f, RGBColor(183, 255, 183));
    fonts[(int)FontType::TooltipHotkey] = FontInfo("arial", 36.0f, RGBColor(50, 200, 50));
    fonts[(int)FontType::ComponentAttribute] = FontInfo("trebuc", 36.0f, RGBColor(0, 0, 0));
    fonts[(int)FontType::MenuTitle] = FontInfo("trebuc", 36.0f, RGBColor(64, 64, 64));
}

TTF_Font* Database::getFont(const string &fontName)
{
    if (_fonts.count(fontName) == 0)
    {
        _fonts[fontName] = TTF_OpenFont((params().assetDir + "fonts/" + fontName + ".ttf").c_str(), 50);
    }
    return _fonts[fontName];
}

void Database::initTextures(Renderer &renderer)
{
    if (debugCalls) cout << "Begin texture loading..." << endl;
    for (int chargeLevel = (int)ChargeType::Red; chargeLevel <= (int)ChargeType::Blue + 1; chargeLevel++)
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

    if (debugCalls) cout << "Done texture loading..." << endl;
}

void Database::processAllCampaignLevels(AppData &app)
{
    int puzzleIndex = 0;
	for (auto &puzzle : allPuzzles)
	{
		cout << "Processing puzzle " << puzzle.filename << endl;
		GameState state(app);
		const string basePuzzleFilename = params().assetDir + "levels/" + puzzle.filename + ".pzl";
		state.loadPuzzle(basePuzzleFilename, "none");
		
		state.levelPack = "Campaign";
        state.levelPackPuzzleIndex = puzzleIndex;
		state.levelPackPuzzleName = puzzle.name;
		state.puzzleFileType = "BasePuzzle";

        puzzle.baseComponentCost = state.componentCost();

		state.savePuzzle(basePuzzleFilename);
        puzzleIndex++;
	}

    ofstream file(params().assetDir + "levelsProcessed.txt");
    file << "Puzzle name\tPuzzle file\tTip\tStep count par\tComponent cost par\tBase component cost" << endl;
    puzzleIndex = 0;
	for (auto &puzzle : allPuzzles)
	{
		cout << "Processing puzzle solution " << puzzle.filename << endl;

		GameState state(app);
		const string basePuzzleFilename = params().assetDir + "providedSolutions/" + puzzle.filename + "_A.pzl";
		state.loadPuzzle(basePuzzleFilename, "none");

		state.levelPack = "Campaign";
		state.levelPackPuzzleIndex = puzzleIndex;
		state.levelPackPuzzleName = puzzle.name;
		state.puzzleFileType = "ProvidedSolution";

		state.savePuzzle(basePuzzleFilename);

        while (!state.victory)
            state.step(app);

        puzzle.stepCountPar = state.victoryInfo.stepCount;
        
        puzzle.componentCostPar = state.componentCost();

        file << puzzle.name << "\t" << puzzle.filename << "\t" << puzzle.rawTip << "\t" << puzzle.stepCountPar << "\t" << puzzle.componentCostPar << "\t" << puzzle.baseComponentCost << endl;

        puzzleIndex++;
	}
}

Texture& Database::getTexture(Renderer &renderer, const string &textureName)
{
    return getTexture(renderer, textureName, ComponentModifiers());
}

Texture& Database::getTexture(Renderer &renderer, const string &componentName, const ComponentModifiers &modifiers, bool getStoredChargeLayer)
{
    if (modifiers.speed != WireType::Standard)
    {
        return getTexture(renderer, GameUtil::speedToTextureName(modifiers.speed), ComponentModifiers());
    }

    string baseTextureName = componentName + GameUtil::suffixFromCharge(modifiers.color);

    if (modifiers.boundary == CircuitBoundaryType::Invalid && componentName == "CircuitBoundary") // this is when the boundary is rendered as part of a UI button
        baseTextureName += "Open";
    if (modifiers.boundary == CircuitBoundaryType::Open)
        baseTextureName += "Open";
    if (modifiers.boundary == CircuitBoundaryType::Closed)
        baseTextureName += "Closed";

    const string fullTextureName = baseTextureName + (getStoredChargeLayer ? "StoredCharge" : "");

    if (textures.count(fullTextureName) == 0)
    {
        const string filename = params().assetDir + "textures/" + baseTextureName + ".png";
        MLIB_ASSERT_STR(util::fileExists(filename), "File not found");
        Bitmap bmp = LodePNG::load(filename);

        const string alphaFilename = util::replace(filename, ".png", "Alpha.png");

        bool isHalfAlphaTexture = (componentName == "Faded" || componentName == "Selector" || componentName == "PuzzleSelector" ||
                                   componentName == "SquareBlocked" || componentName == "SquareOpen" || componentName == "CircuitSelector" ||
                                   componentName == "Tooltip");

        if (isHalfAlphaTexture)
        {
            int alpha = 105;
            if (componentName == "Faded") alpha = 150;
            if (componentName == "Tooltip") alpha = 180;
            for (const auto &p : bmp)
                p.value.a = alpha;
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
               // else
                //    p.value.a = 255;
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
                //else
                //    p.value.a = 255;

                if (p.value == Colors::Magenta())
                {
                    p.value = RGBColor(0, 0, 0, 255);
                }
            }
        }

		if(debugCalls) cout << "Creating texture for " << componentName << " (" << bmp.dimX() << "," << bmp.dimY() << ")" << endl;
        Texture *t = new Texture(renderer, bmp);
        if (debugCalls) cout << "Done creating texture" << endl;

        //
        // configure alpha mode
        //

        bool isAlphaTexture = true;

        if (componentName == "Background") isAlphaTexture = false;

        checkGLError();
        if (isAlphaTexture)
        {
            SDL_SetTextureBlendMode(t->SDL(), SDL_BLENDMODE_BLEND);
        }
        else
        {
            SDL_SetTextureBlendMode(t->SDL(), SDL_BLENDMODE_NONE);
        }
        checkGLError();

        textures[fullTextureName] = t;
    }
    return *textures.at(fullTextureName);
}
