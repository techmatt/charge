
#include "main.h"

namespace depthLayers
{
    const float selection = 0.9f;
    const float charge = 0.01f;
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

    levelName.drawText(app.renderer.font(), "test!", RGBColor(255, 128, 128));
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
    Texture &t = o.dynamicComponent == nullptr ? *o.tex :
        database().getTexture(app.renderer, o.dynamicComponent->info->name, o.dynamicComponent->modifiers);

    if (o.rotation == 0.0f)
        render(t, o.rect, o.depth);
    else
        render(t, o.rect, o.depth, o.rotation);
}

void GameUI::keyDown(SDL_Keycode key)
{
    if (key == SDLK_ESCAPE)
    {
        selectedMenuComponent = nullptr;
    }
    if (key == SDLK_LEFT)
    {
        app.puzzles.currentPuzzle = math::mod(app.puzzles.currentPuzzle - 1, app.puzzles.puzzleList.size());
        app.controller.loadPuzzle(params().assetDir + "../legacy/levelsOld/" + app.puzzles.puzzleList[app.puzzles.currentPuzzle].name);
        backgroundDirty = true;
        
    }
    if (key == SDLK_RIGHT)
    {
        app.puzzles.currentPuzzle = math::mod(app.puzzles.currentPuzzle + 1, app.puzzles.puzzleList.size());
        app.controller.loadPuzzle(params().assetDir + "../legacy/levelsOld/" + app.puzzles.puzzleList[app.puzzles.currentPuzzle].name);
        backgroundDirty = true;
    }
}

void GameUI::removeHoverComponent()
{
    const GameLocation location = hoverLocation(false);
    if (location.inCircuit())
    {
        if (activeCircuit() != nullptr && activeCircuit()->circuitBoard->cells.coordValid(location.circuitPos))
        {
            Component *circuitComponent = activeCircuit()->circuitBoard->cells(location.circuitPos).c;
            if (circuitComponent != nullptr && circuitComponent->info->name != "CircuitBoundary")
            {
                backgroundDirty = true;
                app.controller.designActionTaken = true;
                app.state.removeComponent(activeCircuit()->circuitBoard->cells(location.circuitPos).c);
            }
        }
    }
    else
    {
        if (!app.state.board.cells.coordValid(location.boardPos))
            return;
        
        Component *c = app.state.board.cells(location.boardPos).c;
        if (c == nullptr || c->modifiers.puzzleType == ComponentPuzzlePiece)
            return;

        backgroundDirty = true;
        app.controller.designActionTaken = true;
        app.state.removeComponent(app.state.board.cells(location.boardPos).c);
    }
}

