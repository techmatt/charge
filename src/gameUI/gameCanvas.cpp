
#include "main.h"

void GameCanvas::init()
{
    backgroundDirty = true;
}

Texture& GameCanvas::getFontTexture(const string &text, FontType font, int wrapWidth)
{
    if (textCache.count(text) == 0)
    {
        auto &info = database().fonts[(int)font];
        Texture *t = new Texture(app.renderer.getFont(info.name), text, info.color, wrapWidth);
        textCache[text] = t;
    }
    return *textCache[text];
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

void GameCanvas::renderText(Texture &tex, const vec2f &start, const float lineHeight, const vec4f &color)
{
    const float aspect = (float)tex.bmp().dimX() / (float)tex.bmp().dimY();

    // TODO: the 58 here is a guess
    const float lineCount = tex.bmp().dimY() / 58.0f;
    const float height = lineCount * lineHeight;

    const float width = aspect * height;

    const rect2f rect(start, vec2f(start.x + width, start.y + height));
    app.renderer.render(tex, coordinateFrame.toContainer(rect), depthLayers::font, color);
}

void GameCanvas::render()
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

    for (const auto &button : app.controller.buttons)
    {
        renderButtonForeground(button, false);
    }

    if (app.controller.speed <= GameSpeed::x3)
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
            renderMegaHold(*component, false);
    }

    renderHoverComponent();

    renderText(getFontTexture(app.state.name, FontType::LevelName), vec2f(1.0f, 1.0f), 20.0f);

    renderTooltip();
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

    //glClear(GL_COLOR_BUFFER_BIT);
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

    trailTexture0.unbindRenderTarget();

    //LodePNG::save(trailTexture0.getImage(), "trailTextureA.png");

    //
    // blur in X to trailTexture1
    //
    trailTexture1.bindAsRenderTarget();

    glDisable(GL_BLEND);
    trailTexture0.bindAsTexture();

    glClear(GL_COLOR_BUFFER_BIT);
    app.renderer.renderGaussian(vec2f(1.0f, 0.0f) / (float)trailTexture0.dimensions().x);

    trailTexture1.unbindRenderTarget();

    //LodePNG::save(trailTexture1.getImage(), "trailTextureB.png");

    //
    // blur in Y back to trailTexture0
    //
    trailTexture0.bindAsRenderTarget();

    trailTexture1.bindAsTexture();

    //glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    //glClear(GL_COLOR_BUFFER_BIT);
    app.renderer.renderGaussian(vec2f(0.0f, 1.0f) / (float)trailTexture0.dimensions().y);

    glEnable(GL_BLEND);
    trailTexture0.unbindRenderTarget();

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

    //glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
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
    for (ComponentDefiningProperties c : app.ui.activePlacementBuffer.components)
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
        renderLocalizedComponent(c.baseInfo->name, nullptr, screenRect, depth, IconState(c.modifiers, false, false));



        // render the green field
        if (!componentLocation.inCircuit())
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

void GameCanvas::updateBackgroundObjects()
{
    backgroundObjects.clear();

    app.controller.updateButtonList();

    //Component *gameComponent = app.state.getComponent(selectedGameLocation);
    Component *gameComponent = app.ui.selection.singleElement();

    //app.renderer.setRenderTarget(background);

    addBackgroundObject(database().getTexture(app.renderer, "Background"), rect2f(vec2f(0.0f, 0.0f), canonicalDims), depthLayers::background);

    if (app.activeCircuit() != nullptr)
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

    for (auto &button : app.controller.buttons)
    {
        bool selected = false;
        selected |= (button.type == ButtonType::Component && app.ui.selectedMenuComponent == button.component && app.ui.selectedMenuComponentColor == button.modifiers.color);
        if (gameComponent != nullptr)
        {
            selected |= (button.type == ButtonType::ChargePreference && gameComponent->modifiers.chargePreference == button.modifiers.chargePreference);
            selected |= (button.type == ButtonType::ChargeColor && gameComponent->modifiers.color == button.modifiers.color);
            selected |= (button.type == ButtonType::WireSpeed && gameComponent->modifiers.speed == button.modifiers.speed);
            selected |= (button.type == ButtonType::CircuitBoundary && gameComponent->modifiers.boundary == button.modifiers.boundary);
            selected |= (button.type == ButtonType::TrapState && gameComponent->info->name == button.name);
            selected |= (button.type == ButtonType::GateState && gameComponent->info->name == button.name);
        }
        if (button.type == ButtonType::PuzzleControl)
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
        renderText(getFontTexture(button.text, FontType::LevelName), button.canonicalRect.min(), (float)button.canonicalRect.extentY());
    }
}

