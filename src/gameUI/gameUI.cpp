
#include "main.h"

namespace depthLayers
{
    const float selection = 0.9f;
    const float charge = 0.01f;
    const float font = 0.02f;
    const float hoverComponent = 0.1f;
    const float hoverComponentGrid = 0.05f;
    const float background = 1.0f;
    const float spokes = 0.95f;
    const float component = 0.9f;
    const float miniCircuitOffset = 0.2f;
}

void GameUI::init()
{
    selectedGameLocation.boardPos = constants::invalidCoord;
    backgroundDirty = true;
    selectedMenuComponent = nullptr;
}

Texture& GameUI::getFontTexture(const string &text, float height, RGBColor color)
{
    if (textCache.count(text) == 0)
    {
        Texture *t = new Texture(app.renderer.font(), text, color);
        textCache[text] = t;
    }
    return *textCache[text];
}

void GameUI::render(Texture &tex, const rect2f &destinationRect, float depth, const vec4f &color, const CoordinateFrame &frame)
{
    app.renderer.render(tex, frame.toContainer(destinationRect), depth, color);
}

void GameUI::render(Texture &tex, const rect2f &destinationRect, float depth, float rotation, const vec4f &color, const CoordinateFrame &frame)
{
    app.renderer.render(tex, frame.toContainer(destinationRect), depth, rotation, color);
}

void GameUI::render(Texture &tex, const rect2f &destinationRect, float depth, const vec4f &color)
{
    render(tex, destinationRect, depth, color, coordinateFrame);
}

void GameUI::render(Texture &tex, const rect2f &destinationRect, float depth, float rotation, const vec4f &color)
{
    render(tex, destinationRect, depth, rotation, color, coordinateFrame);
}

void GameUI::render(const UIRenderObject &o)
{
    const vec4f color = (o.dynamicComponent != nullptr && o.type == UIRenderStoredCharge) ? o.dynamicComponent->modifiers.storedChargeColor : o.color;
    Texture &t = (o.dynamicComponent != nullptr && o.type == UIRenderStandard) ? database().getTexture(app.renderer, o.dynamicComponent->info->name, o.dynamicComponent->modifiers) : *o.tex;

    if (o.rotation == 0.0f)
        render(t, o.rect, o.depth, color);
    else
        render(t, o.rect, o.depth, o.rotation, color);
}

void GameUI::renderText(Texture &tex, const vec2f &start, const float height, const vec4f &color)
{
    const float aspect = (float)tex.bmp().dimX() / (float)tex.bmp().dimY();
    const float width = aspect * height;
    const rect2f rect(start, vec2f(start.x + width, start.y + height));
    app.renderer.render(tex, coordinateFrame.toContainer(rect), depthLayers::font, color);
}

void GameUI::keyDown(SDL_Keycode key)
{
	const Uint8 *keys = SDL_GetKeyboardState(NULL);

    if (key == SDLK_ESCAPE)
    {
        backgroundDirty = true;
        selectedMenuComponent = nullptr;
        selectedGameLocation.boardPos = constants::invalidCoord;
    }

    if (key == SDLK_RETURN)
    {
        app.controller.designActionTaken = false;
        app.controller.puzzleMode = ModeExecuting;
        app.state.resetPuzzle();
    }

    auto loadPuzzle = [&]()
    {
        app.controller.loadPuzzle(params().assetDir + "../legacy/levelsOld/" + app.puzzles.puzzleList[app.controller.currentPuzzleIndex].name);
        backgroundDirty = true;
        selectedGameLocation.boardPos = constants::invalidCoord;
    };

    if (key == SDLK_0)
    {
        ParameterFile parameterFile("../assets/parameters.txt");
        g_gameParams.load(parameterFile);
    }

    if (key == SDLK_LEFT)
    {
        app.controller.currentPuzzleIndex = math::mod(app.controller.currentPuzzleIndex - 1, app.puzzles.puzzleList.size());
        loadPuzzle();
		app.backBuffer.reset(app.state);
    }
    if (key == SDLK_RIGHT)
    {
        app.controller.currentPuzzleIndex = math::mod(app.controller.currentPuzzleIndex + 1, app.puzzles.puzzleList.size());
        loadPuzzle();
		app.backBuffer.reset(app.state);
    }
	if (key == SDLK_z)
	{
		// undo and redo
		if (keys[SDL_SCANCODE_LCTRL] || keys[SDL_SCANCODE_RCTRL]){
			if (keys[SDL_SCANCODE_LSHIFT] || keys[SDL_SCANCODE_LSHIFT])
				app.backBuffer.forward(app.state);
			else 
				app.backBuffer.back(app.state);
            backgroundDirty = true;
		}
	}

	if (key == SDLK_c)
	{
		if (keys[SDL_SCANCODE_LCTRL] || keys[SDL_SCANCODE_RCTRL]){
		//TODO COPY
		}
	}

	if (key == SDLK_v)
	{
		if (keys[SDL_SCANCODE_LCTRL] || keys[SDL_SCANCODE_RCTRL]){
			//TODO PASTE
		}
	}
}

void GameUI::removeHoverComponent()
{
    const GameLocation location = hoverLocation(false);
    
    Component *c = app.state.getComponent(location);
    if (c == nullptr || (app.controller.editorMode == ModePlayLevel && c->modifiers.puzzleType == ComponentPuzzlePiece) || c->info->name == "CircuitBoundary")
        return;

    backgroundDirty = true;
    app.controller.designActionTaken = true;
    app.state.removeComponent(c);
	app.backBuffer.save(app.state);
}