void GameUI::mouseDown(Uint8 button, int x, int y)
{
	CoordinateFrame windowFrame = app.renderer.getWindowCoordinateFrame();
    mouseHoverCoord = vec2i(windowFrame.fromContainer(vec2f((float) x,(float) y)));
	x = mouseHoverCoord.x;
	y = mouseHoverCoord.y;

    //
    // TODO: this is more aggressive than it needs to be, but it probably doesn't matter.
    //
    backgroundDirty = true;

    if (button == SDL_BUTTON_RIGHT)
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

    if (button == SDL_BUTTON_LEFT)
    {
        for (const auto &button : buttons)
        {
            const rect2f screenRect = GameUtil::canonicalToWindow(GameUtil::getCanonicalSize(), button.canonicalRect);
            Component *gameComponent = app.state.getComponent(selectedGameLocation);
            if (screenRect.intersects(vec2f((float)x, (float)y)))
            {
                if (button.type == ButtonComponent)
                {
                    selectedMenuComponent = button.component;
                }
                if (button.type == ButtonChargeColor && gameComponent != nullptr)
                {
                    app.controller.designActionTaken = true;
                    gameComponent->modifiers.color = button.modifiers.color;
                }
                if (button.type == ButtonChargePreference && gameComponent != nullptr)
                {
                    app.controller.designActionTaken = true;
                    gameComponent->modifiers.chargePreference = button.modifiers.chargePreference;
                }
                if (button.type == ButtonWireSpeed && gameComponent != nullptr)
                {
                    app.controller.designActionTaken = true;
                    gameComponent->modifiers.speed = button.modifiers.speed;
                }
                if (button.type == ButtonCircuitBoundary && gameComponent != nullptr)
                {
                    app.controller.designActionTaken = true;
                    gameComponent->modifiers.boundary = button.modifiers.boundary;
                }
                if (button.name == "Start")
                {
                    app.controller.designActionTaken = false;
                    app.controller.puzzleMode = ModeExecuting;
                }
                if (button.name == "Stop")
                {
                    app.controller.puzzleMode = ModeDesign;
                    app.state.resetPuzzle();
                }
                if (button.name == "Pause")
                {
                    app.controller.puzzleMode = ModePaused;
                }
                return;
            }
        }

        if (selectedMenuComponent == nullptr)
        {
            selectedGameLocation = hoverLocation(false);
        }
        else
        {
            addHoverComponent();
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
        if (activeCircuit() != nullptr &&
            activeCircuit()->circuitBoard->coordValidForNewComponent(location.circuitPos) &&
            selectedMenuComponent->name != "Circuit")
        {
            Component *newComponent = new Component(selectedMenuComponent->name, selectedMenuComponent->defaultPrimaryCharge(), location);
            app.state.addNewComponent(newComponent);
            app.controller.designActionTaken = true;
            backgroundDirty = true;
        }
    }
    else if (app.state.board.coordValidForNewComponent(location.boardPos))
    {
        Component *newComponent = new Component(selectedMenuComponent->name, selectedMenuComponent->defaultPrimaryCharge(), location);
        app.state.addNewComponent(newComponent);
        app.controller.designActionTaken = true;
        backgroundDirty = true;
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

    renderHoverComponent();

    for (const auto &charge : app.state.charges)
    {
        renderCharge(charge);
    }

    for (const auto &charge : app.state.explodingCharges)
    {
        renderExplodingCharge(charge);
    }

    render(levelName, rect2f(10.0f, 10.0f, 100.0f, 100.0f), 0.001f);
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

    if (app.state.board.cells.coordValid(boardCoordi))
        return GameLocation(boardCoordi);

    if (activeCircuit() != nullptr && activeCircuit()->circuitBoard->cells.coordValid(circuitCoordi))
        return GameLocation(activeCircuit()->location.boardPos, circuitCoordi);


    return GameLocation(constants::invalidCoord);
}
void GameUI::renderHoverComponent()
{
    if (selectedMenuComponent == nullptr)
        return;

    const GameLocation location = hoverLocation(true);
    if (!location.valid())
        return;

    if (location.inCircuit() && activeCircuit() == nullptr)
        return;

    const rect2f screenRect = GameUtil::locationToWindowRect(canonicalDims, location, 2);
    
    renderLocalizedComponent(selectedMenuComponent->name, nullptr, screenRect, ComponentModifiers(*selectedMenuComponent), false, false, false, depthLayers::hoverComponent);

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
            buttons.push_back(GameButton(info.name, info.menuCoordinate, ButtonType::ButtonComponent, ComponentModifiers(info)));
		}
	}

    //
    // Add color, delay, preference, and boundary buttons
    //
    Component *selectedGameComponent = app.state.getComponent(selectedGameLocation);
    if (selectedGameComponent != nullptr)
    {
        const ComponentInfo &info = *selectedGameComponent->info;

        vector<ChargeType> chargeLevels;
        if (info.colorUpgrades)
        {
            for (int charge = (int)ChargeRed; charge <= (int)ChargeBlue; charge++)
                chargeLevels.push_back((ChargeType)charge);
        }
        if (info.grayUpgrade)
            chargeLevels.push_back(ChargeGray);

        if (selectedGameComponent->info->name != "Circuit")
        {
            for (int chargePreference = 0; chargePreference <= 4; chargePreference++)
            {
                buttons.push_back(GameButton(info.name, vec2i(chargePreference, 3), ButtonType::ButtonChargePreference, ComponentModifiers(info.defaultPrimaryCharge(), info.defaultStoredCharge(), chargePreference)));
            }
        }

        if (info.name == "Wire")
        {
            for (int speed = 0; speed <= 4; speed++)
            {
                buttons.push_back(GameButton("Wire", vec2i((int)speed, 4), ButtonType::ButtonWireSpeed, ComponentModifiers(ChargeNone, ChargeNone, 2, (WireSpeedType)speed)));
            }
        }
        else if (info.name == "CircuitBoundary")
        {
            buttons.push_back(GameButton("CircuitBoundary", vec2i(0, 4), ButtonType::ButtonCircuitBoundary, ComponentModifiers(ChargeNone, ChargeNone, 2, WireStandard, CircuitBoundaryOpen)));
            buttons.push_back(GameButton("CircuitBoundary", vec2i(1, 4), ButtonType::ButtonCircuitBoundary, ComponentModifiers(ChargeNone, ChargeNone, 2, WireStandard, CircuitBoundaryClosed)));
        }
        else
        {
            int chargeIndex = 0;
            for (ChargeType charge : chargeLevels)
            {
                buttons.push_back(GameButton(info.name, vec2i(chargeIndex, 4), ButtonType::ButtonChargeColor, ComponentModifiers(charge)));
                chargeIndex++;
            }
        }
    }

    //
    // Add puzzle control buttons
    //
    buttons.push_back(GameButton("Start", vec2i(0, 0), ButtonType::ButtonPuzzleControl, ComponentModifiers()));
    buttons.push_back(GameButton("Pause", vec2i(1, 0), ButtonType::ButtonPuzzleControl, ComponentModifiers()));
    buttons.push_back(GameButton("Stop", vec2i(2, 0), ButtonType::ButtonPuzzleControl, ComponentModifiers()));
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
        selected |= (selectedMenuComponent != nullptr && button.component == selectedMenuComponent);
        if (gameComponent != nullptr)
        {
            selected |= (button.type == ButtonChargePreference && gameComponent->modifiers.chargePreference == button.modifiers.chargePreference);
            selected |= (button.type == ButtonChargeColor && gameComponent->modifiers.color == button.modifiers.color);
            selected |= (button.type == ButtonWireSpeed && gameComponent->modifiers.speed == button.modifiers.speed);
            selected |= (button.type == ButtonCircuitBoundary && gameComponent->modifiers.boundary == button.modifiers.boundary);
        }
        renderButton(button, selected);
	}

    stable_sort(backgroundObjects.begin(), backgroundObjects.end());
}

