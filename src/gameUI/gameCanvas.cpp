
#include "main.h"

void GameCanvas::init()
{
    backgroundDirty = true;
    errorResetBuffer = false;

    int classIndex = 0;
    for (int type = -1; type <= 1; type++)
    {
        for (int axis = 0; axis < 2; axis++)
        {
            vec2i offset;
            if (axis == 0) offset = vec2i(-2, type);
            if (axis == 1) offset = vec2i(type, -2);
            UINT connectorIndex = (type + 1) * 2 + axis;
            Texture &connectorTex = database().getTexture(app.renderer, "WireConnector" + std::to_string(connectorIndex));
            connectionClasses[classIndex++] = CanvasConnection(type, offset, connectorIndex, &connectorTex);
        }
    }
}

Texture& GameCanvas::getFontTexture(const string &text, FontType font, int wrapWidth)
{
    auto &cache = textCache[(int)font];
    if (cache.count(text) == 0)
    {
        auto &info = database().fonts[(int)font];
        Texture *t = new Texture(app.renderer, database().getFont(info.name), text, info.color, wrapWidth, info.glowStrength, info.glowColor);
        cache[text] = t;
    }
    return *cache[text];
}

void GameCanvas::render(Texture &tex, const rect2f &destinationRect, float depth, const vec4f &color, const CoordinateFrame &frame)
{
    app.renderer.render(tex, frame.toContainer(destinationRect), depth, color);
}

void GameCanvas::render(Texture &tex, const rect2f &destinationRect, float depth, float rotation, const vec4f &color, const CoordinateFrame &frame)
{
    app.renderer.render(tex, frame.toContainer(destinationRect), depth, rotation, color);
}

void GameCanvas::render(Texture &tex, const rect2f &destinationRect, float depth, const vec4f &color)
{
    render(tex, destinationRect, depth, color, coordinateFrame);
}

void GameCanvas::render(Texture &tex, const rect2f &destinationRect, float depth, float rotation, const vec4f &color)
{
    render(tex, destinationRect, depth, rotation, color, coordinateFrame);
}

void GameCanvas::render(const UIRenderObject &o)
{
    const vec4f color = (o.dynamicComponent != nullptr && o.type == UIRenderType::StoredCharge) ? o.dynamicComponent->modifiers.storedChargeColor : o.color;
    Texture &t = (o.dynamicComponent != nullptr && o.type == UIRenderType::Standard) ? database().getTexture(app.renderer, o.dynamicComponent->info->name, o.dynamicComponent->modifiers) : *o.tex;

    if (o.rotation == 0.0f)
        render(t, o.rect, o.depth, color);
    else
        render(t, o.rect, o.depth, o.rotation, color);
}

/*void GameCanvas::renderShadowText(const string &text, const FontType type, const vec2f &start, const float lineHeight, vec4f colorMain, vec4f colorShadow, float depth)
{
    Texture &tex = getFontTexture(app.state.puzzleName, FontType::LevelName);
    renderText(tex, start + vec2f(0.5f, 0.5f), lineHeight, vec4f(1.0f, 1.0f, 1.0f, 1.0f), depth);
    renderText(tex, start, lineHeight, vec4f(1.0f, 1.0f, 1.0f, 1.0f), depth);
}*/

void GameCanvas::renderText(Texture &tex, const vec2f &start, const float lineHeight, float depth)
{
    const float aspect = (float)tex.bmp().dimX() / (float)tex.bmp().dimY();

    const float lineCount = tex.bmp().dimY() / 58.0f;
    const float height = lineCount * lineHeight;

    const float width = aspect * height;

    const rect2f rect(start, vec2f(start.x + width, start.y + height));
    app.renderer.render(tex, coordinateFrame.toContainer(rect), depth, vec4f(1.0f, 1.0f, 1.0f, 1.0f));
}

