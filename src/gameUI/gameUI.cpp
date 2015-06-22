
#include "main.h"

void GameUI::init()
{
    //selectedGameLocation.boardPos = constants::invalidCoord;
    selection.empty();

    selectedMenuComponent = nullptr;

    activePlacementBuffer.clear();

    hoverButtonIndex = -1;

    spaceUp = true;
}

void GameUI::keyDown(SDL_Keycode key, bool shift, bool ctrl)
{
    //
    // TODO: this is more aggressive than it needs to be, but it probably doesn't matter.
    //
    app.canvas.backgroundDirty = true;

    if (key == SDLK_ESCAPE)
    {
        selectedMenuComponent = nullptr;

        activePlacementBuffer.clear();

        //selectedGameLocation.boardPos = constants::invalidCoord;
        selection.empty();
    }

    if (key == SDLK_F5)
    {
        ParameterFile parameterFile("../assets/parameters.txt");
        g_gameParams.load(parameterFile);
    }

    Component* gameComponent = selection.singleElement();

    for (const GameButton &b : app.controller.buttons)
    {
        if (b.hotkeyCode == key)
        {
            b.leftClick(app, gameComponent);
        }
    }

    if (key == SDLK_z)
    {
        // undo and redo
        if (ctrl){
            if (shift)
                app.undoBuffer.forward(app.state);
            else
                app.undoBuffer.back(app.state);
        }
    }

    if (key == SDLK_c)
    {
        // COPY if there is anything in the selection buffer
        if (selection.components.size() > 0 && selection.isValidCopy())
            selection.copyToComponentSet(copyBuffer, app.state);
        else
            copyBuffer.clear();
    }

    if (key == SDLK_v)
    {
        selectedMenuComponent = nullptr;
        activePlacementBuffer = copyBuffer;
    }

    if (key == SDLK_F8)
    {
        // debug key to disable all components
        app.state.buildableComponents.components.clear();
    }

    if (key == SDLK_DELETE || key == SDLK_BACKSPACE)
    {
        Component *selectedComponent = selection.singleElement();
        if (selectedComponent != nullptr && selectedComponent->modifiers.puzzleType == ComponentPuzzleType::User)
        {
            //delete all the selected components
            for (Component *c : app.ui.selection.components)
                app.state.removeComponent(c, false);
            app.state.updateAll();
        }
        selection.empty();
    }

    if (key == SDLK_SPACE && spaceUp)
    {
        spaceUp = false;
        if (app.controller.speed != GameSpeed::x10)
            app.controller.speed = GameSpeed((int)app.controller.speed + 1);
    }
}

void GameUI::keyUp(SDL_Keycode key)
{
    if (key == SDLK_SPACE)
    {
        spaceUp = true;
        if (app.controller.speed != GameSpeed::x0)
            app.controller.speed = GameSpeed((int)app.controller.speed - 1);
        app.canvas.backgroundDirty = true;
    }
}

void GameUI::removeHoverComponent()
{
    const GameLocation location = hoverLocation(false);

    Component *c = app.state.getComponent(location);
    if (c == nullptr || c->modifiers.puzzleType == ComponentPuzzleType::PuzzlePiece || c->info->name == "CircuitBoundary")
        return;

    //remove contained elements from the selection
    //if (c->circuitBoard != nullptr)
    //	for (auto &cell : c->circuitBoard->cells)
    //		if (cell.value.c != nullptr)
    //			selection.remove(cell.value.c);

    if (c->circuitBoard != nullptr&&selection.selectionIsInCircuit && selection.circuitLocation == c->location.boardPos)
        selection.empty();

    // and remove the board from the selection
    selection.remove(c);

    // and delete the actual component
    app.state.removeComponent(c);
    app.controller.recordDesignAction();
    app.undoBuffer.save(app.state);
}

