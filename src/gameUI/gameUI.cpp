
#include "main.h"

void GameUI::init()
{
    mode = ModeDesign;
    speed = Speed1x;

    selectedGameLocation.boardPos = constants::invalidCoord;

    designActionTaken = false;
    selectedMenuComponent = nullptr;
}

void GameUI::step()
{
    if (designActionTaken && mode != ModeDesign)
    {
        mode = ModeDesign;
        app.state.resetPuzzle();
    }

    if (mode == ModeExecuting)
    {
        const int tickCount = ticksFromSpeed(speed);
        for (int tickIndex = 0; tickIndex < tickCount; tickIndex++)
        {
            app.state.step();
        }
    }
}

void GameUI::keyDown(SDL_Keycode key)
{
    if (key == SDLK_ESCAPE)
    {
        selectedMenuComponent = nullptr;
    }
    if (key == SDLK_s)
    {
        app.state.savePuzzle("test.txt");
    }
}

void GameUI::removeHoverComponent()
{
    const GameLocation location = hoverLocation(false);
    if (location.inCircuit())
    {
        if (activeCircuit() != nullptr && activeCircuit()->circuitBoard->cells.coordValid(location.circuitPos) && activeCircuit()->circuitBoard->cells(location.circuitPos).c != nullptr)
        {
            designActionTaken = true;
            app.state.removeComponent(activeCircuit()->circuitBoard->cells(location.circuitPos).c);
        }
    }
    else
    {
        if (app.state.board.cells.coordValid(location.boardPos) && app.state.board.cells(location.boardPos).c != nullptr)
        {
            designActionTaken = true;
            app.state.removeComponent(app.state.board.cells(location.boardPos).c);
        }
    }
}