void GameCanvas::render()
{
    app.renderer.bindMainRenderTarget();

    canonicalDims = GameUtil::getCanonicalSize();
    coordinateFrame = app.renderer.getWindowCoordinateFrame();

    //const bool updateBackground = params().alwaysUpdateBackground || backgroundDirty;
    if (backgroundDirty)
    {
        backgroundDirty = false;

        if (errorResetBuffer)
        {
            errorResetBuffer = false;
        }
        else
        {
            app.controller.tooltipErrorTitle = "";
            app.controller.tooltipErrorDescription = "";
        }

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

    for (const auto &component : app.state.components)
    {
        if (component->isSuperMegaHold(app.state))
            renderSuperMegaHold(*component);
    }

    for (const auto &button : app.controller.buttons)
    {
        renderButtonForeground(button, false);
    }

    if (app.renderer.type() == RendererType::OpenGL)
        renderTrails();

    for (const auto &charge : app.state.charges)
    {
        renderCharge(charge, false);
    }

    for (const auto &charge : app.state.explodingCharges)
    {
        renderExplodingCharge(charge);
    }

    for (const auto &component : app.state.components)
    {
        if (component->info->name == "MegaHold")
        {
            renderMegaHold(*component, false);
        }
    }

    renderHoverComponent();

    renderText(getFontTexture(app.state.puzzleName, FontType::LevelName), vec2f(5.0f, 1.0f), 20.0f);
    //renderShadowText(app.state.puzzleName, FontType::LevelName, vec2f(5.0f, 1.0f), 20.0f, vec4f(0.05f, 0.3f, 0.2f, 1.0f), vec4f(0.0f, 0.0f, 0.0f, 1.0f));

    for (auto &entry : menuText)
    {
        renderText(getFontTexture(entry.text, FontType::MenuTitle), entry.coord, 10.0f, depthLayers::fontMenu);
    }

    renderTooltip();

    if (app.activeCircuit() == nullptr)
    {
        renderVictoryPanel();
    }

    if (SDL_GetModState() & KMOD_ALT)
    {
        float fps = app.frameTimer.framesPerSecond();
        if (abs(fps - 60.0f) < 1.0f)  fps = 60.0f;
        renderText(getFontTexture("fps: " + util::formatDouble(fps, 1, false), FontType::TooltipName), vec2f(0.0f, 0.0f), 12.0f);
    }
}

void GameCanvas::renderTrails()
{
    if (trailTexture0.dimensions() != app.renderer.getWindowSize())
    {
        trailTexture0.init(app.renderer, app.renderer.getWindowSize());
        trailTexture1.init(app.renderer, app.renderer.getWindowSize());

        trailTexture0.clear(vec4f(0.0f, 0.0f, 0.0f, 0.0f));
    }

    glDisable(GL_DEPTH_TEST);

    //
    // additively render charges to trailTexture0
    //
    trailTexture0.bindAsRenderTarget();

    //app.renderer.render(database().getTexture(app.renderer, "Border"), rect2f(100.0f, 100.0f, 150.0f, 150.0f), 0.5f, vec4f(1.0f, 1.0f, 1.0f, 1.0f));

    for (const auto &charge : app.state.charges)
        renderCharge(charge, true);
    //for (const auto &charge : app.state.explodingCharges)
    //    renderExplodingCharge(charge);

    for (const auto &component : app.state.components)
    {
        if (component->info->name == "MegaHold")
            renderMegaHold(*component, true);
    }

    for (const UIRenderObject &o : backgroundObjects)
    {
        if (o.dynamicComponent != nullptr && o.type == UIRenderType::StoredCharge &&
            //(o.dynamicComponent->storedCharge != ChargeType::None || o.dynamicComponent->heldCharge != ChargeType::None) &&
            (o.dynamicComponent->info->name == "ChargeGoal" && o.dynamicComponent->heldCharge == o.dynamicComponent->modifiers.color))
        {
            const vec4f color = o.dynamicComponent->modifiers.storedChargeColor;
            //Texture &t = database().getTexture(app.renderer, o.dynamicComponent->info->name, o.dynamicComponent->modifiers);
            Texture &t = *o.tex;
            render(t, o.rect, o.depth, color);
        }
    }

    //LodePNG::save(trailTexture0.getImage(), "trailTextureA.png");

    //
    // blur in X to trailTexture1
    //
    trailTexture1.bindAsRenderTarget();

    glDisable(GL_BLEND);
    trailTexture0.bindAsTexture();

    glClear(GL_COLOR_BUFFER_BIT);
    app.renderer.renderGaussian(vec2f(1.0f, 0.0f) / (float)trailTexture0.dimensions().x);

    //LodePNG::save(trailTexture1.getImage(), "trailTextureB.png");

    //
    // blur in Y back to trailTexture0
    //
    trailTexture0.bindAsRenderTarget();

    trailTexture1.bindAsTexture();

    app.renderer.renderGaussian(vec2f(0.0f, 1.0f) / (float)trailTexture0.dimensions().y);

    glEnable(GL_BLEND);
    
    app.renderer.bindMainRenderTarget();

    //LodePNG::save(trailTexture0.getImage(), "trailTextureC.png");

    //
    // render to screen
    //

    //app.renderer.clear();
    //trailTexture0.bindAsTexture();
    //app.renderer.render(database().getTexture(app.renderer, "Border"), rect2f(100.0f, 100.0f, 250.0f, 250.0f), 0.5f, vec4f(1.0f, 1.0f, 1.0f, 0.5f));

    trailTexture0.bindAsTexture();
    //app.renderer.renderGaussian(vec2f(0.0f, 1.0f) / (float)trailTexture0.dimensions().y);
    app.renderer.renderFullScreen(vec4f(1.0f, 1.0f, 1.0f, 1.0f));

    glEnable(GL_DEPTH_TEST);

    //if (GetAsyncKeyState(VK_F8))
    //{
    //    cout << "Saving trail textures..." << endl;
    //    LodePNG::save(trailTexture0.getImage(), "trailTexture0.png");
    //    LodePNG::save(trailTexture1.getImage(), "trailTexture1.png");
    //}
}

void GameCanvas::renderHoverComponent()
{
    if (app.ui.selectedMenuComponent == nullptr && app.ui.activePlacementBuffer.isEmpty())
        return;

    GameLocation location = app.ui.hoverLocation(true);

    if (!location.valid())
        return;

    if (location.inCircuit() && app.activeCircuit() == nullptr)
        return;

    vec2i buffermin = app.ui.activePlacementBuffer.boundingBox().min();

    // render the components
    for (ComponentDefiningProperties &c : app.ui.activePlacementBuffer.components)
    {
        GameLocation componentLocation;

        // move to the placement location
        if (location.inCircuit())
            componentLocation = GameLocation(location.circuitPos + c.location.boardPos - buffermin, c.location.circuitPos);
        else
            componentLocation = GameLocation(location.boardPos + c.location.boardPos - buffermin, c.location.circuitPos);

        // if the component is in a circuit, render at a lower depth
        float depth = c.location.inCircuit() ? depthLayers::hoverCircuitComponent : depthLayers::hoverComponent;
        //float depth = depthLayers::hoverComponent;

        const rect2f screenRect = GameUtil::locationInLocationToWindowRect(canonicalDims, componentLocation, location, 2);
        renderLocalizedComponentHover(c.baseInfo->name, screenRect, depth, IconState(c.modifiers, false, false));

        // render the green field
        if (!componentLocation.inCircuit() && c.baseInfo->name != "Eraser")
        {
            const vec2i coordBase = componentLocation.boardPos;
            const Board &board = location.inCircuit() ? *app.activeCircuit()->circuitBoard : app.state.board;

            for (int xOffset = 0; xOffset <= 1; xOffset++)
                for (int yOffset = 0; yOffset <= 1; yOffset++)
                {
                    const vec2i coord = coordBase + vec2i(xOffset, yOffset);
                    if (board.cells.coordValid(coord))
                    {
                        bool isBlocked = app.ui.canNotBuildAtPosition(board, c, coord);

                        Texture *tex = isBlocked ? database().squareBlocked : database().squareOpen;
                        const rect2f rect = location.inCircuit() ?
                            GameUtil::circuitToWindowRect(canonicalDims, coord, 1) :
                            GameUtil::boardToWindowRect(canonicalDims, coord, 1);
                        render(*tex, rect, depthLayers::hoverComponentGrid);
                    }
                }
        }
    }
}

void GameCanvas::renderMenuBackground(const string &menuName, const vec2f &canonicalStartCoord, const vec2i &gridDimensions, vec2f extension)
{
    const rect2f rect(canonicalStartCoord, canonicalStartCoord + params().menuButtonOffset + params().menuBorderOffset + vec2f::directProduct(vec2f(gridDimensions), vec2f((float)params().componentMenuCanonicalEntrySize) + extension));
    renderMenuBackground(menuName, canonicalStartCoord, rect);
}

void GameCanvas::renderMenuBackground(const string &menuName, const vec2f &canonicalStartCoord, const rect2f &rect)
{
    addBackgroundObject(database().getTexture(app.renderer, "MenuBackground"), GameUtil::canonicalToWindow(canonicalDims, rect), depthLayers::background);

    MenuTextEntry entry;
    entry.coord = canonicalStartCoord + params().menuTextOffset;
    entry.text = menuName;
    menuText.push_back(entry);
}

void GameCanvas::updateBackgroundObjects()
{
    backgroundObjects.clear();
    menuText.clear();

    app.controller.updateButtonList();

    //app.renderer.setRenderTarget(background);

    addBackgroundObject(database().getTexture(app.renderer, "Background"), rect2f(vec2f(0.0f, 0.0f), canonicalDims), depthLayers::background);

    if (app.controller.levelSelectMenu)
    {
        const vec2f start = params().componentMenuCanonicalStart;
        const vec2f size(260.0f, 250.0f);
        renderMenuBackground("Puzzle select", start, rect2f(start, start + size));
    }
    else
    {
        renderMenuBackground("Available components", params().componentMenuCanonicalStart, vec2i(7, 3), vec2f(1.65f, 0));

        const vec2f gameSpeedMenuStart = params().puzzleMenuBCanonicalStart - vec2f(5.0f, 15.0f);
        const rect2f gameSpeedRect(gameSpeedMenuStart, gameSpeedMenuStart + vec2i(110, 40));
        renderMenuBackground("Game speed", gameSpeedMenuStart, gameSpeedRect);

        if (app.controller.transformMenu)
        {
            const vec2f transformMenuStart = params().puzzleMenuDCanonicalStart - vec2f(5.0f, 15.0f);
            const rect2f transformRect(transformMenuStart, transformMenuStart + vec2i(90, 40));
            renderMenuBackground("Transform options", transformMenuStart, transformRect);
        }

        if (app.controller.affinityMenu) renderMenuBackground("Component affinity", params().affinityMenuCanonicalStart, vec2i(5, 1));
        if (app.controller.gateMenu) renderMenuBackground("Gate state", params().doorMenuCanonicalStart, vec2i(2, 1));
        if (app.controller.trapMenu) renderMenuBackground("Trap state", params().doorMenuCanonicalStart, vec2i(2, 1));
        if (app.controller.wireSpeedMenu) renderMenuBackground("Wire speed", params().typeMenuCanonicalStart, vec2i(5, 1));
        if (app.controller.colorMenu) renderMenuBackground("Component color", params().typeMenuCanonicalStart, vec2i(6, 1));
        if (app.controller.circuitBoundaryMenu) renderMenuBackground("Circuit boundary type", params().typeMenuCanonicalStart, vec2i(3, 1));

        if (app.activeCircuit() != nullptr)
        {
            const rect2f rect(params().circuitCanonicalStart - vec2f(params().circuitBackgroundCanonicalBoundarySize), params().circuitCanonicalStart + (float)constants::circuitBoardSize * params().canonicalCellSize + vec2f(params().circuitBackgroundCanonicalBoundarySize));
            addBackgroundObject(database().getTexture(app.renderer, "CircuitBackground"), GameUtil::canonicalToWindow(canonicalDims, rect), depthLayers::background);
        }
    }

    renderBuildingGrid();

    renderComponents();

    for (auto &component : app.state.components)
    {
        renderSpokes(*component);
    }

	vector<ComponentDefiningProperties> referenceComponents;
	if (app.ui.activePlacementBuffer.components.size() == 1)
		referenceComponents.push_back(app.ui.activePlacementBuffer.components[0]);
	else if (app.ui.selection.components.size() > 0)
		referenceComponents.push_back(ComponentDefiningProperties(*app.ui.selection.components[0]));

    for (auto &button : app.controller.buttons)
    {
        bool selected = false;
        selected |= (button.type == ButtonType::Component && app.ui.selectedMenuComponent == button.component && app.ui.selectedMenuComponentColor == button.modifiers.color);
        if (referenceComponents.size() > 0)
        {
			const ComponentModifiers &modifiers = referenceComponents[0].modifiers;
			const ComponentInfo &info = *referenceComponents[0].baseInfo;
            selected |= (button.type == ButtonType::ChargePreference && modifiers.chargePreference == button.modifiers.chargePreference);
            selected |= (button.type == ButtonType::ChargeColor && modifiers.color == button.modifiers.color);
            selected |= (button.type == ButtonType::WireSpeed && modifiers.speed == button.modifiers.speed);
            selected |= (button.type == ButtonType::CircuitBoundary && modifiers.boundary == button.modifiers.boundary);
            selected |= (button.type == ButtonType::TrapState && info.name == button.name);
            selected |= (button.type == ButtonType::GateState && info.name == button.name);
        }
        if (button.type == ButtonType::PuzzleControlA || button.type == ButtonType::PuzzleControlB || button.type == ButtonType::PuzzleControlC)
        {
            selected |= (button.name == buttonNameFromSpeed(app.controller.speed));
            selected |= (button.name == "ModePuzzle" && app.controller.editorMode == EditorMode::Campaign);
            selected |= (button.name == "ModeLevelEditor" && app.controller.editorMode == EditorMode::LevelEditor);
            selected |= (button.name == "Music" && app.audio.playMusic);
            selected |= (button.name == "SoundEffect" && app.audio.playSoundEffects);
        }
        renderButtonBackground(button, selected);
    }

    stable_sort(backgroundObjects.begin(), backgroundObjects.end());
}

void GameCanvas::renderBuildingGrid()
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

    Component *circuit = app.activeCircuit();
    if (circuit != nullptr)
    {
        for (const auto &cell : circuit->circuitBoard->cells)
        {
            const bool xBoundary = (cell.x == 0 || cell.x == 1 || cell.x == constants::circuitBoardSize - 1 || cell.x == constants::circuitBoardSize - 2);
            const bool yBoundary = (cell.y == 0 || cell.y == 1 || cell.y == constants::circuitBoardSize - 1 || cell.y == constants::circuitBoardSize - 2);
            const bool corner = xBoundary && yBoundary;
            const bool inactiveBoundary = (cell.value.c != nullptr && cell.value.c->circuitCorner());
            if (!corner && !inactiveBoundary) //if (!xBoundary && !yBoundary)
            {
                const rect2f screenRect = GameUtil::circuitToWindowRect(canonicalDims, vec2i(cell.x, cell.y), 1);
                addBackgroundObject(border, screenRect, depthLayers::background);
            }
        }
    }
}

