
#include "main.h"

void GameUI::init()
{
    selectedMenuComponent = nullptr;
}

void GameUI::step()
{
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
    }

    if (button == SDL_BUTTON_LEFT)
    {
        addHoverComponent();
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
            }
            if (button.name == "Stop")
            {
                mode = ModeDesign;
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

    const vec2i boardLocation = hoverBoardLocation();
    if (boardLocation == constants::invalidCoord)
        return;

    if (app.state.board.coordValidForNewComponent(boardLocation))
    {
        Component *newComponent = new Component(selectedMenuComponent->name, selectedMenuComponent->defaultPrimaryCharge(), GameLocation(boardLocation));
        app.state.addNewComponent(newComponent);
    }
}

void GameUI::render()
{
	updateBackground();

    app.renderer.setDefaultRenderTarget();

    app.renderer.render(background, 0, 0);

    renderHoverComponent();
}

vec2i GameUI::hoverBoardLocation()
{
    const vec2f boardCoordf = GameUtil::windowToBoard(windowDims, mouseHoverCoord);
    const vec2i boardCoordi(math::round(boardCoordf) - vec2i(1, 1));

    if (!math::between(boardCoordi.x, 0, 22) || !math::between(boardCoordi.y, 0, 22))
        return constants::invalidCoord;

    return boardCoordi;
}
void GameUI::renderHoverComponent()
{
    if (selectedMenuComponent == nullptr)
        return;

    const vec2i boardLocation = hoverBoardLocation();
    if (boardLocation == constants::invalidCoord)
        return;

    const rect2f screenRect = GameUtil::boardToWindowRect(windowDims, boardLocation, 2);

    renderLocalizedComponent(*selectedMenuComponent, selectedMenuComponent->defaultPrimaryCharge(), screenRect);
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
            ChargeType primaryCharge = info.defaultPrimaryCharge();
            ChargeType secondaryCharge = info.defaultSecondaryCharge();

            if (info.colorUpgrades)
                primaryCharge = ChargeRed;

            if (info.name == "ChargeGoal" || info.name == "Hold")
                secondaryCharge = ChargeGray;

            buttons.push_back(GameButton(info.name, info.menuCoordinate, ButtonType::ButtonComponent, primaryCharge, secondaryCharge));
		}
	}

    //
    // Add puzzle control buttons
    //
    buttons.push_back(GameButton("Start", vec2i(0, 0), ButtonType::ButtonPuzzleControl));
    buttons.push_back(GameButton("Pause", vec2i(1, 0), ButtonType::ButtonPuzzleControl));
    buttons.push_back(GameButton("Stop", vec2i(2, 0), ButtonType::ButtonPuzzleControl));
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

void GameUI::renderLocalizedComponent(const ComponentInfo &info, ChargeType charge, const rect2f &screenRect)
{
    Texture &baseTex = database().getTexture(app.renderer, "WireBase");
    Texture &componentTex = database().getTexture(app.renderer, info.name, charge);

    app.renderer.render(baseTex, screenRect);
    app.renderer.render(componentTex, screenRect);
}

void GameUI::renderComponent(const Component &component)
{
    if (!component.location.inCircuit())
    {
        const rect2f screenRect = GameUtil::boardToWindowRect(windowDims, component.location.boardPos, 2);
        renderLocalizedComponent(*component.info, component.color, screenRect);
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

            if (cells.isValidCoordinate(otherLocation) && cells(otherLocation).c != nullptr)
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
    vec2i screenSource, screenDest;

    /*float sourceScaleFactor = 1.0f + (C._Level - 1) * ChargeScaleWithLevelFactor;
    float destScaleFactor = 1.0f + (C._Level - 1) * ChargeScaleWithLevelFactor;

    screenSource = charge.source.toScreenCoord(windowDims);
    screenDest = charge.destination.toScreenCoord(windowDims);
    
    if (ActiveCircuit)
    {
        ScreenCoordinateEnd = CircuitCoordinateToScreenCoordinate(C._Destination.Location + Coordinate(1, 1));
        ScreenCoordinateStart = CircuitCoordinateToScreenCoordinate(C._Source.Location + Coordinate(1, 1));
    }
    else
    {
        

        if (C._Source.Circuit != Coordinate::Invalid)
        {
            StartScaleFactor = CircuitChargeSize;
        }
        if (C._Destination.Circuit != Coordinate::Invalid)
        {
            EndScaleFactor = CircuitChargeSize;
        }
    }

    if (C._Source.Location == Coordinate::Invalid)
    {
        ScreenCoordinateStart = ScreenCoordinateEnd;
    }

    Vec2 VecStart = ScreenCoordinateStart.AsVec2();
    Vec2 VecEnd = ScreenCoordinateEnd.AsVec2();

    float Factor = 0.0f;
    if (C._TotalTransitTime > 0)
    {
        Factor = float(C._TimeInTransit) / float(C._TotalTransitTime);
    }
    Vec2 VecFinal = Vec2::Lerp(VecStart, VecEnd, Factor);
    float ScaleFactor = Math::Lerp(StartScaleFactor, EndScaleFactor, Factor);

    Matrix4 Rotation = Matrix4::RotationZ(G.CurrentTime() * SecondsPerGameFrame * 2.0f * Math::PIf * ChargeSecondPerRotations + C.RandomValue() * 2.0f * Math::PIf);
    //Matrix4 Jitter = Matrix4::Translation(Vec3(ChargeJitterMagnitude * pmrnd(), ChargeJitterMagnitude * pmrnd(), 0.0f));
    Matrix4 Transform = Rotation * Matrix4::Scaling(Vec3(ScaleFactor, ScaleFactor, 1.0f)) * Matrix4::Translation(Vec3(VecFinal, ChargeLayerZDepth)) * InverseViewportMatrix;

    GD.LoadMatrix(MatrixController(Transform));

    if (UseTexturedCharges)
    {
        EnableAlphaBlending(GD, AlphaBlendingModeBuilding);
        _TexturedChargeTextures[C.Level() - 1].Set(0);
        _TexturedChargeMesh.Render();
        DisableAlphaBlending(GD);
    }
    else
    {
        _SphereChargeTextures[C.Level() - 1].Set(0);
        _SphereChargeMesh.Render();
    }*/
}