
#include "main.h"

void GameController::init()
{
    puzzleMode = ModeDesign;
    speed = Speed1x;
    designActionTaken = false;
    puzzleVerificationMode = false;
    editorMode = ModeEditLevel;
    currentPuzzleIndex = 20;
    fractionalSpeedTicksLeft = 0;

    loadCurrentPuzzle();
}

void GameController::step()
{
    if (designActionTaken && puzzleMode != ModeDesign)
    {
        puzzleMode = ModeDesign;
        app.state.resetPuzzle();
        puzzleVerificationMode = false;
    }

    if (puzzleMode == ModeExecuting || puzzleVerificationMode)
    {
        bool startVictory = app.state.victory;

        if (fractionalSpeedTicksLeft)
        {
            fractionalSpeedTicksLeft--;
        }
        else
        {
            int tickCount = ticksFromSpeed(speed);
            if (speed == SpeedQuarter)
                fractionalSpeedTicksLeft = 3;

            if (puzzleVerificationMode)
                tickCount = 10;

            for (int tickIndex = 0; tickIndex < tickCount; tickIndex++)
            {
                app.state.step(app);
            }
        }

        if (!startVictory && app.state.victory)
        {
            speed = Speed1x;
            const string defaultSolution = params().assetDir + "providedSolutions/" + database().puzzles[app.controller.currentPuzzleIndex].filename + "_A.pzl";
            app.state.savePuzzle(defaultSolution);

            if (puzzleVerificationMode)
            {
                app.controller.currentPuzzleIndex = math::mod(app.controller.currentPuzzleIndex + 1, database().puzzles.size());
                loadCurrentProvidedSolution();
            }
        }
    }
}

void GameController::loadPuzzle(const string &filename, const string &puzzleName)
{
    app.state.loadPuzzle(filename, puzzleName);
    app.undoBuffer.reset(app.state);
    designActionTaken = true;
    app.ui.backgroundDirty = true;
    app.ui.selection.empty();
}

void GameController::loadLegacyPuzzle(const string &filename)
{
    LegacyLoader::load(filename, app.state);
    app.undoBuffer.reset(app.state);
    designActionTaken = true;
    app.ui.backgroundDirty = true;
    app.ui.selection.empty();
}

void GameController::loadCurrentPuzzle()
{
    const PuzzleInfo &puzzle = database().puzzles[app.controller.currentPuzzleIndex];
    app.controller.loadPuzzle(params().assetDir + "levels/" + puzzle.filename + ".pzl", "Puzzle " + to_string(puzzle.index) + ": " + puzzle.name);
}

void GameController::loadCurrentProvidedSolution()
{
    const PuzzleInfo &puzzle = database().puzzles[app.controller.currentPuzzleIndex];
    const string puzzleFilename = params().assetDir + "levels/" + puzzle.filename + ".pzl";
    const string solutionFilename = params().assetDir + "providedSolutions/" + puzzle.filename + "_A.pzl";
    if (util::fileExists(puzzleFilename) && util::fileExists(solutionFilename))
    {
        app.controller.loadPuzzle(solutionFilename, "Example solution " + to_string(puzzle.index) + ": " + puzzle.name);

        //
        // Load the base puzzle file.
        // TODO: We should verify the solution is compatiable with the underlying puzzle file.
        //
        GameState baseState;
        baseState.init();
        baseState.loadPuzzle(puzzleFilename, "comparison");
        app.state.buildableComponents = baseState.buildableComponents;
    }
}

void GameController::recordDesignAction()
{
    app.state.updateAll();
    app.ui.backgroundDirty = true;
    designActionTaken = true;
    puzzleVerificationMode = false;
}

void GameController::updateButtonList()
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
    //Component *selectedGameComponent = app.state.getComponent(selectedGameLocation);
    Component *selectedGameComponent = app.ui.selection.singleElement();
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
            if (info.colorUpgrades)
            {
                ChargeType start = info.name == "FilteredAmplifier" ? ChargeOrange : ChargeRed;
                ChargeType end = info.grayUpgrade ? ChargeGray : ChargeBlue;
                for (int charge = (int)start; charge <= (int)end; charge++)
                    chargeLevels.push_back((ChargeType)charge);
            }

            int chargeIndex = 0;
            for (ChargeType charge : chargeLevels)
            {
                if (app.controller.editorMode == ModeEditLevel || app.state.buildableComponents.canBuild(info.name, ComponentModifiers(charge)))
                    buttons.push_back(GameButton(info.name, vec2i(chargeIndex, 4), ButtonType::ButtonChargeColor, ComponentModifiers(charge)));
                chargeIndex++;
            }

            //
            // some components have a toggleable state
            //
            if (info.name == "TrapSprung" || info.name == "TrapOpen")
            {
                buttons.push_back(GameButton("TrapOpen", vec2i(0, 5), ButtonType::ButtonTrapState, ComponentModifiers(selectedGameComponent->modifiers.color)));
                buttons.push_back(GameButton("TrapSprung", vec2i(1, 5), ButtonType::ButtonTrapState, ComponentModifiers(selectedGameComponent->modifiers.color)));
            }
            if (info.name == "GateOpen" || info.name == "GateClosed")
            {
                buttons.push_back(GameButton("GateOpen", vec2i(0, 5), ButtonType::ButtonGateState, ComponentModifiers(selectedGameComponent->modifiers.color)));
                buttons.push_back(GameButton("GateClosed", vec2i(1, 5), ButtonType::ButtonGateState, ComponentModifiers(selectedGameComponent->modifiers.color)));
            }
        }
    }

    //
    // Add puzzle control buttons
    //
    if (app.controller.puzzleMode == ModeDesign)
        buttons.push_back(GameButton("Start", vec2i(0, 0), ButtonType::ButtonPuzzleControl, ComponentModifiers()));
    if (app.controller.puzzleMode == ModeExecuting)
        buttons.push_back(GameButton("Stop", vec2i(0, 0), ButtonType::ButtonPuzzleControl, ComponentModifiers()));
    buttons.push_back(GameButton("Save", vec2i(2, 0), ButtonType::ButtonPuzzleControl, ComponentModifiers()));
    buttons.push_back(GameButton("Load", vec2i(3, 0), ButtonType::ButtonPuzzleControl, ComponentModifiers()));
    buttons.push_back(GameButton("ModePuzzle", vec2i(5, 0), ButtonType::ButtonPuzzleControl, ComponentModifiers()));
    buttons.push_back(GameButton("ModeLevelEditor", vec2i(6, 0), ButtonType::ButtonPuzzleControl, ComponentModifiers()));

    for (int speed = (int)Speed0x; speed <= (int)Speed5x; speed++)
        buttons.push_back(GameButton(buttonNameFromSpeed((GameSpeed)speed), vec2i(speed, 1), ButtonType::ButtonPuzzleControl, ComponentModifiers()));

    buttons.push_back(GameButton("Music", vec2i(6, 1), ButtonType::ButtonPuzzleControl, ComponentModifiers()));
    buttons.push_back(GameButton("SoundEffect", vec2i(7, 1), ButtonType::ButtonPuzzleControl, ComponentModifiers()));

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