void GameUI::mouseDown(Uint8 mouseButton, int x, int y)
{
    CoordinateFrame windowFrame = app.renderer.getWindowCoordinateFrame();
    mouseHoverCoord = vec2i(windowFrame.fromContainer(vec2f((float)x, (float)y)));
    x = mouseHoverCoord.x;
    y = mouseHoverCoord.y;

    //
    // TODO: this is more aggressive than it needs to be, but it probably doesn't matter.
    //
    backgroundDirty = true;

    if (mouseButton == SDL_BUTTON_RIGHT)
    {
        if (selectedMenuComponent != nullptr)
        {
            selectedMenuComponent = nullptr;
        }
        else
        {
            removeHoverComponent();
        }
    }

    if (mouseButton == SDL_BUTTON_LEFT)
    {
        if (selectedMenuComponent == nullptr)
        {
            GameLocation hover = hoverLocation(false);
            if (hover.valid())
                selectedGameLocation = hover;
        }
        else
        {
            addHoverComponent();
        }
    }

    const GameButton *hitButton = nullptr;
    for (const auto &button : buttons)
    {
        const rect2f screenRect = GameUtil::canonicalToWindow(GameUtil::getCanonicalSize(), button.canonicalRect);
        if (screenRect.intersects(vec2f((float)x, (float)y)))
        {
            hitButton = &button;
        }
    }
    if (hitButton == nullptr)
        return;
    const GameButton &button = *hitButton;

    //buttons.push_back(GameButton("TotalCharge", vec2i(0, 0), ButtonComponentAttribute, "Total charges: " + to_string(selectedGameComponent->intrinsics.totalCharges)));
    //buttons.push_back(GameButton("FirstEmission", vec2i(0, 1), ButtonComponentAttribute, "First charge at " + to_string(selectedGameComponent->intrinsics.secondsBeforeFirstEmission) + "s"));
    //buttons.push_back(GameButton("EmissionFrequency", vec2i(0, 2), ButtonComponentAttribute, "New charge every " + to_string(selectedGameComponent->intrinsics.secondsPerEmission) + "s"));

    int delta = mouseButton == SDL_BUTTON_LEFT ? -1 : 1;
    Component *gameComponent = app.state.getComponent(selectedGameLocation);
    if (gameComponent != nullptr && button.type == ButtonComponentAttribute && app.controller.editorMode == ModeEditLevel)
    {
        if (button.name == "TotalCharge")
        {
            gameComponent->intrinsics.totalCharges = math::mod(gameComponent->intrinsics.totalCharges + delta, 0xFFFFFF);
            app.controller.recordDesignAction();
        }
        if (button.name == "FirstEmission")
        {
            gameComponent->intrinsics.secondsBeforeFirstEmission = math::clamp(gameComponent->intrinsics.secondsBeforeFirstEmission + delta, 0, 1000);
            app.controller.recordDesignAction();
        }
        if (button.name == "EmissionFrequency")
        {
            gameComponent->intrinsics.secondsPerEmission = math::clamp(gameComponent->intrinsics.secondsPerEmission + delta, 1, 1000);
            app.controller.recordDesignAction();
        }
    }
    
    if (mouseButton == SDL_BUTTON_RIGHT && app.controller.editorMode == ModeEditLevel)
    {
        if (button.type == ButtonComponent)
        {
            const bool buildable = app.state.buildableComponents.canBuild(button.name, button.modifiers);
            const auto &info = database().getComponent(button.name);
            if (database().getComponent(button.name).colorUpgrades && button.modifiers.color != ChargeGray)
            {
                ChargeType start = info.name == "FilteredAmplifier" ? ChargeOrange : ChargeRed;
                for (int charge = (int)start; charge <= (int)ChargeBlue; charge++)
                    app.state.buildableComponents.setBuild(button.name, ComponentModifiers((ChargeType)charge), !buildable);
            }
            else
            {
                app.state.buildableComponents.setBuild(button.name, button.modifiers, !buildable);
            }
        }
        if (button.type == ButtonChargeColor)
        {
            app.state.buildableComponents.setBuild(button.name, button.modifiers, !app.state.buildableComponents.canBuild(button.name, button.modifiers));
        }
        if (button.type == ButtonWireSpeed)
        {
            app.state.buildableComponents.setBuild(button.name, button.modifiers, !app.state.buildableComponents.canBuild(button.name, button.modifiers));
        }
    }

    if (mouseButton == SDL_BUTTON_LEFT)
    {
        if (button.type == ButtonComponent)
        {
            selectedMenuComponent = button.component;
            selectedMenuComponentColor = button.modifiers.color;
        }
        if (gameComponent != nullptr && !(app.controller.editorMode == ModePlayLevel && gameComponent->modifiers.puzzleType == ComponentPuzzlePiece))
        {
            if (button.type == ButtonChargeColor)
            {
                gameComponent->modifiers.color = button.modifiers.color;
                app.controller.recordDesignAction();
            }
            if (button.type == ButtonChargePreference)
            {
                gameComponent->modifiers.chargePreference = button.modifiers.chargePreference;
                app.controller.recordDesignAction();
            }
            if (button.type == ButtonWireSpeed)
            {
                gameComponent->modifiers.speed = button.modifiers.speed;
                app.controller.recordDesignAction();
            }
            if (button.type == ButtonCircuitBoundary)
            {
                gameComponent->modifiers.boundary = button.modifiers.boundary;
                app.controller.recordDesignAction();
            }
        }
        if (button.name == "Start")
        {
            if (app.controller.puzzleMode == ModeExecuting)
            {
                app.controller.speed = Speed1x;
            }
            else
            {
                app.controller.designActionTaken = false;
                app.controller.puzzleMode = ModeExecuting;
                app.state.resetPuzzle();
            }
        }
        if (button.name == "Stop")
        {
            app.controller.puzzleMode = ModeDesign;
            app.state.resetPuzzle();
        }
        if (button.name == "Pause")
        {
            app.controller.speed = Speed0x;
        }
        if (button.name == "ModePuzzle")
        {
            app.controller.editorMode = ModePlayLevel;
        }
        if (button.name == "ModeLevelEditor")
        {
            app.controller.editorMode = ModeEditLevel;
        }
        if (button.name == "Save")
        {
            string filename = FileDialog::showSave();
            if (filename.size() > 0)
            {
                if (util::endsWith(filename, ".pzl"))
                    filename = util::remove(filename, ".pzl");
                filename = util::replace(filename, '.', ' ');
                filename = util::replace(filename, ',', ' ');
                filename += ".pzl";
                app.state.savePuzzle(filename);
            }
        }
        if (button.name == "Load")
        {
            const string filename = FileDialog::showOpen();
			if (filename.size() > 0)
			{
				app.state.loadPuzzle(filename);
				app.backBuffer.reset(app.state);
			}
        }

        for (int speed = (int)Speed1x; speed <= (int)Speed50x; speed++)
            if (button.name == buttonNameFromSpeed((GameSpeed)speed))
            {
                app.controller.speed = (GameSpeed)speed;
            }
    }
}