void GameUI::mouseUp(Uint8 mouseButton, int x, int y, bool shift, bool ctrl)
{
    // we only do this for left clicks
    if (mouseButton != SDL_BUTTON_LEFT)
        return;

    // no dragging if we are placing things
    if (!activePlacementBuffer.isEmpty())
        return;


    // the current coordinates
    CoordinateFrame windowFrame = app.renderer.getWindowCoordinateFrame();
    mouseHoverCoord = vec2i(windowFrame.fromContainer(vec2f((float)x, (float)y)));
    x = mouseHoverCoord.x;
    y = mouseHoverCoord.y;
    GameLocation hover = hoverLocation(false);

    // TODO: do stuff when releasing the button
    vec2f mouseDelta = mouseHoverCoord - clickScreenLocation;
    if (mouseDelta.length() > constants::dragThreshold)
    {
        // The mouse was dragged
    }
    else
    {
        // Just a click
        if (hover.valid())
        {
            app.canvas.backgroundDirty = true;
            Component* hoverComponent = app.state.getComponent(hover, false);
            if (ctrl || shift){
                if (hoverComponent != nullptr)
                    selection.toggle(hoverComponent);
            }
            else
            {
                //selectedGameLocation = hover;
                if (hoverComponent != nullptr)
                    selection.newSelectionFromComponent(hoverComponent);
            }
        }

    }
}

void GameUI::mouseDown(Uint8 mouseButton, int x, int y, bool shift, bool ctrl)
{

    const Uint8 *keys = SDL_GetKeyboardState(NULL);

    CoordinateFrame windowFrame = app.renderer.getWindowCoordinateFrame();
    mouseHoverCoord = vec2i(windowFrame.fromContainer(vec2f((float)x, (float)y)));

    x = mouseHoverCoord.x;
    y = mouseHoverCoord.y;

    //
    // TODO: this is more aggressive than it needs to be, but it probably doesn't matter.
    //
    app.canvas.backgroundDirty = true;

    if (mouseButton == SDL_BUTTON_RIGHT)
    {
        if (!activePlacementBuffer.isEmpty())
        {
            selectedMenuComponent = nullptr;
            activePlacementBuffer.clear();
        }
        else
        {
            removeHoverComponent();
        }
    }

    if (mouseButton == SDL_BUTTON_LEFT)
    {
        if (activePlacementBuffer.isEmpty())
        {
            GameLocation hover = hoverLocation(false);
            clickLocation = hover;
            clickScreenLocation = mouseHoverCoord;

            // stuff that was gets processed when the mouse is released, in case of a click and drag event
        }
        else
        {
            addHoverComponent(hoverLocation(true));
        }
    }

    const GameButton *hitButton = app.controller.getHitButton(mouseHoverCoord);
    if (hitButton == nullptr)
        return;
    const GameButton &button = *hitButton;

    int delta = mouseButton == SDL_BUTTON_LEFT ? -1 : 1;

    if (ctrl || shift)
        delta *= 10;

    //Component *gameComponent = app.state.getComponent(selectedGameLocation);
    Component* gameComponent = selection.singleElement();
    if (gameComponent != nullptr && button.type == ButtonType::ComponentAttribute && app.controller.editorMode == EditorMode::LevelEditor)
    {
        Component *modifiedComponent = gameComponent;
        if (gameComponent->inactiveCircuitMegaHold(app.state))
            modifiedComponent = &gameComponent->getSuperMegaHoldTarget(app.state);

        //
        // power source modifiers
        //
        if (button.name == "TotalCharge")
        {
            modifiedComponent->intrinsics.totalCharges = math::mod(modifiedComponent->intrinsics.totalCharges + delta, 0xFFFFFF);
            app.controller.recordDesignAction();
        }
        if (button.name == "FirstEmission")
        {
            modifiedComponent->intrinsics.secondsBeforeFirstEmission = math::clamp(modifiedComponent->intrinsics.secondsBeforeFirstEmission + delta, 0, 1000);
            app.controller.recordDesignAction();
        }
        if (button.name == "EmissionFrequency")
        {
            modifiedComponent->intrinsics.secondsPerEmission = math::clamp(modifiedComponent->intrinsics.secondsPerEmission + delta, 1, 1000);
            app.controller.recordDesignAction();
        }

        //
        // megahold modifiers
        //
        if (button.name == "TicksPerDischarge")
        {
            modifiedComponent->intrinsics.ticksPerDischarge = math::clamp(modifiedComponent->intrinsics.ticksPerDischarge + delta, 1, 1000);
            app.controller.recordDesignAction();
        }
        if (button.name == "ChargesLostPerDischarge")
        {
            modifiedComponent->intrinsics.chargesLostPerDischarge = math::clamp(modifiedComponent->intrinsics.chargesLostPerDischarge + delta, 0, 1000);
            app.controller.recordDesignAction();
        }
        if (button.name == "TotalChargeRequired")
        {
            modifiedComponent->intrinsics.totalChargeRequired = math::clamp(modifiedComponent->intrinsics.totalChargeRequired + delta, 1, 1000);
            app.controller.recordDesignAction();
        }
    }

    if (mouseButton == SDL_BUTTON_RIGHT && app.controller.editorMode == EditorMode::LevelEditor)
    {
        if (button.type == ButtonType::Component)
        {
            const bool buildable = app.state.buildableComponents.canBuild(button.name, button.modifiers);
            const auto &info = database().getComponent(button.name);
            if (database().getComponent(button.name).colorUpgrades && button.modifiers.color != ChargeType::Gray)
            {
                ChargeType start = info.name == "FilteredAmplifier" ? ChargeType::Orange : ChargeType::Red;
                for (int charge = (int)start; charge <= (int)ChargeType::Blue; charge++)
                    app.state.buildableComponents.setBuild(button.name, ComponentModifiers((ChargeType)charge), !buildable);
            }
            else if (button.name == "Wire")
            {
                for (int speed = (int)WireType::MajorDelay; speed <= (int)WireType::MajorAccelerator; speed++)
                    app.state.buildableComponents.setBuild(button.name, ComponentModifiers(ChargeType::None, 2, (WireType)speed), !buildable);
            }
            else
            {
                app.state.buildableComponents.setBuild(button.name, button.modifiers, !buildable);
            }
        }
        if (button.type == ButtonType::ChargeColor)
        {
            app.state.buildableComponents.setBuild(button.name, button.modifiers, !app.state.buildableComponents.canBuild(button.name, button.modifiers));
        }
        if (button.type == ButtonType::WireSpeed)
        {
            app.state.buildableComponents.setBuild(button.name, button.modifiers, !app.state.buildableComponents.canBuild(button.name, button.modifiers));
        }
    }

    if (mouseButton == SDL_BUTTON_LEFT)
    {
        button.leftClick(app, gameComponent);
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
        addHoverComponent(hoverLocation(true));
    }
    if (buttonState & SDL_BUTTON_RMASK)
    {
        removeHoverComponent();
    }

    hoverButtonIndex;
}