void GameCanvas::renderButtonForeground(const GameButton &button, bool selected)
{
    if (button.type == ButtonType::ComponentAttribute)
    {
        renderText(getFontTexture(button.text, FontType::LevelSelectIndex), button.canonicalRect.min(), (float)button.canonicalRect.extentY());
    }
    if (button.type == ButtonType::LevelSelect)
    {
        renderText(getFontTexture(to_string(button.levelIndex + 1), FontType::LevelSelectIndex), button.canonicalRect.min() + vec2f(4.0f, 3.0f), 12.0f);
    }
}

void GameCanvas::renderTooltip()
{
    if (app.controller.tooltipErrorTitle.size() > 0)
    {
        renderTooltip(params().tooltipDefaultStart, app.controller.tooltipErrorTitle, app.controller.tooltipErrorDescription, ComponentModifiers(), "!", nullptr, true, false);
        return;
    }

    const GameButton *button = app.controller.getHitButton(app.ui.mouseHoverCoord);
    if (button != nullptr && button->tooltip != nullptr)
    {
        vec2f start = params().tooltipDefaultStart;
        bool transparent = false;
        
        if (button->type == ButtonType::PuzzleControlD)
        {
            if (app.activeCircuit() != nullptr)
            {
                transparent = true;
                start = button->canonicalRect.min() - vec2i(100, 140);
            }
        }
        else if (button->type == ButtonType::PuzzleControlA || button->type == ButtonType::PuzzleControlB || button->type == ButtonType::PuzzleControlC || button->type == ButtonType::PuzzleControlE || button->type == ButtonType::PuzzleControlF)
        {
            if (app.activeCircuit() != nullptr)
                return;
        }
        else
        {
            //start.y = button->canonicalRect.max().y + 5.0f;
            transparent = false;
        }

        renderTooltip(start, button->tooltip->semanticName, button->tooltip->description, button->modifiers, button->tooltipHotkey(), nullptr, false, transparent);
        //renderTooltip(params().tooltipDefaultStart, *button->tooltip, button->modifiers, button->hotkey, nullptr);
        return;
    }

    Component *hoverComponent = app.state.getComponent(app.ui.hoverLocation(false));
    Component *clickComponent = app.state.getComponent(app.ui.clickLocation);
    Component *selectedComponent = app.ui.selection.singleElement();
    if (clickComponent != nullptr && hoverComponent == clickComponent && hoverComponent == selectedComponent &&
        (clickComponent->modifiers.puzzleType == ComponentPuzzleType::PuzzlePiece || hoverComponent->info->name == "PowerSource" || hoverComponent->info->name == "MegaHold") &&
        clickComponent->info->name != "Blocker" && clickComponent->info->name != "Circuit" &&clickComponent->info->name != "Eraser" &&
        (!app.activeCircuit() || hoverComponent->info->name == "PowerSource" || hoverComponent->info->name == "MegaHold"))
    {
        if (clickComponent->inactiveCircuitMegaHold(app.state))
            clickComponent = &clickComponent->getSuperMegaHoldTarget(app.state);

        const ComponentInfo *info = clickComponent->info;
        if (clickComponent->modifiers.color == ChargeType::Gray && (clickComponent->info->name == "GateSwitch" || clickComponent->info->name == "TrapReset" || clickComponent->info->name == "MegaHold"))
            info = &database().getComponent(clickComponent->info->name + "GrayProxy");

        if (clickComponent->modifiers.speed != WireType::Standard)
            info = &database().getComponent(GameUtil::speedToTextureName(clickComponent->modifiers.speed));

        renderTooltip(params().tooltipDefaultStart, info->semanticName, info->description, clickComponent->modifiers, "", clickComponent, false, app.activeCircuit() != nullptr);
        return;
    }

    //
    // render level tip
    //
    if (app.activeCircuit() == nullptr)
    {
        //string solvedState;
        //auto sessionInfo = app.session.getLevelInfo(app.state.levelPack, app.state.levelPackPuzzleIndex);
        //if (sessionInfo != nullptr && sessionInfo->state == LevelState::Solved)
        //    solvedState = " (Solved)";
        renderTooltip(params().tooltipDefaultStart, app.controller.getActivePuzzle().name, app.controller.getActivePuzzle().tip, ComponentModifiers(), "!", nullptr, false, false);
    }
}