void GameUI::mouseMove(Uint32 buttonState, int x, int y)
{
    CoordinateFrame windowFrame = app.renderer.getWindowCoordinateFrame();
    mouseHoverCoord = vec2i(windowFrame.fromContainer(vec2f((float)x, (float)y)));
    x = mouseHoverCoord.x;
    y = mouseHoverCoord.y;

    if (buttonState & SDL_BUTTON_LMASK)
    {
        addHoverComponent();
    }
    if (buttonState & SDL_BUTTON_RMASK)
    {
        removeHoverComponent();
    }

}

void GameUI::addHoverComponent()
{
    if (selectedMenuComponent == nullptr)
        return;

    const GameLocation location = hoverLocation(true);
    if (location.boardPos == constants::invalidCoord)
        return;

    if (location.inCircuit())
    {
        const bool cornerX = location.circuitPos.x == 0 || location.circuitPos.x == constants::circuitBoardSize - 2;
        const bool cornerY = location.circuitPos.y == 0 || location.circuitPos.y == constants::circuitBoardSize - 2;

        if (activeCircuit() != nullptr &&
            activeCircuit()->circuitBoard->coordValidForNewComponent(location.circuitPos) &&
            !(cornerX && cornerY) &&
            selectedMenuComponent->name != "Circuit" &&
            selectedMenuComponent->name != "Blocker")
        {
            Component *newComponent = new Component(selectedMenuComponent->name, selectedMenuComponentColor, location);
            app.state.addNewComponent(newComponent);
            app.controller.designActionTaken = true;
            backgroundDirty = true;

			app.backBuffer.save(app.state); //saves to the backwards/forwards buffer
        }
    }
    else if (app.state.board.coordValidForNewComponent(location.boardPos))
    {
        Component *newComponent = new Component(selectedMenuComponent->name, selectedMenuComponentColor, location);
        app.state.addNewComponent(newComponent);
        app.controller.designActionTaken = true;
        backgroundDirty = true;

		app.backBuffer.save(app.state); //saves to the backwards/forwards buffer
    }
}

void GameUI::render()
{
    canonicalDims = GameUtil::getCanonicalSize();
    coordinateFrame = app.renderer.getWindowCoordinateFrame();

    if (backgroundDirty)
    {
        backgroundDirty = false;

        updateBackgroundObjects();

        //
        // resize the background if needed
        //
        //const vec2i windowSize = app.renderer.getWindowSize();
        //if (vec2i((int)background.bmp().width(), (int)background.bmp().height()) != windowSize)
        //    background.loadRenderTarget(windowSize.x, windowSize.y);

        //app.renderer.setDefaultRenderTarget();
        //render(background, rect2f(vec2f(0.0f, 0.0f), background.bmp().dims()));
    }

    //
    // TODO: render to off-screen buffer instead of re-rendering background every time
    //
    for (const UIRenderObject &o : backgroundObjects)
    {
        render(o);
    }

    for (const auto &button : buttons)
    {
        renderButtonForeground(button, false);
    }

    renderHoverComponent();

    for (const auto &charge : app.state.charges)
    {
        renderCharge(charge);
    }

    for (const auto &charge : app.state.explodingCharges)
    {
        renderExplodingCharge(charge);
    }

    renderText(getFontTexture(app.state.name, 20.0f, Colors::Black()), vec2f(1.0f, 1.0f), 20.0f);
}

GameLocation GameUI::hoverLocation(bool constructionOffset) const
{
    const vec2f boardCoordf = GameUtil::windowToBoard(canonicalDims, mouseHoverCoord);
    const vec2f circuitCoordf = GameUtil::windowToCircuit(canonicalDims, mouseHoverCoord);

    const vec2i boardCoordi = constructionOffset ?
        math::round(boardCoordf) - vec2i(1, 1) :
        math::floor(boardCoordf);

    const vec2i circuitCoordi = constructionOffset ?
        math::round(circuitCoordf) - vec2i(1, 1) :
        math::floor(circuitCoordf);

    const auto &cells = app.state.board.cells;
    if (cells.coordValid(boardCoordi))
        return GameLocation(boardCoordi);

    if (activeCircuit() != nullptr && activeCircuit()->circuitBoard->cells.coordValid(circuitCoordi))
        return GameLocation(activeCircuit()->location.boardPos, circuitCoordi);


    return GameLocation(constants::invalidCoord);
}
void GameUI::renderHoverComponent()
{
    if (selectedMenuComponent == nullptr)
        return;

    GameLocation location = hoverLocation(true);
    location.boardPos.x = std::min(location.boardPos.x, (int)app.state.board.cells.dimX() - 2);
    location.boardPos.y = std::min(location.boardPos.y, (int)app.state.board.cells.dimY() - 2);

    if (!location.valid())
        return;

    if (location.inCircuit() && activeCircuit() == nullptr)
        return;

    if (location.inCircuit())
    {
        location.circuitPos.x = std::min(location.circuitPos.x, constants::circuitBoardSize - 2);
        location.circuitPos.y = std::min(location.circuitPos.y, constants::circuitBoardSize - 2);
    }

    const rect2f screenRect = GameUtil::locationToWindowRect(canonicalDims, location, 2);

    renderLocalizedComponent(selectedMenuComponent->name, nullptr, screenRect, depthLayers::hoverComponent, IconState(ComponentModifiers(selectedMenuComponentColor), false, false));

    const vec2i coordBase = location.inCircuit() ? location.circuitPos : location.boardPos;
    const Board &board = location.inCircuit() ? *activeCircuit()->circuitBoard : app.state.board;

    for (int xOffset = 0; xOffset <= 1; xOffset++)
        for (int yOffset = 0; yOffset <= 1; yOffset++)
        {
            const vec2i coord = coordBase + vec2i(xOffset, yOffset);
            if (board.cells.coordValid(coord))
            {
                const BoardCell &cell = board.cells(coord);
                Texture *tex = (cell.c != nullptr || cell.blocked) ? database().squareBlocked : database().squareOpen;
                const rect2f rect = location.inCircuit() ?
                    GameUtil::circuitToWindowRect(canonicalDims, coord, 1) :
                    GameUtil::boardToWindowRect(canonicalDims, coord, 1);
                render(*tex, rect, depthLayers::hoverComponentGrid);
            }
        }
}