void GameCanvas::renderTooltip()
{
    const GameButton *hitButton = app.controller.getHitButton(app.ui.mouseHoverCoord);
    if (hitButton != nullptr && 
        (hitButton->type == ButtonType::Component || hitButton->type == ButtonType::ChargeColor || hitButton->type == ButtonType::ChargePreference ||
        hitButton->type == ButtonType::CircuitBoundary || hitButton->type == ButtonType::GateState || hitButton->type == ButtonType::TrapState ||
        hitButton->type == ButtonType::WireSpeed))
    {
        float startY = hitButton->canonicalRect.max().y + 5.0f;

        const ComponentInfo *info = hitButton->component;

        if (hitButton->type == ButtonType::ChargePreference)
            info = &database().getComponent("Preference" + to_string((int)hitButton->modifiers.chargePreference));

        if (hitButton->type == ButtonType::WireSpeed)
            info = &database().getComponent(GameUtil::speedToTextureName(hitButton->modifiers.speed));

        if (hitButton->modifiers.color == ChargeType::Gray && (hitButton->component->name == "GateSwitch" || hitButton->component->name == "TrapReset" || hitButton->component->name == "MegaHold"))
            info = &database().getComponent(hitButton->component->name + "GrayProxy");

        renderTooltip(vec2f(params().tooltipDefaultStart.x, startY), *info, hitButton->modifiers, nullptr);
        return;
    }

    Component *hoverComponent = app.state.getComponent(app.ui.hoverLocation(false));
    Component *clickComponent = app.state.getComponent(app.ui.clickLocation);
    Component *selectedComponent = app.ui.selection.singleElement();
    if (app.activeCircuit() == nullptr &&
        clickComponent != nullptr && hoverComponent == clickComponent && hoverComponent == selectedComponent &&
        clickComponent->modifiers.puzzleType == ComponentPuzzleType::PuzzlePiece &&
        clickComponent->info->name != "Blocker" && clickComponent->info->name != "Circuit")
    {
        renderTooltip(params().tooltipDefaultStart, *clickComponent->baseInfo, clickComponent->modifiers, clickComponent);
    }
}

void GameCanvas::renderTooltip(const vec2f &canonicalStart, const ComponentInfo &info, const ComponentModifiers &modifiers, const Component *component)
{
    auto splice = [&](const string &s) {
        string r = util::replace(s, "#", GameUtil::suffixFromCharge(modifiers.color));
        r = util::replace(r, "a Orange", "an Orange");
        return r;
    };

    Texture &tex = database().getTexture(app.renderer, "Tooltip");
    const rect2f rect(canonicalStart, canonicalStart + params().tooltipSize);
    render(tex, rect, depthLayers::tooltip);

    renderText(getFontTexture(splice(info.semanticName), FontType::TooltipName), canonicalStart + vec2f(15.0f, 9.0f), 18.0f);
    renderText(getFontTexture(splice(info.description), FontType::TooltipDescription, 1050), canonicalStart + vec2f(15.0f, 30.0f), 12.0f);

    if (component != nullptr)
    {
        if (component->info->name == "MegaHold")
        {
            double chargeLossPerSecond = (double)component->intrinsics.chargesLostPerDischarge / (double)component->intrinsics.ticksPerDischarge / (double)constants::secondsPerStep;
            renderText(getFontTexture("Loses " + util::formatDouble(chargeLossPerSecond, 2) + " charge per second", FontType::TooltipDescription), canonicalStart + vec2f(15.0f, 77.0f), 12.0f);
            renderText(getFontTexture("Charge level: " + to_string(component->megaHoldTotalCharge) + " / " + to_string(component->intrinsics.totalChargeRequired), FontType::TooltipDescription), canonicalStart + vec2f(15.0f, 97.0f), 12.0f);
        }
    }
}