GameLocation GameUI::hoverLocation(bool constructionOffset,const vec2f mouseOffsetFromHover) const
{
    vec2f newMouseOffsetFromHover = mouseOffsetFromHover;
    rect2i bufferShape;

    // start by finding the actual mouseOffsetFromHover, which is the vector from the upper left corner of the active buffer to the mouse
    if (constructionOffset && mouseOffsetFromHover.x < 0.0f) //it works...
    {
        bufferShape = activePlacementBuffer.boundingBox();
        newMouseOffsetFromHover = vec2f(bufferShape.extent()) / 2.0f;
    }

    vec2f boardCoordf = GameUtil::windowToBoard(app.canvas.canonicalDims, mouseHoverCoord);
    vec2f circuitCoordf = GameUtil::windowToCircuit(app.canvas.canonicalDims, mouseHoverCoord);

    bool onBoard = ((0 <= boardCoordf.x && boardCoordf.x <= params().boardDims.x) && (0 <= boardCoordf.y && boardCoordf.y <= params().boardDims.y));
    bool onCircuit = app.activeCircuit() != nullptr && ((0 <= circuitCoordf.x && circuitCoordf.x <= constants::circuitBoardSize) && (0 <= circuitCoordf.y && circuitCoordf.y <= constants::circuitBoardSize));

    if (!onBoard && !onCircuit)
        return GameLocation(constants::invalidCoord);


    vec2i boardCoordi = vec2i(0, 0);
    vec2i circuitCoordi = vec2i(0, 0);

    vec2f highestPosition;
    if (onCircuit && constructionOffset)
    {
        highestPosition = vec2f((float)constants::circuitBoardSize, (float)constants::circuitBoardSize) - vec2f(bufferShape.extent()) + vec2f(newMouseOffsetFromHover);
        circuitCoordf.x = max(circuitCoordf.x, newMouseOffsetFromHover.x);
        circuitCoordf.y = max(circuitCoordf.y, newMouseOffsetFromHover.y);
        circuitCoordf.x = min(circuitCoordf.x, highestPosition.x);
        circuitCoordf.y = min(circuitCoordf.y, highestPosition.y);
        circuitCoordf -= newMouseOffsetFromHover;
    }
    else if (onBoard && constructionOffset)
    {
        highestPosition = vec2f((float)params().boardDims.x, (float)params().boardDims.x) - vec2f(bufferShape.extent()) + vec2f(newMouseOffsetFromHover);
        boardCoordf.x = max(boardCoordf.x, newMouseOffsetFromHover.x);
        boardCoordf.y = max(boardCoordf.y, newMouseOffsetFromHover.y);
        boardCoordf.x = min(boardCoordf.x, highestPosition.x);
        boardCoordf.y = min(boardCoordf.y, highestPosition.y);
        boardCoordf -= newMouseOffsetFromHover;
    }

    // this will return the square we are actually on, rather than the closest vertex.
    if (onBoard && !constructionOffset) boardCoordf -= vec2f(0.5f, 0.5f);
    if (onCircuit && !constructionOffset) circuitCoordf -= vec2f(0.5f, 0.5f);


    if (onCircuit) return GameLocation(app.activeCircuit()->location.boardPos, vec2i((int)round(circuitCoordf.x), (int)round(circuitCoordf.y)));
    else return GameLocation(vec2i((int)round(boardCoordf.x), (int)round(boardCoordf.y)));
}