void GameUI::updateButtonList()
{
    buttons.clear();

    //
    // Add all buildable components
    //
    for (auto &p : database().components)
    {
        const ComponentInfo &info = *p.second;
        if (info.menuCoordinate.x != -1)
        {
            const ComponentInfo *realInfo = &info;
            if (util::endsWith(info.name, "GrayProxy"))
            {
                realInfo = &database().getComponent(util::remove(info.name, "GrayProxy"));
            }
            if (app.controller.editorMode == ModeEditLevel || app.state.buildableComponents.canBuild(realInfo->name, ComponentModifiers(info)))
                buttons.push_back(GameButton(realInfo->name, info.menuCoordinate, ButtonType::ButtonComponent, ComponentModifiers(info)));
        }
    }

    //
    // Add color, delay, preference, and boundary buttons
    //
    Component *selectedGameComponent = app.state.getComponent(selectedGameLocation);
    if (selectedGameComponent != nullptr && !(app.controller.puzzleMode == ModePlayLevel && selectedGameComponent->modifiers.puzzleType == ComponentPuzzlePiece))
    {
        const ComponentInfo &info = *selectedGameComponent->info;

        if (selectedGameComponent->info->name != "Circuit" && selectedGameComponent->info->name != "Blocker" && selectedGameComponent->info->name != "PowerSource")
        {
            for (int chargePreference = 0; chargePreference <= 4; chargePreference++)
            {
                buttons.push_back(GameButton(info.name, vec2i(chargePreference, 3), ButtonType::ButtonChargePreference, ComponentModifiers(selectedGameComponent->modifiers.color, chargePreference)));
            }
        }

        if (info.name == "Wire")
        {
            for (int speed = 0; speed <= 4; speed++)
            {
                ComponentModifiers modifier(ChargeNone, 2, (WireSpeedType)speed);
                if (app.controller.editorMode == ModeEditLevel || app.state.buildableComponents.canBuild(info.name, modifier))
                    buttons.push_back(GameButton("Wire", vec2i((int)speed, 4), ButtonType::ButtonWireSpeed, modifier));
            }
        }
        else if (info.name == "CircuitBoundary")
        {
            buttons.push_back(GameButton("CircuitBoundary", vec2i(0, 4), ButtonType::ButtonCircuitBoundary, ComponentModifiers(ChargeNone, 2, WireStandard, CircuitBoundaryOpen)));
            buttons.push_back(GameButton("CircuitBoundary", vec2i(1, 4), ButtonType::ButtonCircuitBoundary, ComponentModifiers(ChargeNone, 2, WireStandard, CircuitBoundaryClosed)));
        }
        else
        {
            vector<ChargeType> chargeLevels;
            if (info.colorUpgrades && selectedGameComponent->modifiers.color != ChargeGray)
            {
                ChargeType start = info.name == "FilteredAmplifier" ? ChargeOrange : ChargeRed;
                for (int charge = (int)start; charge <= (int)ChargeBlue; charge++)
                    chargeLevels.push_back((ChargeType)charge);
            }

            int chargeIndex = 0;
            for (ChargeType charge : chargeLevels)
            {
                if (app.controller.editorMode == ModeEditLevel || app.state.buildableComponents.canBuild(info.name, ComponentModifiers(charge)))
                    buttons.push_back(GameButton(info.name, vec2i(chargeIndex, 4), ButtonType::ButtonChargeColor, ComponentModifiers(charge)));
                chargeIndex++;
            }
        }
    }

    //
    // Add puzzle control buttons
    //
    if (app.controller.puzzleMode != ModeExecuting || (app.controller.puzzleMode == ModeExecuting && app.controller.speed == Speed0x))
        buttons.push_back(GameButton("Start", vec2i(0, 0), ButtonType::ButtonPuzzleControl, ComponentModifiers()));
    else if (app.controller.puzzleMode == ModeExecuting)
        buttons.push_back(GameButton("Pause", vec2i(0, 0), ButtonType::ButtonPuzzleControl, ComponentModifiers()));
    buttons.push_back(GameButton("Stop", vec2i(1, 0), ButtonType::ButtonPuzzleControl, ComponentModifiers()));
    buttons.push_back(GameButton("Save", vec2i(3, 0), ButtonType::ButtonPuzzleControl, ComponentModifiers()));
    buttons.push_back(GameButton("Load", vec2i(4, 0), ButtonType::ButtonPuzzleControl, ComponentModifiers()));
    buttons.push_back(GameButton("ModePuzzle", vec2i(6, 0), ButtonType::ButtonPuzzleControl, ComponentModifiers()));
    buttons.push_back(GameButton("ModeLevelEditor", vec2i(7, 0), ButtonType::ButtonPuzzleControl, ComponentModifiers()));

    for (int speed = (int)Speed1x; speed <= (int)Speed50x; speed++)
        buttons.push_back(GameButton(buttonNameFromSpeed((GameSpeed)speed), vec2i(speed - 1, 1), ButtonType::ButtonPuzzleControl, ComponentModifiers()));

    //
    // Add power source indicators
    //
    if (selectedGameComponent != nullptr && selectedGameComponent->info->name == "PowerSource")
    {
        buttons.push_back(GameButton("TotalCharge", vec2i(0, 0), ButtonComponentAttribute, "Total charges: " + to_string(selectedGameComponent->intrinsics.totalCharges)));
        buttons.push_back(GameButton("FirstEmission", vec2i(0, 1), ButtonComponentAttribute, "First charge at " + to_string(selectedGameComponent->intrinsics.secondsBeforeFirstEmission) + "s"));
        buttons.push_back(GameButton("EmissionFrequency", vec2i(0, 2), ButtonComponentAttribute, "New charge every " + to_string(selectedGameComponent->intrinsics.secondsPerEmission) + "s"));
    }
}