void GameUI::mouseDown(Uint8 button, int x, int y)
{
	CoordinateFrame windowFrame = app.renderer.getWindowCoordinateFrame();
    mouseHoverCoord = vec2i(windowFrame.fromContainer(vec2f((float) x,(float) y)));

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
            const rect2f screenRect = GameUtil::canonicalToWindow(app.renderer.getWindowSize(), button.canonicalRect);
            Component *gameComponent = app.state.getComponent(selectedGameLocation);
            if (screenRect.intersects(vec2f((float)x, (float)y)))
            {
                if (button.type == ButtonComponent)
                {
                    selectedMenuComponent = button.component;
                }
                if (button.type == ButtonChargeColor && gameComponent != nullptr)
                {
                    designActionTaken = true;
                    gameComponent->modifiers.color = button.modifiers.color;
                }
                if (button.type == ButtonChargePreference && gameComponent != nullptr)
                {
                    designActionTaken = true;
                    gameComponent->modifiers.chargePreference = button.modifiers.chargePreference;
                }
                if (button.type == ButtonWireSpeed && gameComponent != nullptr)
                {
                    designActionTaken = true;
                    gameComponent->modifiers.speed = button.modifiers.speed;
                }
                if (button.name == "Start")
                {
                    mode = ModeExecuting;
                    designActionTaken = false;
                }
                if (button.name == "Stop")
                {
                    mode = ModeDesign;
                    app.state.resetPuzzle();
                }
                if (button.name == "Pause")
                {
                    mode = ModePaused;
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
    mouseHoverCoord = vec2i(x, y);

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
        if (activeCircuit() != nullptr && activeCircuit()->circuitBoard->coordValidForNewComponent(location.circuitPos))
        {
            Component *newComponent = new Component(selectedMenuComponent->name, selectedMenuComponent->defaultPrimaryCharge(), location);
            app.state.addNewComponent(newComponent);
            designActionTaken = true;
        }
    }
    else if (app.state.board.coordValidForNewComponent(location.boardPos))
    {
        Component *newComponent = new Component(selectedMenuComponent->name, selectedMenuComponent->defaultPrimaryCharge(), location);
        app.state.addNewComponent(newComponent);
        designActionTaken = true;
    }
}

void GameUI::render()
{
    // TODO: don't do this every frame
	updateBackground();

    app.renderer.setDefaultRenderTarget();

    app.renderer.render(background, 0, 0);

    for (const auto &charge : app.state.charges)
    {
        renderCharge(charge);
    }

    for (const auto &charge : app.state.explodingCharges)
    {
        renderExplodingCharge(charge);
    }
    
    renderHoverComponent();
}

GameLocation GameUI::hoverLocation(bool constructionOffset) const
{
    const vec2f boardCoordf = GameUtil::windowToBoard(windowDims, mouseHoverCoord);
    const vec2f circuitCoordf = GameUtil::windowToCircuit(windowDims, mouseHoverCoord);

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

    const rect2f screenRect = GameUtil::locationToWindowRect(windowDims, location, 2);
    
    renderLocalizedComponent(selectedMenuComponent->name, screenRect, ComponentModifiers(*selectedMenuComponent), false, false);

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
                    GameUtil::circuitToWindowRect(windowDims, coord, 1) :
                    GameUtil::boardToWindowRect(windowDims, coord, 1);
                app.renderer.render(*tex, rect,coordinateFrame);
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
    // Add color, delay, and preference buttons
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

        for (int chargePreference = 0; chargePreference <= 4; chargePreference++)
        {
            buttons.push_back(GameButton(info.name, vec2i(chargePreference, 3), ButtonType::ButtonChargePreference, ComponentModifiers(info.defaultPrimaryCharge(), info.defaultSecondaryCharge(), chargePreference)));
        }

        if (info.name == "Wire")
        {
            for (int speed = 0; speed <= 4; speed++)
            {
                buttons.push_back(GameButton("Wire", vec2i((int)speed, 4), ButtonType::ButtonWireSpeed, ComponentModifiers(ChargeNone, ChargeNone, 2, (WireSpeedType)speed)));
            }
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

void GameUI::updateBackground()
{
	//
	// resize the background if needed
	//
    windowDims = app.renderer.getWindowSize();
	coordinateFrame = app.renderer.getWindowCoordinateFrame();

	if (vec2i((int)background.bmp().width(), (int)background.bmp().height()) != windowDims)
    {
		cout << "Resizing render target: " << windowDims << endl;
		background.loadRenderTarget(windowDims.x, windowDims.y);
    }

	updateButtonList();

    Component *gameComponent = app.state.getComponent(selectedGameLocation);

    app.renderer.setRenderTarget(background);

	app.renderer.render(database().getTexture(app.renderer, "Background"), rect2i(0, 0, windowDims.x, windowDims.y), coordinateFrame);

    if (activeCircuit() != nullptr)
    {
        const rect2f rect(params().circuitCanonicalStart - vec2f(params().circuitBackgroundCanonicalBoundarySize), params().circuitCanonicalStart + (float)constants::circuitBoardSize * params().canonicalCellSize + vec2f(params().circuitBackgroundCanonicalBoundarySize));
		app.renderer.render(database().getTexture(app.renderer, "CircuitBackground"), GameUtil::canonicalToWindow(windowDims, rect), coordinateFrame);
    }

	renderBuildingGrid();

    renderComponents(true);

    for (auto &component : app.state.components)
    {
        renderSpokes(*component);
    }

	for (auto &button : buttons)
	{
        bool selected = false;
        selected |= (selectedMenuComponent != nullptr && button.component == selectedMenuComponent);
        selected |= (gameComponent != nullptr && button.type == ButtonChargePreference && gameComponent->modifiers.chargePreference == button.modifiers.chargePreference);
        selected |= (gameComponent != nullptr && button.type == ButtonChargeColor && gameComponent->modifiers.color == button.modifiers.color);
        selected |= (gameComponent != nullptr && button.type == ButtonWireSpeed && gameComponent->modifiers.speed == button.modifiers.speed);
        renderButton(button, selected);
	}

    app.renderer.setDefaultRenderTarget();
}

void GameUI::renderBuildingGrid()
{
    Texture &border = database().getTexture(app.renderer, "Border");
    for (const auto &cell : app.state.board.cells)
	{
		//if (cell.value.c == nullptr && !cell.value.blocked)
        if (!cell.value.blocked)
		{
			rect2f rectangle = rect2f(vec2f(cell.x, cell.y), vec2f(cell.x + 1, cell.y + 1));
			const rect2f screenRect = params().boardInWindow.toContainer(rectangle);
			app.renderer.render(border, screenRect, coordinateFrame);
		}
	}

    Component *circuit = activeCircuit();
    if (circuit != nullptr)
    {
        for (const auto &cell : circuit->circuitBoard->cells)
        {
            const bool xBoundary = (cell.x == 0 || cell.x == 1 || cell.x == constants::circuitBoardSize - 1 || cell.x == constants::circuitBoardSize - 2);
            const bool yBoundary = (cell.y == 0 || cell.y == 1 || cell.y == constants::circuitBoardSize - 1 || cell.y == constants::circuitBoardSize - 2);
            if (!xBoundary || !yBoundary)
            {
                const rect2f screenRect = GameUtil::circuitToWindowRect(windowDims, vec2i(cell.x, cell.y), 1);
				app.renderer.render(border, screenRect, coordinateFrame);
            }
        }
    }

}

void GameUI::renderButton(const GameButton &button, bool selected)
{
    Texture &borderTex = database().getTexture(app.renderer, "Border");
    
    const rect2f screenRect = GameUtil::canonicalToWindow(app.renderer.getWindowSize(), button.canonicalRect);
	app.renderer.render(borderTex, screenRect, coordinateFrame);
    
    renderLocalizedComponent(button.name, screenRect, button.modifiers, selected, true);
}

void GameUI::renderLocalizedComponent(const string &name, const rect2f &screenRect, const ComponentModifiers &modifiers, bool selected, bool isButton)
{
    if (!isButton && name == "Blocker" &&
        (selectedMenuComponent == nullptr || selectedMenuComponent->name != "Blocker"))
        return;

    Texture &baseTex = database().getTexture(app.renderer, "WireBase");
    Texture &componentTex = database().getTexture(app.renderer, name, modifiers);
    Texture &preferenceTex = *database().preferenceTextures[modifiers.chargePreference];
    
	app.renderer.render(preferenceTex, screenRect, coordinateFrame);

    if (name != "Blocker")
		app.renderer.render(baseTex, screenRect, coordinateFrame);

	app.renderer.render(componentTex, screenRect, coordinateFrame);

    if (selected)
    {
        Texture &selectionTex = database().getTexture(app.renderer, "Selector");
		app.renderer.render(selectionTex, screenRect, coordinateFrame);
    }
}

void GameUI::renderComponent(const Component &component)
{
    Component *selectedGameComponent = app.state.getComponent(selectedGameLocation);
    
    if (component.location.inCircuit())
    {
        if (activeCircuit() != nullptr && component.location.boardPos == activeCircuit()->location.boardPos)
        {
            const rect2f screenRect = GameUtil::circuitToWindowRect(windowDims, component.location.circuitPos, 2);
            renderLocalizedComponent(component.info->name, screenRect, component.modifiers, (selectedGameComponent != nullptr && component.location == selectedGameComponent->location), false);
        }
    }
    else
    {
        const rect2f screenRect = GameUtil::boardToWindowRect(windowDims, component.location.boardPos, 2);
        renderLocalizedComponent(component.info->name, screenRect, component.modifiers, (selectedGameComponent != nullptr && component.location == selectedGameComponent->location), false);
    }
}

void GameUI::renderSpokes(const Component &component)
{
    if (!component.info->hasSpokes())
        return;

    if (component.location.inCircuit())
    {
        renderSpokesCircuit(component);
        return;
    }

    const vec2f myScreenPos = GameUtil::boardToWindow(windowDims, component.location.boardPos + vec2i(1, 1));

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
                if (otherComponent.location.boardPos == otherLocation && component.info->hasSpokes())
                {
                    const vec2f otherScreenPos = GameUtil::boardToWindow(windowDims, otherComponent.location.boardPos + vec2i(1, 1));
                    
                    const vec2f middle = (myScreenPos + otherScreenPos) * 0.5f;
                    const vec2f diff = myScreenPos - otherScreenPos;
                    const float dist = diff.length();

                    const vec2f variance(dist * 0.18f, dist * 0.045f);
                    
                    Texture &connectorTex = database().getTexture(app.renderer, "WireConnector" + std::to_string(connectorIndex));
                    //const float angle = 180.0f;
                    const float angle = math::radiansToDegrees(atan2f(diff.y, diff.x)) + 180.0f;
					app.renderer.render(connectorTex, math::round(rect2f(middle - variance, middle + variance)), angle, coordinateFrame);
                }
            }
        }
    }
}

