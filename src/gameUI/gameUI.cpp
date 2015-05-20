
#include "main.h"

void GameUI::init()
{
    mode = ModeDesign;
    speed = Speed1x;

    selectedGameLocation.boardPos = constants::invalidCoord;

    designActionTaken = false;
    selectedMenuComponent = nullptr;

    for (int chargeLevel = 0; chargeLevel < constants::maxChargeLevel; chargeLevel++)
    {
        chargeTextures[chargeLevel] = &database().getTexture(app.renderer, "ChargeTexture" + to_string(chargeLevel));
    }
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
}

void GameUI::mouseDown(Uint8 button, int x, int y)
{
    if (button == SDL_BUTTON_RIGHT)
    {
        selectedMenuComponent = nullptr;

        const GameLocation location = hoverBoardLocation();
        if (app.state.board.cells.coordValid(location.boardPos) && app.state.board.cells(location.boardPos).c != nullptr)
        {
            designActionTaken = true;
            app.state.removeComponent(app.state.board.cells(location.boardPos).c);
        }
    }

    if (button == SDL_BUTTON_LEFT)
    {
        if (selectedMenuComponent == nullptr)
        {
            selectedGameLocation = hoverBoardLocation();
        }
        else
        {
            addHoverComponent();
        }
    }

    for (const auto &button : buttons)
    {
        const rect2f screenRect = GameUtil::canonicalToWindow(app.renderer.getWindowSize(), button.canonicalRect);
        if (screenRect.intersects(vec2f((float)x, (float)y)))
        {
            if (button.type == ButtonComponent)
            {
                selectedMenuComponent = button.component;
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
}

void GameUI::addHoverComponent()
{
    if (selectedMenuComponent == nullptr)
        return;

    const GameLocation boardLocation = hoverBoardLocation();
    if (boardLocation.boardPos == constants::invalidCoord)
        return;

    if (app.state.board.coordValidForNewComponent(boardLocation.boardPos))
    {
        Component *newComponent = new Component(selectedMenuComponent->name, selectedMenuComponent->defaultPrimaryCharge(), GameLocation(boardLocation));
        app.state.addNewComponent(newComponent);
        designActionTaken = true;
    }
}

void GameUI::render()
{
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

GameLocation GameUI::hoverBoardLocation() const
{
    const vec2f boardCoordf = GameUtil::windowToBoard(windowDims, mouseHoverCoord);
    const vec2i boardCoordi(math::round(boardCoordf) - vec2i(1, 1));

    if (!math::between(boardCoordi.x, 0, 23) || !math::between(boardCoordi.y, 0, 23))
        return GameLocation(constants::invalidCoord);

    return GameLocation(boardCoordi);
}
void GameUI::renderHoverComponent()
{
    if (selectedMenuComponent == nullptr)
        return;

    const GameLocation boardLocation = hoverBoardLocation();
    if (boardLocation.boardPos == constants::invalidCoord)
        return;

    const rect2f screenRect = GameUtil::boardToWindowRect(windowDims, boardLocation.boardPos, 2);

    renderLocalizedComponent(*selectedMenuComponent, screenRect, ComponentModifiers(*selectedMenuComponent));
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
    if (selectedMenuComponent != nullptr)
    {
        const ComponentInfo &info = *selectedMenuComponent;

        vector<ChargeType> chargeLevels;
        if (info.colorUpgrades)
        {
            for (int charge = (int)ChargeRed; charge <= (int)ChargeBlue; charge++)
                chargeLevels.push_back((ChargeType)charge);
        }
        if (info.grayUpgrade)
            chargeLevels.push_back(ChargeGray);

        int chargeIndex = 0;
        for (ChargeType charge : chargeLevels)
        {
            buttons.push_back(GameButton(info.name, vec2i(chargeIndex, 4), ButtonType::ButtonChargeColor, ComponentModifiers(charge)));
            chargeIndex++;
        }

        if (info.name == "Wire")
        {
            for (int speed = 0; speed <= 4; speed++)
            {
                buttons.push_back(GameButton("Wire", vec2i((int)speed, 5), ButtonType::ButtonWireSpeed, ComponentModifiers(ChargeNone, ChargeNone, 2, (WireSpeedType)speed)));
            }
        }
        else
        {
            for (int chargePreference = 0; chargePreference <= 4; chargePreference++)
            {
                buttons.push_back(GameButton(info.name, vec2i(chargePreference, 5), ButtonType::ButtonPreference, ComponentModifiers(info.defaultPrimaryCharge(), info.defaultSecondaryCharge(), chargePreference)));
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
	if (vec2i((int)background.bmp().width(), (int)background.bmp().height()) != windowDims)
    {
		cout << "Resizing render target: " << windowDims << endl;
		background.loadRenderTarget(windowDims.x, windowDims.y);
    }

	updateButtonList();

    app.renderer.setRenderTarget(background);

    app.renderer.render(database().getTexture(app.renderer, "Background"), rect2i(0, 0, windowDims.x, windowDims.y));

	renderBuildingGrid();

    renderComponents(true);

    for (auto &component : app.state.components)
    {
        renderSpokes(*component);
    }

	for (auto &button : buttons)
	{
        button.render(app.renderer, (selectedMenuComponent != nullptr && button.component == selectedMenuComponent));
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
			const rect2f screenRect = GameUtil::boardToWindowRect(windowDims, vec2i(cell.x, cell.y), 1);
            app.renderer.render(border, screenRect);
		}
	}
}

void GameUI::renderLocalizedComponent(const ComponentInfo &info, const rect2f &screenRect, const ComponentModifiers &modifiers)
{
    Texture &baseTex = database().getTexture(app.renderer, "WireBase");
    Texture &componentTex = database().getTexture(app.renderer, info.name, modifiers.color, modifiers.secondaryColor);

    app.renderer.render(baseTex, screenRect);
    app.renderer.render(componentTex, screenRect);
}

void GameUI::renderComponent(const Component &component)
{
    if (!component.location.inCircuit())
    {
        const rect2f screenRect = GameUtil::boardToWindowRect(windowDims, component.location.boardPos, 2);
        renderLocalizedComponent(*component.info, screenRect, component.modifiers);
    }
}

void GameUI::renderSpokes(const Component &component)
{
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
                if (otherComponent.location.boardPos == otherLocation)
                {
                    const vec2f otherScreenPos = GameUtil::boardToWindow(windowDims, otherComponent.location.boardPos + vec2i(1, 1));
                    
                    const vec2f middle = (myScreenPos + otherScreenPos) * 0.5f;
                    const vec2f diff = myScreenPos - otherScreenPos;
                    const float dist = diff.length();

                    const vec2f variance(dist * 0.18f, dist * 0.045f);
                    
                    Texture &connectorTex = database().getTexture(app.renderer, "WireConnector" + std::to_string(connectorIndex));
                    //const float angle = 180.0f;
                    const float angle = math::radiansToDegrees(atan2f(diff.y, diff.x)) + 180.0f;
                    app.renderer.render(connectorTex, math::round(rect2f(middle - variance, middle + variance)), angle);
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
            renderComponent(*component);
    }
}

void GameUI::renderCharge(const Charge &charge)
{
    const float s = float(charge.timeInTransit) / float(charge.totalTransitTime);

    const pair<vec2f, float> screen = GameUtil::computeChargeScreenPos(charge.source, charge.destination, s, charge.level, windowDims);
    
    const float angle = charge.rotationOffset(app.state.stepCount);

    const rect2i destinationRect(screen.first - vec2f(screen.second), screen.first + vec2f(screen.second));

    app.renderer.render(*chargeTextures[charge.level], destinationRect, angle);
}

void GameUI::renderExplodingCharge(const ExplodingCharge &charge)
{
    const pair<vec2f, float> screen = GameUtil::computeChargeScreenPos(charge.locationA, charge.locationB, charge.interpolation, charge.level, windowDims);

    const float angle = charge.baseRotationOffset + (app.state.stepCount - charge.birthTick) / constants::stepsPerSecond * constants::chargeRotationsPerSecond * 360.0f * constants::explodingChargeRotationFactor;

    const float scale = screen.second * math::lerp(1.0f, 3.0f, charge.percentDone());

    const rect2i destinationRect(screen.first - vec2f(scale), screen.first + vec2f(scale));

    app.renderer.render(*chargeTextures[charge.level], destinationRect, angle);
}