void GameUI::updateBackgroundObjects()
{
    backgroundObjects.clear();

    updateButtonList();

    Component *gameComponent = app.state.getComponent(selectedGameLocation);

    //app.renderer.setRenderTarget(background);

    addBackgroundObject(database().getTexture(app.renderer, "Background"), rect2f(vec2f(0.0f, 0.0f), canonicalDims), depthLayers::background);

    if (activeCircuit() != nullptr)
    {
        const rect2f rect(params().circuitCanonicalStart - vec2f(params().circuitBackgroundCanonicalBoundarySize), params().circuitCanonicalStart + (float)constants::circuitBoardSize * params().canonicalCellSize + vec2f(params().circuitBackgroundCanonicalBoundarySize));
        addBackgroundObject(database().getTexture(app.renderer, "CircuitBackground"), GameUtil::canonicalToWindow(canonicalDims, rect), depthLayers::background);
    }

    renderBuildingGrid();

    renderComponents();

    for (auto &component : app.state.components)
    {
        renderSpokes(*component);
    }

    for (auto &button : buttons)
    {
        bool selected = false;
        selected |= (button.type == ButtonComponent && selectedMenuComponent == button.component && selectedMenuComponentColor == button.modifiers.color);
        if (gameComponent != nullptr)
        {
            selected |= (button.type == ButtonChargePreference && gameComponent->modifiers.chargePreference == button.modifiers.chargePreference);
            selected |= (button.type == ButtonChargeColor && gameComponent->modifiers.color == button.modifiers.color);
            selected |= (button.type == ButtonWireSpeed && gameComponent->modifiers.speed == button.modifiers.speed);
            selected |= (button.type == ButtonCircuitBoundary && gameComponent->modifiers.boundary == button.modifiers.boundary);
        }
        if (button.type == ButtonPuzzleControl)
        {
            selected |= (button.name == buttonNameFromSpeed(app.controller.speed));
            selected |= (button.name == "ModePuzzle" && app.controller.editorMode == ModePlayLevel);
            selected |= (button.name == "ModeLevelEditor" && app.controller.editorMode == ModeEditLevel);
        }
        renderButtonBackground(button, selected);
    }

    stable_sort(backgroundObjects.begin(), backgroundObjects.end());
}