void GameCanvas::renderTooltip(const vec2f &canonicalStart, const string &title, const string &description, const ComponentModifiers &modifiers, const string &hotkey, const Component *component, bool error, bool transparent)
{
    auto splice = [&](const string &s) {
        string r = util::replace(s, "#", GameUtil::suffixFromCharge(modifiers.color));
        r = util::replace(r, "a Orange", "an Orange");
        return r;
    };

    Texture &tex = database().getTexture(app.renderer, error ? "TooltipError" : "Tooltip");

    if (!transparent) glDisable(GL_BLEND);
    
    const rect2f rect(canonicalStart, canonicalStart + params().tooltipSize);
    render(tex, rect, depthLayers::tooltip);

    if (!transparent) glEnable(GL_BLEND);

    renderText(getFontTexture(splice(title), FontType::TooltipName), canonicalStart + vec2f(15.0f, 9.0f), 18.0f);
    renderText(getFontTexture(splice(description), FontType::TooltipDescriptionA, 1050), canonicalStart + vec2f(15.0f, 30.0f), 12.0f);

    const float attributeSpacing = 17.0f;
    const float attributeBottom = 97.0f;

    if (hotkey.size() > 0 && hotkey[0] != '!')
    {
        float offset = hotkey.size() > 1 ? 22.0f : 0.0f;
        renderText(getFontTexture("Key", FontType::TooltipKeyA), canonicalStart + vec2f(210.0f - offset, attributeBottom), 12.0f);
        renderText(getFontTexture(hotkey, FontType::TooltipKeyB), canonicalStart + vec2f(230.0f - offset, attributeBottom), 12.0f);
    }

    if (component != nullptr)
    {
        if (component->info->name == "PowerSource")
        {
            if (component->intrinsics.totalCharges > 1)
                renderText(getFontTexture("Emits a charge every " + to_string(component->intrinsics.secondsPerEmission) + "s", FontType::TooltipDescriptionB), canonicalStart + vec2f(15.0f, attributeBottom - attributeSpacing * 2.0f), 12.0f);
            renderText(getFontTexture("First charge at " + to_string(component->intrinsics.secondsBeforeFirstEmission) + "s", FontType::TooltipDescriptionB), canonicalStart + vec2f(15.0f, attributeBottom - attributeSpacing), 12.0f);

            if (component->chargesRemaining >= 500)
                renderText(getFontTexture("Infinite charges", FontType::TooltipDescriptionB), canonicalStart + vec2f(15.0f, attributeBottom), 12.0f);
            else
                renderText(getFontTexture("Charges remaining: " + to_string(component->chargesRemaining) + " / " + to_string(component->intrinsics.totalCharges), FontType::TooltipDescriptionB), canonicalStart + vec2f(15.0f, attributeBottom), 12.0f);
        }
        if (component->info->name == "MegaHold")
        {
            double chargeLossPerSecond = (double)component->intrinsics.chargesLostPerDischarge / (double)component->intrinsics.ticksPerDischarge / (double)constants::secondsPerStep;
            renderText(getFontTexture("Loses " + util::formatDouble(chargeLossPerSecond, 2, true) + " charge per second", FontType::TooltipDescriptionB), canonicalStart + vec2f(15.0f, attributeBottom - attributeSpacing), 12.0f);
            renderText(getFontTexture("Charge level: " + to_string(component->megaHoldTotalCharge) + " / " + to_string(component->intrinsics.totalChargeRequired), FontType::TooltipDescriptionB), canonicalStart + vec2f(15.0f, attributeBottom), 12.0f);
        }
    }
}