void GameUI::renderSpokesCircuit(const Component &component)
{
    if (selectedGameLocation.boardPos != component.location.boardPos)
        return;

    const vec2f myScreenPos = GameUtil::circuitToWindow(windowDims, component.location.circuitPos + vec2i(1, 1));

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
                if (otherComponent.location.circuitPos == otherLocation && component.info->hasSpokes())
                {
                    const vec2f otherScreenPos = GameUtil::circuitToWindow(windowDims, otherComponent.location.circuitPos + vec2i(1, 1));

                    const vec2f middle = (myScreenPos + otherScreenPos) * 0.5f;
                    const vec2f diff = myScreenPos - otherScreenPos;
                    const float dist = diff.length();

                    const vec2f variance(dist * 0.18f, dist * 0.045f);

                    Texture &connectorTex = database().getTexture(app.renderer, "WireConnector" + std::to_string(connectorIndex));
                    const float angle = math::radiansToDegrees(atan2f(diff.y, diff.x)) + 180.0f;
					app.renderer.render(connectorTex, math::round(rect2f(middle - variance, middle + variance)), angle, coordinateFrame);
                }
            }
        }
    }
}

void GameUI::renderComponents(bool background)
{
    for (auto &component : app.state.components)
    {
        if (component->info->background == background)
        {
            renderComponent(*component);
        }
    }
}

