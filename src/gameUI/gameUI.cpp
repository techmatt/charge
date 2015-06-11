
#include "main.h"

// GetAsyncKeyState
#include <Windows.h>

void GameUI::init()
{
    //selectedGameLocation.boardPos = constants::invalidCoord;
    selection.empty();

    selectedMenuComponent = nullptr;

    activePlacementBuffer.clear();

    hoverButtonIndex = -1;
}

bool ctrlPressed()
{
    const Uint8 *keys = SDL_GetKeyboardState(NULL);
    return keys[SDL_SCANCODE_LCTRL] || keys[SDL_SCANCODE_RCTRL];
}

bool shiftPressed()
{
    // NOTE: the spec for SDL_GetKeyboardState suggests shift is not recorded!
    const Uint8 *keys = SDL_GetKeyboardState(NULL);
    return keys[SDL_SCANCODE_LSHIFT] || keys[SDL_SCANCODE_RSHIFT];
}

void GameUI::keyDown(SDL_Keycode key)
{
    //
    // TODO: this is more aggressive than it needs to be, but it probably doesn't matter.
    //
    app.canvas.backgroundDirty = true;

    if (!ctrlPressed())
    {
        const ComponentInfo *hotkeyComponent = database().componentFromKey(key);
        if (hotkeyComponent != nullptr)
        {
            selectedMenuComponent = hotkeyComponent;
            selectedMenuComponentColor = selectedMenuComponent->defaultPrimaryCharge();

            activePlacementBuffer.clear();
            activePlacementBuffer = ComponentSet(selectedMenuComponent, selectedMenuComponentColor);
        }
    }

    if (key == SDLK_ESCAPE)
    {
        selectedMenuComponent = nullptr;

        activePlacementBuffer.clear();

        //selectedGameLocation.boardPos = constants::invalidCoord;
        selection.empty();
    }

    if (key == SDLK_RETURN)
    {
        app.controller.designActionTaken = false;
        app.controller.puzzleMode = PuzzleMode::Executing;
        app.state.resetPuzzle();
    }

    if (key == SDLK_F5)
    {
        ParameterFile parameterFile("../assets/parameters.txt");
        g_gameParams.load(parameterFile);
    }

    Component* gameComponent = selection.singleElement();
    if (gameComponent != nullptr && app.controller.canEdit(*gameComponent))
    {
        if (key >= SDLK_1 && key <= SDLK_6)
        {
            ChargeType color = (ChargeType)((int)ChargeType::Red + key - SDLK_1);

            if (gameComponent->info->colorUpgrades && color != ChargeType::Gray)
                gameComponent->modifiers.color = color;
            if (gameComponent->info->grayUpgrade && color == ChargeType::Gray)
                gameComponent->modifiers.color = color;

            if (gameComponent->info->name == "Wire")
                gameComponent->modifiers.speed = (WireType)(key - SDLK_1);

            app.controller.recordDesignAction();
        }

        if (key >= SDLK_1 && key <= SDLK_2 && gameComponent->info->name == "CircuitBoundary")
        {
            if (key == SDLK_1) gameComponent->modifiers.boundary = CircuitBoundaryType::Open;
            if (key == SDLK_2) gameComponent->modifiers.boundary = CircuitBoundaryType::Closed;
        }

        auto keyToPreference = [](SDL_Keycode key)
        {
            switch (key)
            {
            case SDLK_7: return 0;
            case SDLK_8: return 1;
            case SDLK_9: return 2;
            case SDLK_0: return 3;
            case SDLK_MINUS: return 4;
            default: return -1;
            }
        };

        int preference = keyToPreference(key);
        if (preference != -1)
        {
            gameComponent->modifiers.chargePreference = preference;
            app.controller.recordDesignAction();
        }
    }

    if (key == SDLK_LEFT)
    {
        app.controller.currentPuzzleIndex = math::mod(app.controller.currentPuzzleIndex - 1, database().puzzles.size());
        app.controller.loadCurrentPuzzle();
    }
    if (key == SDLK_RIGHT)
    {
        app.controller.currentPuzzleIndex = math::mod(app.controller.currentPuzzleIndex + 1, database().puzzles.size());
        app.controller.loadCurrentPuzzle();

    }
    if (key == SDLK_UP)
    {
        app.controller.loadCurrentProvidedSolution();
    }
    if (key == SDLK_z)
    {
        // undo and redo
        if (ctrlPressed()){
            if (shiftPressed())
                app.undoBuffer.forward(app.state);
            else
                app.undoBuffer.back(app.state);
        }
    }

    if (key == SDLK_c)
    {
        // COPY if there is anything in the selection buffer
        if (selection.components.size() > 0)
            selection.copyToComponentSet(&copyBuffer, &app.state);
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
        //delete all the selected components
        for (Component *c : app.ui.selection.components)
            app.state.removeComponent(c, false);
        selection.empty();
        app.state.updateAll();
    }
}