void GameCanvas::renderButtonBackground(const GameButton &button, bool selected)
{
    if (button.type != ButtonType::ComponentAttribute)
    {
        Texture &borderTex = database().getTexture(app.renderer, "Border");

        const rect2f screenRect = GameUtil::canonicalToWindow(GameUtil::getCanonicalSize(), button.canonicalRect);
        addBackgroundObject(borderTex, screenRect, depthLayers::background);

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

void GameCanvas::renderLocalizedComponent(const string &name, const Component *dynamicComponent, const rect2f &screenRect, float depthOffset, const IconState &icon)
{
    /*if (!isButton && name == "Blocker" &&
    (app.ui.selectedMenuComponent == nullptr || app.ui.selectedMenuComponent->name != "Blocker"))
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

    record(preferenceTex, screenRect, 1.0f, UIRenderType::Standard, Colors::White(), nullptr);

    if (name != "Blocker" && icon.modifiers.boundary != CircuitBoundaryType::Closed)
        record(baseTex, screenRect, 1.0f, UIRenderType::Standard, Colors::White(), nullptr);

    record(componentTex, screenRect, depthLayers::component, UIRenderType::Standard, Colors::White(), dynamicComponent);

    if (database().hasComponent(name) && database().getComponent(name).hasStoredChargeLayer)
    {
        Texture &chargeLayerTex = database().getTexture(app.renderer, name, icon.modifiers, true);
        record(chargeLayerTex, screenRect, depthLayers::component, UIRenderType::StoredCharge, GameUtil::chargeColor(ChargeType::Gray), dynamicComponent);
    }

    if (icon.selected)
    {
        bool usePuzzleSelector = (dynamicComponent != nullptr && !app.controller.canEdit(*dynamicComponent));

        Texture &selectionTex = usePuzzleSelector ? database().getTexture(app.renderer, "PuzzleSelector") : database().getTexture(app.renderer, "Selector");
        record(selectionTex, screenRect, depthLayers::selection, UIRenderType::Standard, Colors::White(), nullptr);
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

    if (component.location.inCircuit())
    {
        // if the component is in the active circuit, render it in the selected circuit area
        if (app.activeCircuit() != nullptr && component.location.boardPos == app.activeCircuit()->location.boardPos && !component.circuitCorner())
        {
            const rect2f screenRect = GameUtil::circuitToWindowRect(canonicalDims, component.location.circuitPos, 2);
            const bool faded = component.inactiveBoundary();
            renderLocalizedComponent(component.info->name, &component, screenRect, 0.0f, IconState(component.modifiers, selected, true, faded));
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
    if (!component.location.inCircuit() || component.inactiveBoundary() || component.modifiers.boundary == CircuitBoundaryType::Closed)
        return;
    const CoordinateFrame frame = CoordinateFrame(component.location.boardPos, component.location.boardPos + vec2f(2.0f, 2.0f), vec2i(constants::circuitBoardSize, constants::circuitBoardSize));
    const rect2f circuitRect = rect2f(component.location.circuitPos, component.location.circuitPos + 2);
    const rect2f screenRect = params().boardInWindow.toContainer(frame.toContainer(circuitRect));
    renderLocalizedComponent(component.info->name, &component, screenRect, depthLayers::miniCircuitOffset, IconState(component.modifiers, false));
}

void GameCanvas::renderSpokes(const Component &component)
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

void GameCanvas::renderSpokesCircuit(const Component &component)
{
    if (app.activeCircuit() == nullptr ||
        app.activeCircuit()->location.boardPos != component.location.boardPos ||
        component.modifiers.boundary == CircuitBoundaryType::Closed)
        return;

    const vec2f myScreenPos = GameUtil::circuitToWindow(canonicalDims, component.location.circuitPos + vec2i(1, 1));

    const auto &cells = app.activeCircuit()->circuitBoard->cells;

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
                renderSpokes &= otherComponent.modifiers.boundary != CircuitBoundaryType::Closed;
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
        screen.second *= app.state.victoryChargeScaleFactor;

    const float angle = charge.randomRotationOffset + app.state.globalRotationOffset;
    const rect2f destinationRect(screen.first - vec2f(screen.second), screen.first + vec2f(screen.second));

    render(*database().chargeTextures[(int)charge.level], destinationRect, depthLayers::charge, angle);
}

void GameCanvas::renderMegaHold(const Component &component, bool trailRender)
{
    const float megaHoldSize = 7.5f;

    const vec2f screenCenter = component.location.toScreenCoordMainBoard(canonicalDims);
    float screenSize = ((float)component.megaHoldTotalCharge / (float)component.intrinsics.totalChargeRequired) * GameUtil::windowScaleFactor(canonicalDims) * megaHoldSize;

    if (trailRender)
        screenSize *= 1.2f;

    const float angle = component.randomRotationOffset + app.state.globalRotationOffset;
    const rect2f destinationRect(screenCenter - vec2f(screenSize), screenCenter + vec2f(screenSize));

    render(*database().chargeTextures[(int)component.modifiers.color], destinationRect, depthLayers::charge, angle);
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
        screen.second *= app.state.victoryChargeScaleFactor;

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