void GameUI::renderBuildingGrid()
{
    Texture &border = database().getTexture(app.renderer, "Border");
    for (const auto &cell : app.state.board.cells)
	{
		//if (cell.value.c == nullptr && !cell.value.blocked)
        if (!cell.value.blocked)
		{
			const rect2f rectangle = rect2f(vec2f(cell.x, cell.y), vec2f(cell.x + 1, cell.y + 1));
			const rect2f screenRect = params().boardInWindow.toContainer(rectangle);
            addBackgroundObject(border, screenRect, depthLayers::background);
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

void GameUI::renderButton(const GameButton &button, bool selected)
{
    Texture &borderTex = database().getTexture(app.renderer, "Border");
    
    const rect2f screenRect = GameUtil::canonicalToWindow(GameUtil::getCanonicalSize(), button.canonicalRect);
    addBackgroundObject(borderTex, screenRect, depthLayers::background);
    
    renderLocalizedComponent(button.name, nullptr, screenRect, button.modifiers, selected, true, true, 0.0f);
}

void GameUI::renderLocalizedComponent(const string &name, const Component *dynamicComponent, const rect2f &screenRect, const ComponentModifiers &modifiers, bool selected, bool isButton, bool isBackground, float depthOffset)
{
    if (!isButton && name == "Blocker" &&
        (selectedMenuComponent == nullptr || selectedMenuComponent->name != "Blocker"))
        return;

    Texture &baseTex = database().getTexture(app.renderer, "WireBase");
    Texture &componentTex = database().getTexture(app.renderer, name, modifiers);
    Texture &preferenceTex = *database().preferenceTextures[modifiers.chargePreference];
    
    auto record = [&](Texture &tex, const rect2f &rect, float depth, const Component *component) {
        depth -= depthOffset;
        if (isBackground)
            addBackgroundObject(tex, rect, depth, component);
        else
            render(tex, rect, depth);
    };

    record(preferenceTex, screenRect, 1.0f, nullptr);

    if (name != "Blocker" && modifiers.boundary != CircuitBoundaryClosed)
        record(baseTex, screenRect, 1.0f, nullptr);

    record(componentTex, screenRect, depthLayers::component, dynamicComponent);

    if (selected)
    {
        bool usePuzzleSelector = (dynamicComponent != nullptr && dynamicComponent->modifiers.puzzleType == ComponentPuzzlePiece);
            
        Texture &selectionTex = usePuzzleSelector ? database().getTexture(app.renderer, "PuzzleSelector") : database().getTexture(app.renderer, "Selector");
        record(selectionTex, screenRect, depthLayers::selection, nullptr);
    }
}

void GameUI::renderComponent(const Component &component)
{
    Component *selectedGameComponent = app.state.getComponent(selectedGameLocation);
   
    if (component.location.inCircuit())
    {
        // if the component is in the active circuit, render it in the selected circuit area
		if (activeCircuit() != nullptr && component.location.boardPos == activeCircuit()->location.boardPos)
        {
            if (!component.inactiveBoundary())
            {
                const rect2f screenRect = GameUtil::circuitToWindowRect(canonicalDims, component.location.circuitPos, 2);
                renderLocalizedComponent(component.info->name, &component, screenRect, component.modifiers, (selectedGameComponent != nullptr && component.location == selectedGameComponent->location), false, true, 0.0f);
            }
        }
		// regardless, we'll need to render it in the main grid, but we'll wait until later

    }
    else
    {
        const rect2f screenRect = GameUtil::boardToWindowRect(canonicalDims, component.location.boardPos, 2);
        renderLocalizedComponent(component.info->name, &component, screenRect, component.modifiers, (selectedGameComponent != nullptr && component.location == selectedGameComponent->location), false, true, 0.0f);
    }
}

void GameUI::renderCircuitComponent(const Component &component)
{
	if (!component.location.inCircuit() || component.inactiveBoundary() || component.modifiers.boundary == CircuitBoundaryClosed)
		return;
    const CoordinateFrame frame = CoordinateFrame(component.location.boardPos, component.location.boardPos + vec2f(2.0f, 2.0f), vec2i(constants::circuitBoardSize, constants::circuitBoardSize));
    const rect2f circuitRect = rect2f(component.location.circuitPos, component.location.circuitPos + 2);
    const rect2f screenRect = params().boardInWindow.toContainer(frame.toContainer(circuitRect));
    renderLocalizedComponent(component.info->name, &component, screenRect, component.modifiers, false, false, true, depthLayers::miniCircuitOffset);
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
                if (otherComponent.location.boardPos == otherLocation && otherComponent.hasSpokes())
                {
                    const vec2f otherScreenPos = GameUtil::boardToWindow(canonicalDims, otherComponent.location.boardPos + vec2i(1, 1));
                    
                    const vec2f middle = (myScreenPos + otherScreenPos) * 0.5f;
                    const vec2f diff = myScreenPos - otherScreenPos;
                    const float dist = diff.length();

                    const vec2f variance = constants::connectorDims[type + 1] * dist;
                    
                    Texture &connectorTex = database().getTexture(app.renderer, "WireConnector" + std::to_string(connectorIndex));
                    //const float angle = 180.0f;
                    const float angle = math::radiansToDegrees(atan2f(diff.y, diff.x)) + 180.0f;
                    addBackgroundObject(connectorTex, rect2f(middle - variance, middle + variance), depthLayers::spokes, nullptr, angle);
                }
            }
        }
    }
}