void GameUI::addHoverComponent(const GameLocation &location)
{
    if (activePlacementBuffer.isEmpty())
        return;

    if (!location.valid())
        return;

    if (location.inCircuit() && app.activeCircuit() == nullptr)
        return;

    vec2i buffermin = activePlacementBuffer.boundingBox().min();

    const Board &board = location.inCircuit() ? *app.activeCircuit()->circuitBoard : app.state.board;

    //
    // when overriding a single placement, copy the preference of the replaced component.
    //
    int preferenceOverride = -1;

    //
    // in the case of a single placement, the user can override an existing non-circuit placement with their placement if they
    // are exactly over the target.
    //
    if (activePlacementBuffer.components.size() == 1 && activePlacementBuffer.components.front().baseInfo->name != "Circuit")
    {
        const ComponentDefiningProperties &c = activePlacementBuffer.components.front();

        vec2i componentLocation;

        if (location.inCircuit())
            componentLocation = location.circuitPos + c.location.boardPos - buffermin;
        else
            componentLocation = location.boardPos + c.location.boardPos - buffermin;

        if (board.cells.coordValid(componentLocation))
        {
            Component *c = board.cells(componentLocation).c;
            if (c != nullptr && c->location == location && c->modifiers.puzzleType == ComponentPuzzleType::User && c->info->name != "CircuitBoundary" && c->info->name != "Circuit")
            {
                if (c->modifiers.chargePreference != 2)
                    preferenceOverride = c->modifiers.chargePreference;
                selection.remove(c);
                app.state.removeComponent(c);
            }
        }
    }
    
    //
    // test if we can build at the specified location
    //
    for (const ComponentDefiningProperties &c : activePlacementBuffer.components)
    {
        if (c.location.inCircuit()) continue;
        GameLocation componentLocation;

        // move to the placement location
        if (location.inCircuit())
            componentLocation = GameLocation(location.circuitPos + c.location.boardPos - buffermin, c.location.circuitPos);
        else
            componentLocation = GameLocation(location.boardPos + c.location.boardPos - buffermin, c.location.circuitPos);

        const vec2i coordBase = componentLocation.boardPos;
        
        for (int xOffset = 0; xOffset <= 1; xOffset++)
            for (int yOffset = 0; yOffset <= 1; yOffset++)
            {
                const vec2i coord = coordBase + vec2i(xOffset, yOffset);
                if (board.cells.coordValid(coord))
                {
                    if (canNotBuildAtPosition(board, c, coord))
                        return;
                }
            }
    }

    // verfied that we can build the thing at the specified place
    // build it

    if (location.inCircuit())
    {
        vec2i offset = location.circuitPos - buffermin;
        activePlacementBuffer.addToCircuit(app.state, location.boardPos, offset, preferenceOverride);
    }
    else
    {
        vec2i offset = location.boardPos - buffermin;
        activePlacementBuffer.addToComponents(app.state, offset, preferenceOverride);
    }

    app.controller.recordDesignAction();
    app.undoBuffer.save(app.state); //saves to the backwards/forwards buffer

}