void GameUI::renderCharge(const Charge &charge)
{
    renderChargeCircuit(charge);
    const float s = float(charge.timeInTransit) / float(charge.totalTransitTime);
    const pair<vec2f, float> screen = GameUtil::computeChargeScreenPos(charge.source, charge.destination, s, charge.level, windowDims);
    const float angle = charge.randomRotationOffset + app.state.globalRotationOffset;
    const rect2i destinationRect(screen.first - vec2f(screen.second), screen.first + vec2f(screen.second));
	app.renderer.render(*database().chargeTextures[charge.level], destinationRect, angle, coordinateFrame);
}

void GameUI::renderExplodingCharge(const ExplodingCharge &charge)
{
    renderExplodingChargeCircuit(charge);
    const pair<vec2f, float> screen = GameUtil::computeChargeScreenPos(charge.locationA, charge.locationB, charge.interpolation, charge.level, windowDims);
    const float angle = charge.baseRotationOffset + (app.state.stepCount - charge.birthTick) / constants::stepsPerSecond * constants::chargeRotationsPerSecond * 360.0f * constants::explodingChargeRotationFactor;
    const float scale = screen.second * math::lerp(1.0f, 3.0f, charge.percentDone());
    const rect2i destinationRect(screen.first - vec2f(scale), screen.first + vec2f(scale));
	app.renderer.render(*database().chargeTextures[charge.level], destinationRect, angle, coordinateFrame);
}

void GameUI::renderChargeCircuit(const Charge &charge)
{
    if (!charge.source.inCircuit() || !charge.destination.inCircuit()) return;
    if (activeCircuit() == nullptr || charge.source.boardPos != activeCircuit()->location.boardPos) return;

    const float s = float(charge.timeInTransit) / float(charge.totalTransitTime);
    const pair<vec2f, float> screen = GameUtil::computeChargeScreenPosCircuit(charge.source, charge.destination, s, charge.level, windowDims);
    const float angle = charge.randomRotationOffset + app.state.globalRotationOffset;
    const rect2i destinationRect(screen.first - vec2f(screen.second), screen.first + vec2f(screen.second));
	app.renderer.render(*database().chargeTextures[charge.level], destinationRect, angle, coordinateFrame);
}

void GameUI::renderExplodingChargeCircuit(const ExplodingCharge &charge)
{
    /*const pair<vec2f, float> screen = GameUtil::computeChargeScreenPos(charge.locationA, charge.locationB, charge.interpolation, charge.level, windowDims);
    const float angle = charge.baseRotationOffset + (app.state.stepCount - charge.birthTick) / constants::stepsPerSecond * constants::chargeRotationsPerSecond * 360.0f * constants::explodingChargeRotationFactor;
    const float scale = screen.second * math::lerp(1.0f, 3.0f, charge.percentDone());
    const rect2i destinationRect(screen.first - vec2f(scale), screen.first + vec2f(scale));
    app.renderer.render(*database().chargeTextures[charge.level], destinationRect, angle);*/
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