void GameUI::renderSpokesCircuit(const Component &component)
{
    if (activeCircuit() == nullptr || activeCircuit()->location.boardPos != component.location.boardPos)
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
                    addBackgroundObject(connectorTex, rect2f(middle - variance, middle + variance), depthLayers::spokes, nullptr, angle);
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
    const float s = float(charge.timeInTransit) / float(charge.totalTransitTime);
    const pair<vec2f, float> screen = GameUtil::computeChargeScreenPos(charge.source, charge.destination, s, charge.level, canonicalDims);
    const float angle = charge.randomRotationOffset + app.state.globalRotationOffset;
    const rect2f destinationRect(screen.first - vec2f(screen.second), screen.first + vec2f(screen.second));
	render(*database().chargeTextures[charge.level], destinationRect, depthLayers::charge, angle);
}

void GameUI::renderExplodingCharge(const ExplodingCharge &charge)
{
    renderExplodingChargeCircuit(charge);
    const pair<vec2f, float> screen = GameUtil::computeChargeScreenPos(charge.locationA, charge.locationB, charge.interpolation, charge.level, canonicalDims);
    const float angle = charge.baseRotationOffset + (app.state.stepCount - charge.birthTick) / constants::stepsPerSecond * constants::chargeRotationsPerSecond * 360.0f * constants::explodingChargeRotationFactor;
    const float scale = screen.second * math::lerp(1.0f, 3.0f, charge.percentDone());
    const rect2f destinationRect(screen.first - vec2f(scale), screen.first + vec2f(scale));

    const vec4f color(1.0f, 1.0f, 1.0f, 1.0f - charge.percentDone() * charge.percentDone());

    render(*database().chargeTextures[charge.level], destinationRect, depthLayers::charge, angle, color);
}