void GameCanvas::renderVictoryPanel()
{
    auto formatSteps = [](int stepCount) {
        return util::formatDouble((double)stepCount / (double)constants::stepsPerSecond, 2, false) + "s";
    };

    auto sessionInfo = app.session.getLevelInfo(app.state.levelPack, app.state.levelPackPuzzleIndex);
    const PuzzleInfo &curPuzzle = app.controller.getActivePuzzle();
    if (sessionInfo == nullptr || sessionInfo->state != LevelState::Solved)
        return;

    Texture &tex = database().getTexture(app.renderer, "VictoryPanel");

    glDisable(GL_BLEND);

    const vec2i victoryPanelStart(403, 370);
    const vec2i victoryPanelSize(254, 82);

    const vec2i tableStart = victoryPanelStart + vec2f(15.0f, 31.0f);
    const int tableHeight = 15;
    const int row0 = tableStart.y + tableHeight * 0;
    const int row1 = tableStart.y + tableHeight * 1;
    const int row2 = tableStart.y + tableHeight * 2;

    const int col0 = tableStart.x;
    const int col1 = col0 + 80;
    const int col2 = col1 + 55;
    const int col3 = col2 + 55;

    const rect2f rect(victoryPanelStart, victoryPanelStart + victoryPanelSize);
    render(tex, rect, depthLayers::tooltip);

    glEnable(GL_BLEND);

    renderText(getFontTexture("Puzzle solved!", FontType::TooltipName), victoryPanelStart + vec2f(15.0f, 9.0f), 18.0f);

    renderText(getFontTexture("Your score", FontType::TooltipDescriptionA), vec2i(col1, row0), 12.0f);
    renderText(getFontTexture("Your best", FontType::TooltipDescriptionA), vec2i(col2, row0), 12.0f);
    renderText(getFontTexture("Par", FontType::TooltipDescriptionA), vec2i(col3, row0), 12.0f);

    renderText(getFontTexture("Solve time", FontType::TooltipDescriptionA), vec2i(col0, row1), 12.0f);
    renderText(getFontTexture("Component cost", FontType::TooltipDescriptionA), vec2i(col0, row2), 12.0f);

	renderText(getFontTexture(formatSteps(sessionInfo->recentStepCount), FontType::TooltipDescriptionA), vec2i(col1, row1), 12.0f);
	renderText(getFontTexture(to_string(sessionInfo->recentComponentCost - curPuzzle.baseComponentCost), FontType::TooltipDescriptionA), vec2i(col1, row2), 12.0f);

    renderText(getFontTexture(formatSteps(sessionInfo->bestStepCount), FontType::TooltipDescriptionA), vec2i(col2, row1), 12.0f);
    renderText(getFontTexture(to_string(sessionInfo->bestComponentCost - curPuzzle.baseComponentCost), FontType::TooltipDescriptionA), vec2i(col2, row2), 12.0f);

	renderText(getFontTexture(formatSteps(curPuzzle.stepCountPar), FontType::TooltipDescriptionA), vec2i(col3, row1), 12.0f);
	renderText(getFontTexture(to_string(curPuzzle.componentCostPar - curPuzzle.baseComponentCost), FontType::TooltipDescriptionA), vec2i(col3, row2), 12.0f);
}

void GameCanvas::renderButtonBackground(const GameButton &button, bool selected)
{
    if (button.type != ButtonType::ComponentAttribute)
    {
        Texture &borderTex = database().getTexture(app.renderer, "ButtonBackground");

        const rect2f screenRect = GameUtil::canonicalToWindow(GameUtil::getCanonicalSize(), button.canonicalRect);
        addBackgroundObject(borderTex, screenRect, depthLayers::background);


        if (button.type == ButtonType::LevelSelect)
        {
            string suffix;
            auto info = app.session.getLevelInfo("Campaign", button.levelIndex);
            if (button.levelIndex > app.session.highestAccessiblePuzzle())
                suffix = "Inaccessible";
            else if (info->state == LevelState::Solved)
                suffix = "Solved";
            else if (info->state == LevelState::Unsolved)
                suffix = "Accessible";
            renderLocalizedComponent("ChoosePuzzle" + suffix, nullptr, screenRect, 0.0f, IconState(button.modifiers, false));
            return;
        }

        //if (levelIndex > app.session.highestAccessiblePuzzle())
        //if (info->state == LevelState::Solved)


        renderLocalizedComponent(button.name, nullptr, screenRect, 0.0f, IconState(button.modifiers, selected));

        if (app.controller.editorMode == EditorMode::LevelEditor &&
            app.state.buildableComponents.canBuild(button.name, button.modifiers) &&
            (button.type == ButtonType::Component || button.type == ButtonType::ChargeColor || button.type == ButtonType::WireSpeed))
        {
            Texture &constructionTex = database().getTexture(app.renderer, "Construction");
            const rect2f screenRect = GameUtil::canonicalToWindow(GameUtil::getCanonicalSize(), button.canonicalRect);
            addBackgroundObject(constructionTex, screenRect, depthLayers::component);
        }
    }
}