void GameUI::renderBuildingGrid()
{
    Texture &border = database().getTexture(app.renderer, "Border");
    const auto &cells = app.state.board.cells;

    //
    // the extent is expanded by 1 to draw the border tiles
    //
    for (int x = -1; x <= (int)cells.dimX(); x++)
    {
        for (int y = -1; y <= (int)cells.dimY(); y++)
        {
            const rect2f rectangle = rect2f(vec2f((float)x, (float)y), vec2f(x + 1.0f, y + 1.0f));
            const rect2f screenRect = params().boardInWindow.toContainer(rectangle);

            if (cells.coordValid(x, y) && !cells(x, y).blocked)
            {
                addBackgroundObject(border, screenRect, depthLayers::background);
            }
            else
            {
                //
                // this is legacy code that could be refactored at some point
                //
                bool leftBlocked = false;
                bool rightBlocked = false;
                bool topBlocked = false;
                bool bottomBlocked = false;
                bool topLeftBlocked = false;
                bool topRightBlocked = false;
                bool bottomLeftBlocked = false;
                bool bottomRightBlocked = false;

                const vec2i baseOffset = vec2i(x, y);
                for (UINT OffsetIndex = 0; OffsetIndex < 8; OffsetIndex++)
                {
                    vec2i offset = baseOffset;
                    if (OffsetIndex == 0) offset += vec2i(-1, 0);
                    if (OffsetIndex == 1) offset += vec2i(1, 0);
                    if (OffsetIndex == 2) offset += vec2i(0, -1);
                    if (OffsetIndex == 3) offset += vec2i(0, 1);
                    if (OffsetIndex == 4) offset += vec2i(-1, -1);
                    if (OffsetIndex == 5) offset += vec2i(1, -1);
                    if (OffsetIndex == 6) offset += vec2i(-1, 1);
                    if (OffsetIndex == 7) offset += vec2i(1, 1);

                    if (!cells.coordValid(offset) || cells(offset).blocked)
                    {
                        if (OffsetIndex == 0) leftBlocked = true;
                        if (OffsetIndex == 1) rightBlocked = true;
                        if (OffsetIndex == 2) topBlocked = true;
                        if (OffsetIndex == 3) bottomBlocked = true;
                        if (OffsetIndex == 4) topLeftBlocked = true;
                        if (OffsetIndex == 5) topRightBlocked = true;
                        if (OffsetIndex == 6) bottomLeftBlocked = true;
                        if (OffsetIndex == 7) bottomRightBlocked = true;
                    }
                }

                UINT boundaryIndex = 8;
                if (leftBlocked && !rightBlocked && topBlocked && !bottomBlocked) boundaryIndex = 0;
                else if (leftBlocked && !rightBlocked && !topBlocked && bottomBlocked) boundaryIndex = 1;
                else if (!leftBlocked && rightBlocked && topBlocked && !bottomBlocked) boundaryIndex = 2;
                else if (!leftBlocked && rightBlocked && !topBlocked && bottomBlocked) boundaryIndex = 3;
                else if (!leftBlocked && rightBlocked && topBlocked && bottomBlocked)
                {
                    boundaryIndex = 4;
                    if (!topRightBlocked) boundaryIndex = 13;
                    if (!bottomRightBlocked) boundaryIndex = 14;
                }
                else if (leftBlocked && !rightBlocked && topBlocked && bottomBlocked)
                {
                    boundaryIndex = 5;
                    if (!topLeftBlocked) boundaryIndex = 15;
                    if (!bottomLeftBlocked) boundaryIndex = 16;
                }
                else if (leftBlocked && rightBlocked && !topBlocked && bottomBlocked)
                {
                    boundaryIndex = 6;
                    if (!bottomLeftBlocked) boundaryIndex = 17;
                    if (!bottomRightBlocked) boundaryIndex = 18;
                }
                else if (leftBlocked && rightBlocked && topBlocked && !bottomBlocked)
                {
                    boundaryIndex = 7;
                    if (!topLeftBlocked) boundaryIndex = 19;
                    if (!topRightBlocked) boundaryIndex = 20;
                }
                else if (leftBlocked && rightBlocked && !topBlocked && !bottomBlocked)
                {
                    boundaryIndex = 21;
                }

                else if (!leftBlocked && !rightBlocked && topBlocked && bottomBlocked)
                {
                    boundaryIndex = 22;
                }


                if (boundaryIndex == 8)
                {
                    if (!topLeftBlocked) boundaryIndex = 9;
                    else if (!topRightBlocked) boundaryIndex = 10;
                    else if (!bottomLeftBlocked) boundaryIndex = 11;
                    else if (!bottomRightBlocked) boundaryIndex = 12;
                }

                addBackgroundObject(*database().boundaryTextures[boundaryIndex], screenRect, depthLayers::background);
            }
        }
    }

    Component *circuit = activeCircuit();
    if (circuit != nullptr)
    {
        for (const auto &cell : circuit->circuitBoard->cells)
        {
            const bool xBoundary = (cell.x == 0 || cell.x == 1 || cell.x == constants::circuitBoardSize - 1 || cell.x == constants::circuitBoardSize - 2);
            const bool yBoundary = (cell.y == 0 || cell.y == 1 || cell.y == constants::circuitBoardSize - 1 || cell.y == constants::circuitBoardSize - 2);
            const bool corner = xBoundary && yBoundary;
            const bool inactiveBoundary = (cell.value.c != nullptr && cell.value.c->inactiveBoundary());
            if (!corner && !inactiveBoundary) //if (!xBoundary && !yBoundary)
            {
                const rect2f screenRect = GameUtil::circuitToWindowRect(canonicalDims, vec2i(cell.x, cell.y), 1);
                addBackgroundObject(border, screenRect, depthLayers::background);
            }
        }
    }
}

void GameUI::renderButtonForeground(const GameButton &button, bool selected)
{
    if (button.type == ButtonComponentAttribute)
    {
        renderText(getFontTexture(button.text, 24.0f, Colors::Black()), button.canonicalRect.min(), (float)button.canonicalRect.extentY());
    }
}

void GameUI::renderButtonBackground(const GameButton &button, bool selected)
{
    if (button.type != ButtonComponentAttribute)
    {
        Texture &borderTex = database().getTexture(app.renderer, "Border");

        const rect2f screenRect = GameUtil::canonicalToWindow(GameUtil::getCanonicalSize(), button.canonicalRect);
        addBackgroundObject(borderTex, screenRect, depthLayers::background);

        renderLocalizedComponent(button.name, nullptr, screenRect, 0.0f, IconState(button.modifiers, selected));

        if (app.controller.editorMode == ModeEditLevel &&
            app.state.buildableComponents.canBuild(button.name, button.modifiers) &&
            (button.type == ButtonComponent || button.type == ButtonChargeColor || button.type == ButtonWireSpeed))
        {
            Texture &constructionTex = database().getTexture(app.renderer, "Construction");
            const rect2f screenRect = GameUtil::canonicalToWindow(GameUtil::getCanonicalSize(), button.canonicalRect);
            addBackgroundObject(constructionTex, screenRect, depthLayers::component);
        }
    }
}

void GameUI::renderLocalizedComponent(const string &name, const Component *dynamicComponent, const rect2f &screenRect, float depthOffset, const IconState &icon)
{
    /*if (!isButton && name == "Blocker" &&
        (selectedMenuComponent == nullptr || selectedMenuComponent->name != "Blocker"))
        //(app.state.getComponent(selectedGameLocation) == nullptr || app.state.getComponent(selectedGameLocation)->info->name != "Blocker"))
        return;*/

    Texture &baseTex = database().getTexture(app.renderer, "WireBase");
    Texture &componentTex = database().getTexture(app.renderer, name, icon.modifiers);
    Texture &preferenceTex = *database().preferenceTextures[icon.modifiers.chargePreference];

    auto record = [&](Texture &tex, const rect2f &rect, float depth, UIRenderType type, const vec4f &color, const Component *component) {
        depth -= depthOffset;
        if (icon.background)
            addBackgroundObject(tex, rect, depth, type, color, component);
        else
            render(tex, rect, depth, color);
    };

    record(preferenceTex, screenRect, 1.0f, UIRenderStandard, Colors::White(), nullptr);

    if (name != "Blocker" && icon.modifiers.boundary != CircuitBoundaryClosed)
        record(baseTex, screenRect, 1.0f, UIRenderStandard, Colors::White(), nullptr);

    record(componentTex, screenRect, depthLayers::component, UIRenderStandard, Colors::White(), dynamicComponent);

    if (database().hasComponent(name) && database().getComponent(name).hasStoredChargeLayer)
    {
        Texture &chargeLayerTex = database().getTexture(app.renderer, name, icon.modifiers, true);
        record(chargeLayerTex, screenRect, depthLayers::component, UIRenderStoredCharge, GameUtil::chargeColor(ChargeGray), dynamicComponent);
    }

    if (icon.selected)
    {
        bool usePuzzleSelector = (dynamicComponent != nullptr && dynamicComponent->modifiers.puzzleType == ComponentPuzzlePiece && app.controller.editorMode == ModePlayLevel);

        Texture &selectionTex = usePuzzleSelector ? database().getTexture(app.renderer, "PuzzleSelector") : database().getTexture(app.renderer, "Selector");
        record(selectionTex, screenRect, depthLayers::selection, UIRenderStandard, Colors::White(), nullptr);
    }
}