void GameUI::removeHoverComponent()
{
    const GameLocation location = hoverLocation(false);

    Component *c = app.state.getComponent(location);
    if (c == nullptr || !app.controller.canEdit(*c) || c->info->name == "CircuitBoundary")
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
    app.canvas.backgroundDirty = true;
    app.controller.designActionTaken = true;
    app.undoBuffer.save(app.state);
}

void GameUI::mouseUp(Uint8 mouseButton, int x, int y)
{
    const Uint8 *keys = SDL_GetKeyboardState(NULL);

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
            Component* hoverComponent = app.state.getComponent(hover);
            if (keys[SDL_SCANCODE_LCTRL] || keys[SDL_SCANCODE_RCTRL]){
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

    app.canvas.backgroundDirty = true;
}

void GameUI::mouseDown(Uint8 mouseButton, int x, int y)
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
            addHoverComponent();
        }
    }

    const GameButton *hitButton = app.controller.getHitButton(mouseHoverCoord);
    if (hitButton == nullptr)
        return;
    const GameButton &button = *hitButton;

    int delta = mouseButton == SDL_BUTTON_LEFT ? -1 : 1;

    if (ctrlPressed() || shiftPressed())
        delta *= 10;

    //Component *gameComponent = app.state.getComponent(selectedGameLocation);
    Component* gameComponent = selection.singleElement();
    if (gameComponent != nullptr && button.type == ButtonType::ComponentAttribute && app.controller.editorMode == EditorMode::LevelEditor)
    {
        //
        // power source modifiers
        //
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

        //
        // megahold modifiers
        //
        if (button.name == "TicksPerDischarge")
        {
            gameComponent->intrinsics.ticksPerDischarge = math::clamp(gameComponent->intrinsics.ticksPerDischarge + delta, 1, 1000);
            app.controller.recordDesignAction();
        }
        if (button.name == "ChargesLostPerDischarge")
        {
            gameComponent->intrinsics.chargesLostPerDischarge = math::clamp(gameComponent->intrinsics.chargesLostPerDischarge + delta, 0, 1000);
            app.controller.recordDesignAction();
        }
        if (button.name == "TotalChargeRequired")
        {
            gameComponent->intrinsics.totalChargeRequired = math::clamp(gameComponent->intrinsics.totalChargeRequired + delta, 1, 1000);
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
        if (button.type == ButtonType::Component)
        {
            selectedMenuComponent = button.component;
            selectedMenuComponentColor = button.modifiers.color;

            activePlacementBuffer.clear();
            activePlacementBuffer = ComponentSet(selectedMenuComponent, selectedMenuComponentColor);
        }
        if (gameComponent != nullptr && app.controller.canEdit(*gameComponent))
        {
            if (button.type == ButtonType::ChargeColor)
            {
                gameComponent->modifiers.color = button.modifiers.color;
                app.controller.recordDesignAction();
            }
            if (button.type == ButtonType::ChargePreference)
            {
                gameComponent->modifiers.chargePreference = button.modifiers.chargePreference;
                app.controller.recordDesignAction();
            }
            if (button.type == ButtonType::WireSpeed)
            {
                gameComponent->modifiers.speed = button.modifiers.speed;
                app.controller.recordDesignAction();
            }
            if (button.type == ButtonType::CircuitBoundary)
            {
                gameComponent->modifiers.boundary = button.modifiers.boundary;
                app.controller.recordDesignAction();
            }
            if (button.type == ButtonType::GateState || button.type == ButtonType::TrapState)
            {
                gameComponent->baseInfo = gameComponent->info = &database().getComponent(button.name);
                app.controller.recordDesignAction();
            }
        }
        if (button.name == "Start")
        {
            app.controller.designActionTaken = false;
            app.controller.puzzleMode = PuzzleMode::Executing;
            app.state.resetPuzzle();
            if (app.controller.speed == GameSpeed::x0)
                app.controller.speed = GameSpeed::x1;
        }
        if (button.name == "Stop")
        {
            app.controller.puzzleMode = PuzzleMode::Design;
            app.state.resetPuzzle();
        }
        if (button.name == "Pause")
        {
            app.controller.speed = GameSpeed::x0;
        }
        if (button.name == "ModePuzzle")
        {
            app.controller.puzzleVerificationMode = false;
            app.controller.editorMode = EditorMode::Campaign;
        }
        if (button.name == "ModeLevelEditor")
        {
            app.controller.puzzleVerificationMode = false;
            app.controller.editorMode = EditorMode::LevelEditor;

            if (GetAsyncKeyState(VK_SHIFT))
            {
                app.controller.puzzleVerificationMode = true;
                app.controller.loadCurrentProvidedSolution();
            }
        }
        if (button.name == "Music")
        {
            app.audio.setMusic(!app.audio.playMusic);
        }
        if (button.name == "SoundEffect")
        {
            app.audio.playSoundEffects = !app.audio.playSoundEffects;
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
                app.state.loadPuzzle(filename, util::removeExtensions(util::fileNameFromPath(filename)));
                app.undoBuffer.reset(app.state);
            }
        }
		if (button.name == "CircuitRotateN90")
			app.ui.activePlacementBuffer.rotate(1);
		if (button.name == "CircuitRotate90")
			app.ui.activePlacementBuffer.rotate(3);
		if (button.name == "CircuitFlipHorizontal")
			app.ui.activePlacementBuffer.flipAboutVerical();
		if (button.name == "CircuitFlipVertical")
			app.ui.activePlacementBuffer.flipAboutHorizonal();

        for (int speed = (int)GameSpeed::x0; speed <= (int)GameSpeed::x5; speed++)
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

void GameUI::addHoverComponent()
{
    if (selectedMenuComponent == nullptr && activePlacementBuffer.isEmpty())
        return;

    GameLocation location = hoverLocation(true);

    if (!location.valid())
        return;

    if (location.inCircuit() && app.activeCircuit() == nullptr)
        return;

    vec2i buffermin = activePlacementBuffer.boundingBox().min();

    //figure out component placement
    for (ComponentDefiningProperties c : activePlacementBuffer.components)
    {
        if (c.location.inCircuit()) continue;
        GameLocation componentLocation;

        // move to the placement location
        if (location.inCircuit())
            componentLocation = GameLocation(location.circuitPos + c.location.boardPos - buffermin, c.location.circuitPos);
        else
            componentLocation = GameLocation(location.boardPos + c.location.boardPos - buffermin, c.location.circuitPos);



        const vec2i coordBase = componentLocation.boardPos;
        const Board &board = location.inCircuit() ? *app.activeCircuit()->circuitBoard : app.state.board;

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
        activePlacementBuffer.addToCircuit(app.state, location.boardPos, offset);
    }
    else
    {
        vec2i offset = location.boardPos - buffermin;
        activePlacementBuffer.addToComponents(app.state, offset);
    }

    app.controller.recordDesignAction();
    app.undoBuffer.save(app.state); //saves to the backwards/forwards buffer

}