void GameCanvas::renderLocalizedComponentHover(const string &name, const rect2f &screenRect, float depth, const IconState &icon)
{
    if (icon.modifiers.boundary == CircuitBoundaryType::Closed)
        return;

    Texture &baseTex = database().getTexture(app.renderer, constants::useSmallWire && name == "Wire" ? "PureWire" : "WireBase");
    Texture &componentTex = database().getTexture(app.renderer, name, icon.modifiers);
    Texture &preferenceTex = *database().preferenceTextures[icon.modifiers.chargePreference];

    render(preferenceTex, screenRect, depth, Colors::White());

    if (name != "Blocker" && name != "Eraser")
        render(baseTex, screenRect, depth, Colors::White());

    render(componentTex, screenRect, depth, Colors::White());

    if (database().hasComponent(name) && database().getComponent(name).hasStoredChargeLayer)
    {
        Texture &chargeLayerTex = database().getTexture(app.renderer, name, icon.modifiers, true);
        render(chargeLayerTex, screenRect, depth, GameUtil::chargeColor(ChargeType::Gray));
    }
}

void GameCanvas::renderLocalizedComponent(const string &name, const Component *dynamicComponent, const rect2f &screenRect, float depthOffset, const IconState &icon)
{
	//Texture &baseTex = database().getTexture(app.renderer, constants::useSmallWire && name == "Wire" ? "PureWire" : "WireBase");

	string baseTexName = "WireBase";
	if (icon.orientation != CircuitBoundaryOrientation::None)
	{
		if (icon.orientation == CircuitBoundaryOrientation::Left)   baseTexName = "CircuitBoundaryLeft";
		if (icon.orientation == CircuitBoundaryOrientation::Right)  baseTexName = "CircuitBoundaryRight";
		if (icon.orientation == CircuitBoundaryOrientation::Top)    baseTexName = "CircuitBoundaryTop";
		if (icon.orientation == CircuitBoundaryOrientation::Bottom) baseTexName = "CircuitBoundaryBottom";
	}

    Texture &baseTex = database().getTexture(app.renderer, baseTexName);
    Texture &componentTex = database().getTexture(app.renderer, name, icon.modifiers);
    Texture &preferenceTex = *database().preferenceTextures[icon.modifiers.chargePreference];

    auto record = [&](Texture &tex, const rect2f &rect, float depth, UIRenderType type, const vec4f &color, const Component *component) {
        depth -= depthOffset;
        if (icon.background)
            addBackgroundObject(tex, rect, depth, type, color, component);
        else
            render(tex, rect, depth, color);
    };

    record(preferenceTex, screenRect, 1.0f, UIRenderType::Standard, Colors::White(), nullptr);

    if (name != "Blocker" && name != "Eraser" && icon.modifiers.boundary != CircuitBoundaryType::Closed && name != "CloseAll")
        record(baseTex, screenRect, 1.0f, UIRenderType::Standard, Colors::White(), nullptr);

    record(componentTex, screenRect, depthLayers::component, UIRenderType::Standard, Colors::White(), dynamicComponent);

    if (database().hasComponent(name) && database().getComponent(name).hasStoredChargeLayer)
    {
        Texture &chargeLayerTex = database().getTexture(app.renderer, name, icon.modifiers, true);
        record(chargeLayerTex, screenRect, depthLayers::component, UIRenderType::StoredCharge, GameUtil::chargeColor(ChargeType::Gray), dynamicComponent);
    }

    if (icon.selected)
    {
        Texture *selectionTex = &database().getTexture(app.renderer, "Selector");

        if (dynamicComponent != nullptr)
        {
            if (dynamicComponent->modifiers.puzzleType == ComponentPuzzleType::PuzzlePiece)
                selectionTex = &database().getTexture(app.renderer, "PuzzleSelector");

            if (dynamicComponent->modifiers.puzzleType == ComponentPuzzleType::CopiedCircuit)
                selectionTex = &database().getTexture(app.renderer, "CircuitSelector");
        }

        record(*selectionTex, screenRect, depthLayers::selection, UIRenderType::Standard, Colors::White(), nullptr);
    }

    if (icon.faded)
    {
        Texture &fadedTex = database().getTexture(app.renderer, "Faded");
        record(fadedTex, screenRect, depthLayers::selection, UIRenderType::Standard, Colors::White(), nullptr);
    }
}

void GameCanvas::renderComponent(const Component &component)
{
    const bool selected = app.ui.selection.isIn(&component) || (app.ui.selection.selectionIsInCircuit && app.ui.selection.circuitLocation == component.location.boardPos && !component.location.inCircuit());

    if (component.info->name == "Blocker" &&
        (app.ui.selectedMenuComponent == nullptr || app.ui.selectedMenuComponent->name != "Blocker"))
        return;

    if (component.inactiveCircuitMegaHold(app.state))
        return;

    if (component.location.inCircuit())
    {
        // if the component is in the active circuit, render it in the selected circuit area
        if (app.activeCircuit() != nullptr && component.location.boardPos == app.activeCircuit()->location.boardPos && !component.circuitCorner())
        {
			const CircuitBoundaryOrientation orientation = component.location.getCircuitBoundaryOrientation();
			const rect2f screenRect = GameUtil::circuitToWindowRect(canonicalDims, component.location.circuitPos, 2);
            const bool faded = component.inactiveBoundary();
            renderLocalizedComponent(component.info->name, &component, screenRect, 0.0f, IconState(component.modifiers, selected, true, faded, orientation));
        }
        // regardless, we'll need to render it in the main grid, but we'll wait until later
    }
    else
    {
        const rect2f screenRect = GameUtil::boardToWindowRect(canonicalDims, component.location.boardPos, 2);
        renderLocalizedComponent(component.info->name, &component, screenRect, 0.0f, IconState(component.modifiers, selected));
    }
}

void GameCanvas::renderCircuitComponent(const Component &component)
{
    if (!component.location.inCircuit() ||
        component.inactiveBoundary() || component.modifiers.boundary == CircuitBoundaryType::Closed ||
        component.inactiveCircuitMegaHold(app.state))
        return;

    const CoordinateFrame frame = CoordinateFrame(component.location.boardPos, component.location.boardPos + vec2f(2.0f, 2.0f), vec2i(constants::circuitBoardSize, constants::circuitBoardSize));
    const rect2f circuitRect = rect2f(component.location.circuitPos, component.location.circuitPos + 2);
    const rect2f screenRect = params().boardInWindow.toContainer(frame.toContainer(circuitRect));
	
	const CircuitBoundaryOrientation orientation = component.location.getCircuitBoundaryOrientation();

	renderLocalizedComponent(component.info->name, &component, screenRect, depthLayers::miniCircuitOffsetStandard, IconState(component.modifiers, false, true, false, orientation));
}