void GameUI::renderChargeCircuit(const Charge &charge)
{
    if (!charge.source.inCircuit() || !charge.destination.inCircuit()) return;
    if (activeCircuit() == nullptr || charge.source.boardPos != activeCircuit()->location.boardPos) return;

    const float s = float(charge.timeInTransit) / float(charge.totalTransitTime);
    const pair<vec2f, float> screen = GameUtil::computeChargeScreenPosCircuit(charge.source, charge.destination, s, charge.level, canonicalDims);
    const float angle = charge.randomRotationOffset + app.state.globalRotationOffset;

    const rect2f destinationRect(screen.first - vec2f(screen.second), screen.first + vec2f(screen.second));
    render(*database().chargeTextures[charge.level], destinationRect, depthLayers::charge, angle);
}

void GameUI::renderExplodingChargeCircuit(const ExplodingCharge &charge)
{
    if (!charge.locationA.inCircuit() || !charge.locationB.inCircuit()) return;
    if (activeCircuit() == nullptr || charge.locationA.boardPos != activeCircuit()->location.boardPos) return;

    const pair<vec2f, float> screen = GameUtil::computeChargeScreenPosCircuit(charge.locationA, charge.locationB, charge.interpolation, charge.level, canonicalDims);
    const float angle = charge.baseRotationOffset + (app.state.stepCount - charge.birthTick) / constants::stepsPerSecond * constants::chargeRotationsPerSecond * 360.0f * constants::explodingChargeRotationFactor;
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