void GameUI::renderComponent(const Component &component)
{
    Component *selectedGameComponent = app.state.getComponent(selectedGameLocation);
    const bool selected = (selectedGameComponent != nullptr && component.location == selectedGameComponent->location);

    if (component.info->name == "Blocker" &&
        (selectedMenuComponent == nullptr || selectedMenuComponent->name != "Blocker"))
        return;

    if (component.location.inCircuit())
    {
        // if the component is in the active circuit, render it in the selected circuit area
        if (activeCircuit() != nullptr && component.location.boardPos == activeCircuit()->location.boardPos)
        {
            if (!component.inactiveBoundary())
            {
                const rect2f screenRect = GameUtil::circuitToWindowRect(canonicalDims, component.location.circuitPos, 2);
                renderLocalizedComponent(component.info->name, &component, screenRect, 0.0f, IconState(component.modifiers, selected));
            }
        }
        // regardless, we'll need to render it in the main grid, but we'll wait until later

    }
    else
    {
        const rect2f screenRect = GameUtil::boardToWindowRect(canonicalDims, component.location.boardPos, 2);
        renderLocalizedComponent(component.info->name, &component, screenRect, 0.0f, IconState(component.modifiers, selected));
    }
}

void GameUI::renderCircuitComponent(const Component &component)
{
    if (!component.location.inCircuit() || component.inactiveBoundary() || component.modifiers.boundary == CircuitBoundaryClosed)
        return;
    const CoordinateFrame frame = CoordinateFrame(component.location.boardPos, component.location.boardPos + vec2f(2.0f, 2.0f), vec2i(constants::circuitBoardSize, constants::circuitBoardSize));
    const rect2f circuitRect = rect2f(component.location.circuitPos, component.location.circuitPos + 2);
    const rect2f screenRect = params().boardInWindow.toContainer(frame.toContainer(circuitRect));
    renderLocalizedComponent(component.info->name, &component, screenRect, depthLayers::miniCircuitOffset, IconState(component.modifiers, false));
}

void GameUI::renderSpokes(const Component &component)
{
    if (!component.hasSpokes())
        return;

    if (component.location.inCircuit())
    {
        renderSpokesCircuit(component);
        return;
    }

    const vec2f myScreenPos = GameUtil::boardToWindow(canonicalDims, component.location.boardPos + vec2i(1, 1));

    const auto &cells = app.state.board.cells;

    auto drawCircuitConnection = [&](const Component &a, const Component &b)
    {
        auto test = [&](const Component &circuit, const Component &board)
        {
            for (const Connection &connection : board.connections)
            {
                if (connection.component != nullptr && connection.component->location.boardPos == circuit.location.boardPos && connection.component->modifiers.boundary == CircuitBoundaryOpen)
                    return true;
            }
            return false;
        };
        if (a.info->name == "Circuit" && b.info->name == "Circuit")
            return false;
        else if (a.info->name != "Circuit" && b.info->name != "Circuit")
            return true;
        else if (a.info->name == "Circuit" && b.info->name != "Circuit")
            return test(a, b);
        if (a.info->name != "Circuit" && b.info->name == "Circuit")
            return test(b, a);
        return false;
    };

    for (int type = -1; type <= 1; type++)
    {
        for (int axis = 0; axis < 2; axis++)
        {
            vec2i offset;
            if (axis == 0) offset = vec2i(-2, type);
            if (axis == 1) offset = vec2i(type, -2);
            UINT connectorIndex = (type + 1) * 2 + axis;
            vec2i otherLocation = component.location.boardPos + offset;

            if (cells.coordValid(otherLocation) && cells(otherLocation).c != nullptr)
            {
                const Component &otherComponent = *cells(otherLocation).c;
                
                bool circuitTest = drawCircuitConnection(component, otherComponent);

                if (otherComponent.location.boardPos == otherLocation && otherComponent.hasSpokes() && circuitTest)
                {
                    const vec2f otherScreenPos = GameUtil::boardToWindow(canonicalDims, otherComponent.location.boardPos + vec2i(1, 1));

                    const vec2f middle = (myScreenPos + otherScreenPos) * 0.5f;
                    const vec2f diff = myScreenPos - otherScreenPos;
                    const float dist = diff.length();

                    const vec2f variance = constants::connectorDims[type + 1] * dist;

                    Texture &connectorTex = database().getTexture(app.renderer, "WireConnector" + std::to_string(connectorIndex));
                    //const float angle = 180.0f;
                    const float angle = math::radiansToDegrees(atan2f(diff.y, diff.x)) + 180.0f;
                    addBackgroundObject(connectorTex, rect2f(middle - variance, middle + variance), depthLayers::spokes, angle);
                }
            }
        }
    }
}