void GameCanvas::renderSpokesMiniCircuit(const Component &component)
{
    if (component.modifiers.boundary == CircuitBoundaryType::Closed)
        return;

    const vec2f myScreenPos = component.location.toScreenCoordMainBoard(canonicalDims);

    const auto &cells = app.state.getCircuit(component.location).circuitBoard->cells;

    for (const CanvasConnection &connection : connectionClasses)
    {
        vec2i otherLocation = component.location.circuitPos + connection.offset;

        if (cells.coordValid(otherLocation) && cells(otherLocation).c != nullptr)
        {
            const Component &otherComponent = *cells(otherLocation).c;
            bool renderSpokes = true;
            renderSpokes &= otherComponent.location.circuitPos == otherLocation;
            renderSpokes &= otherComponent.modifiers.boundary != CircuitBoundaryType::Closed;
            renderSpokes &= otherComponent.hasSpokes(app.state);
            renderSpokes &= !(component.info->name == "CircuitBoundary" && otherComponent.info->name == "CircuitBoundary");
            if (renderSpokes)
            {
                const vec2f otherScreenPos = otherComponent.location.toScreenCoordMainBoard(canonicalDims);

                const vec2f middle = (myScreenPos + otherScreenPos) * 0.5f;
                const vec2f diff = myScreenPos - otherScreenPos;
                const float dist = diff.length();

                vec2f variance = constants::connectorDims[connection.type + 1] * dist;
                variance.y *= 2.0f;

                const float angle = math::radiansToDegrees(atan2f(diff.y, diff.x)) + 180.0f;
                addBackgroundObject(*connection.tex, rect2f(middle - variance, middle + variance), depthLayers::spokes - depthLayers::miniCircuitOffsetStandard, angle);
            }
        }
    }
}

void GameCanvas::renderSpokes(const Component &component)
{
    if (!component.hasSpokes(app.state))
        return;

    if (component.location.inCircuit())
    {
        renderSpokesCircuit(component);
        renderSpokesMiniCircuit(component);
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
                if (connection.component != nullptr && connection.component->location.boardPos == circuit.location.boardPos && connection.component->modifiers.boundary == CircuitBoundaryType::Open)
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

    for (const CanvasConnection &connection : connectionClasses)
    {
        vec2i otherLocation = component.location.boardPos + connection.offset;

        if (cells.coordValid(otherLocation) && cells(otherLocation).c != nullptr)
        {
            const Component &otherComponent = *cells(otherLocation).c;

            bool circuitTest = drawCircuitConnection(component, otherComponent);

            if (otherComponent.location.boardPos == otherLocation && otherComponent.hasSpokes(app.state) && circuitTest)
            {
                const vec2f otherScreenPos = GameUtil::boardToWindow(canonicalDims, otherComponent.location.boardPos + vec2i(1, 1));

                const vec2f middle = (myScreenPos + otherScreenPos) * 0.5f;
                const vec2f diff = myScreenPos - otherScreenPos;
                const float dist = diff.length();

                const vec2f variance = constants::connectorDims[connection.type + 1] * dist;

                const float angle = math::radiansToDegrees(atan2f(diff.y, diff.x)) + 180.0f;
                addBackgroundObject(*connection.tex, rect2f(middle - variance, middle + variance), depthLayers::spokes, angle);
            }
        }
    }
}

void GameCanvas::renderSpokesCircuit(const Component &component)
{
    if (app.activeCircuit() == nullptr ||
        app.activeCircuit()->location.boardPos != component.location.boardPos ||
        component.modifiers.boundary == CircuitBoundaryType::Closed)
        return;

    const vec2f myScreenPos = GameUtil::circuitToWindow(canonicalDims, component.location.circuitPos + vec2i(1, 1));

    const auto &cells = app.activeCircuit()->circuitBoard->cells;

    for (const CanvasConnection &connection : connectionClasses)
    {
        vec2i otherLocation = component.location.circuitPos + connection.offset;

        if (cells.coordValid(otherLocation) && cells(otherLocation).c != nullptr)
        {
            const Component &otherComponent = *cells(otherLocation).c;
            bool renderSpokes = true;
            renderSpokes &= otherComponent.location.circuitPos == otherLocation;
            renderSpokes &= otherComponent.modifiers.boundary != CircuitBoundaryType::Closed;
            renderSpokes &= otherComponent.hasSpokes(app.state);
            renderSpokes &= !(component.info->name == "CircuitBoundary" && otherComponent.info->name == "CircuitBoundary");
            if (renderSpokes)
            {
                const vec2f otherScreenPos = GameUtil::circuitToWindow(canonicalDims, otherComponent.location.circuitPos + vec2i(1, 1));

                const vec2f middle = (myScreenPos + otherScreenPos) * 0.5f;
                const vec2f diff = myScreenPos - otherScreenPos;
                const float dist = diff.length();

                const vec2f variance = constants::connectorDims[connection.type + 1] * dist;

                const float angle = math::radiansToDegrees(atan2f(diff.y, diff.x)) + 180.0f;
                addBackgroundObject(*connection.tex, rect2f(middle - variance, middle + variance), depthLayers::spokes, angle);
            }
        }
    }
}

void GameCanvas::renderComponents()
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

void GameCanvas::renderCharge(const Charge &charge, bool trailRender)
{
    renderChargeCircuit(charge, trailRender);
    pair<vec2f, float> screen = GameUtil::computeChargeScreenPos(charge.source, charge.destination, charge.interpolation(), charge.level, canonicalDims);

    if (trailRender)
        screen.second *= constants::trailExpansionFactor;

    Component *component = app.state.getComponent(charge.destination);
    if (component != nullptr && component->heldCharge == charge.level && component->info->name == "ChargeGoal")
        screen.second *= app.state.victoryChargeScaleFactorPositive;
    else
        screen.second *= app.state.victoryChargeScaleFactorNegative;

    const float angle = charge.randomRotationOffset + app.state.globalRotationOffset;
    const rect2f destinationRect(screen.first - vec2f(screen.second), screen.first + vec2f(screen.second));

    render(*database().chargeTextures[(int)charge.level], destinationRect, depthLayers::charge, angle);
}

void GameCanvas::renderMegaHold(const Component &component, bool trailRender)
{
    renderMegaHoldCircuit(component, trailRender);

    if (component.inactiveCircuitMegaHold(app.state) && !component.isSuperMegaHold(app.state)) return;

    GameLocation location = component.location;
    float screenSize = ((float)component.megaHoldTotalCharge / (float)component.intrinsics.totalChargeRequired) * GameUtil::windowScaleFactor(canonicalDims) * constants::megaHoldCanonicalSize;

    if (component.isSuperMegaHold(app.state))
    {
        location = GameLocation(component.location.boardPos, vec2i(constants::circuitBoardSize / 2 - 1));
        screenSize *= 1.2f;
    }
    else if (component.location.inCircuit())
        screenSize *= 0.35f;

    if (trailRender)
        screenSize *= 1.2f;

    const vec2f screenCenter = location.toScreenCoordMainBoard(canonicalDims);

    const float angle = component.randomRotationOffset + app.state.globalRotationOffset;
    const rect2f destinationRect(screenCenter - vec2f(screenSize), screenCenter + vec2f(screenSize));

    render(*database().chargeTextures[(int)component.modifiers.color], destinationRect, depthLayers::charge, angle);
}

void GameCanvas::renderMegaHoldCircuit(const Component &component, bool trailRender)
{
    if (!component.location.inCircuit()) return;
    if (app.activeCircuit() == nullptr || component.location.boardPos != app.activeCircuit()->location.boardPos) return;
    if (component.inactiveCircuitMegaHold(app.state) && !component.isSuperMegaHold(app.state)) return;

    const float angle = component.randomRotationOffset + app.state.globalRotationOffset;

    vec2f screenCenter;
    float screenSize;
    if (component.isSuperMegaHold(app.state))
    {
        const GameLocation circuitMiddle(component.location.boardPos, vec2i(constants::circuitBoardSize / 2 - 1));
        screenCenter = circuitMiddle.toScreenCoordCircuitBoard(canonicalDims);
        screenSize = ((float)component.megaHoldTotalCharge / (float)component.intrinsics.totalChargeRequired) * GameUtil::windowScaleFactor(canonicalDims) * constants::superMegaHoldCanonicalSize;
    }
    else
    {
        screenCenter = component.location.toScreenCoordCircuitBoard(canonicalDims);
        screenSize = ((float)component.megaHoldTotalCharge / (float)component.intrinsics.totalChargeRequired) * GameUtil::windowScaleFactor(canonicalDims) * constants::megaHoldCanonicalSize;
    }
    if (trailRender)
        screenSize *= 1.2f;
    const rect2f destinationRect(screenCenter - vec2f(screenSize), screenCenter + vec2f(screenSize));
    render(*database().chargeTextures[(int)component.modifiers.color], destinationRect, depthLayers::charge, angle);
}

void GameCanvas::renderSuperMegaHold(const Component &component)
{
    Texture &tex = database().getTexture(app.renderer, "SuperMegaHold" + GameUtil::suffixFromCharge(component.modifiers.color));

    const CoordinateFrame frame = CoordinateFrame(component.location.boardPos, component.location.boardPos + vec2f(2.0f, 2.0f), vec2i(constants::circuitBoardSize));

    const rect2f miniCircuitRect = rect2f(vec2i(2, 2), vec2i(constants::circuitBoardSize - 2));
    const rect2f miniCircuitScreenRect = params().boardInWindow.toContainer(frame.toContainer(miniCircuitRect));

    render(tex, miniCircuitScreenRect, depthLayers::superMegaHold);

    if (app.activeCircuit() == nullptr || component.location.boardPos != app.activeCircuit()->location.boardPos) return;

    const rect2f rect(params().circuitCanonicalStart + float(params().canonicalCellSize) * vec2f(2.0f),
                      params().circuitCanonicalStart + float(params().canonicalCellSize) * vec2f(constants::circuitBoardSize - 2.0f));

    render(tex, rect, depthLayers::superMegaHold);
}

void GameCanvas::renderExplodingCharge(const ExplodingCharge &charge)
{
    renderExplodingChargeCircuit(charge);
    const pair<vec2f, float> screen = GameUtil::computeChargeScreenPos(charge.locationA, charge.locationB, charge.interpolation, charge.level, canonicalDims);
    const float angle = charge.baseRotationOffset + (app.state.stepCount - charge.birthTick) * constants::secondsPerStep * constants::chargeRotationsPerSecond * 360.0f * constants::explodingChargeRotationFactor;
    const float scale = screen.second * math::lerp(1.0f, 3.0f, charge.percentDone());

    const rect2f destinationRect(screen.first - vec2f(scale), screen.first + vec2f(scale));

    const vec4f color(1.0f, 1.0f, 1.0f, 1.0f - charge.percentDone() * charge.percentDone());

    render(*database().chargeTextures[(int)charge.level], destinationRect, depthLayers::charge, angle, color);
}

void GameCanvas::renderChargeCircuit(const Charge &charge, bool trailRender)
{
    if (!charge.source.inCircuit() || !charge.destination.inCircuit()) return;
    if (charge.source.boardPos != charge.destination.boardPos) return;
    if (app.activeCircuit() == nullptr || charge.source.boardPos != app.activeCircuit()->location.boardPos) return;

    pair<vec2f, float> screen = GameUtil::computeChargeScreenPosCircuit(charge.source, charge.destination, charge.interpolation(), charge.level, canonicalDims);
    const float angle = charge.randomRotationOffset + app.state.globalRotationOffset;

    if (trailRender)
        screen.second *= constants::trailExpansionFactor;

    Component *component = app.state.getComponent(charge.destination);
    if (component != nullptr && component->heldCharge == charge.level && component->info->name == "ChargeGoal")
        screen.second *= app.state.victoryChargeScaleFactorPositive;
    else
        screen.second *= app.state.victoryChargeScaleFactorNegative;

    const rect2f destinationRect(screen.first - vec2f(screen.second), screen.first + vec2f(screen.second));
    render(*database().chargeTextures[(int)charge.level], destinationRect, depthLayers::charge, angle);
}

void GameCanvas::renderExplodingChargeCircuit(const ExplodingCharge &charge)
{
    if (!charge.locationA.inCircuit() || !charge.locationB.inCircuit()) return;
    if (app.activeCircuit() == nullptr || charge.locationA.boardPos != app.activeCircuit()->location.boardPos) return;

    const pair<vec2f, float> screen = GameUtil::computeChargeScreenPosCircuit(charge.locationA, charge.locationB, charge.interpolation, charge.level, canonicalDims);
    const float angle = charge.baseRotationOffset + (app.state.stepCount - charge.birthTick) * constants::secondsPerStep * constants::chargeRotationsPerSecond * 360.0f * constants::explodingChargeRotationFactor;
    const float scale = screen.second * math::lerp(1.0f, 3.0f, charge.percentDone());
    const rect2f destinationRect(screen.first - vec2f(scale), screen.first + vec2f(scale));

    const vec4f color(1.0f, 1.0f, 1.0f, 1.0f - charge.percentDone() * charge.percentDone());

    render(*database().chargeTextures[(int)charge.level], destinationRect, depthLayers::charge, angle, color);
}