void GameUI::renderSpokesCircuit(const Component &component)
{
    if (activeCircuit() == nullptr ||
        activeCircuit()->location.boardPos != component.location.boardPos ||
        component.modifiers.boundary == CircuitBoundaryClosed)
        return;

    const vec2f myScreenPos = GameUtil::circuitToWindow(canonicalDims, component.location.circuitPos + vec2i(1, 1));

    const auto &cells = activeCircuit()->circuitBoard->cells;

    for (int type = -1; type <= 1; type++)
    {
        for (int axis = 0; axis < 2; axis++)
        {
            vec2i offset;
            if (axis == 0) offset = vec2i(-2, type);
            if (axis == 1) offset = vec2i(type, -2);
            UINT connectorIndex = (type + 1) * 2 + axis;
            vec2i otherLocation = component.location.circuitPos + offset;

            if (cells.coordValid(otherLocation) && cells(otherLocation).c != nullptr)
            {
                const Component &otherComponent = *cells(otherLocation).c;
                bool renderSpokes = true;
                renderSpokes &= otherComponent.location.circuitPos == otherLocation;
                renderSpokes &= otherComponent.modifiers.boundary != CircuitBoundaryClosed;
                renderSpokes &= otherComponent.hasSpokes();
                renderSpokes &= !(component.info->name == "CircuitBoundary" && otherComponent.info->name == "CircuitBoundary");
                if (renderSpokes)
                {
                    const vec2f otherScreenPos = GameUtil::circuitToWindow(canonicalDims, otherComponent.location.circuitPos + vec2i(1, 1));

                    const vec2f middle = (myScreenPos + otherScreenPos) * 0.5f;
                    const vec2f diff = myScreenPos - otherScreenPos;
                    const float dist = diff.length();

                    const vec2f variance = constants::connectorDims[type + 1] * dist;

                    Texture &connectorTex = database().getTexture(app.renderer, "WireConnector" + std::to_string(connectorIndex));
                    const float angle = math::radiansToDegrees(atan2f(diff.y, diff.x)) + 180.0f;
                    addBackgroundObject(connectorTex, rect2f(middle - variance, middle + variance), depthLayers::spokes, angle);
                }
            }
        }
    }
}

void GameUI::renderComponents()
{
    for (auto &component : app.state.components)
    {
        renderComponent(*component);
    }
    for (auto &component : app.state.components)
    {
        renderCircuitComponent(*component);
    }
}

void GameUI::renderCharge(const Charge &charge)
{
    renderChargeCircuit(charge);
    const pair<vec2f, float> screen = GameUtil::computeChargeScreenPos(charge.source, charge.destination, charge.interpolation(), charge.level, canonicalDims);
    const float angle = charge.randomRotationOffset + app.state.globalRotationOffset;
    const rect2f destinationRect(screen.first - vec2f(screen.second), screen.first + vec2f(screen.second));
    render(*database().chargeTextures[charge.level], destinationRect, depthLayers::charge, angle);
}

void GameUI::renderExplodingCharge(const ExplodingCharge &charge)
{
    renderExplodingChargeCircuit(charge);
    const pair<vec2f, float> screen = GameUtil::computeChargeScreenPos(charge.locationA, charge.locationB, charge.interpolation, charge.level, canonicalDims);
    const float angle = charge.baseRotationOffset + (app.state.stepCount - charge.birthTick) * constants::secondsPerStep * constants::chargeRotationsPerSecond * 360.0f * constants::explodingChargeRotationFactor;
    const float scale = screen.second * math::lerp(1.0f, 3.0f, charge.percentDone());
    const rect2f destinationRect(screen.first - vec2f(scale), screen.first + vec2f(scale));

    const vec4f color(1.0f, 1.0f, 1.0f, 1.0f - charge.percentDone() * charge.percentDone());

    render(*database().chargeTextures[charge.level], destinationRect, depthLayers::charge, angle, color);
}

void GameUI::renderChargeCircuit(const Charge &charge)
{
    if (!charge.source.inCircuit() || !charge.destination.inCircuit()) return;
    if (charge.source.boardPos != charge.destination.boardPos) return;
    if (activeCircuit() == nullptr || charge.source.boardPos != activeCircuit()->location.boardPos) return;

    const pair<vec2f, float> screen = GameUtil::computeChargeScreenPosCircuit(charge.source, charge.destination, charge.interpolation(), charge.level, canonicalDims);
    const float angle = charge.randomRotationOffset + app.state.globalRotationOffset;

    const rect2f destinationRect(screen.first - vec2f(screen.second), screen.first + vec2f(screen.second));
    render(*database().chargeTextures[charge.level], destinationRect, depthLayers::charge, angle);
}

void GameUI::renderExplodingChargeCircuit(const ExplodingCharge &charge)
{
    if (!charge.locationA.inCircuit() || !charge.locationB.inCircuit()) return;
    if (activeCircuit() == nullptr || charge.locationA.boardPos != activeCircuit()->location.boardPos) return;

    const pair<vec2f, float> screen = GameUtil::computeChargeScreenPosCircuit(charge.locationA, charge.locationB, charge.interpolation, charge.level, canonicalDims);
    const float angle = charge.baseRotationOffset + (app.state.stepCount - charge.birthTick) * constants::secondsPerStep * constants::chargeRotationsPerSecond * 360.0f * constants::explodingChargeRotationFactor;
    const float scale = screen.second * math::lerp(1.0f, 3.0f, charge.percentDone());
    const rect2f destinationRect(screen.first - vec2f(scale), screen.first + vec2f(scale));

    const vec4f color(1.0f, 1.0f, 1.0f, 1.0f - charge.percentDone() * charge.percentDone());

    render(*database().chargeTextures[charge.level], destinationRect, depthLayers::charge, angle, color);
}

Component* GameUI::activeCircuit() const
{
    Component *c = app.state.getComponent(GameLocation(selectedGameLocation.boardPos));
    if (c != nullptr && c->info->name == "Circuit")
    {
        return c;
    }
    return nullptr;
}
