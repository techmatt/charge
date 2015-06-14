
#include "main.h"

void GameController::init()
{
    puzzleMode = PuzzleMode::Design;
    speed = GameSpeed::x1;
    designActionTaken = false;
    puzzleVerificationMode = false;
    editorMode = EditorMode::Campaign;
    currentPuzzleIndex = 79;
    fractionalSpeedTicksLeft = 0;

    loadCurrentPuzzle();
}

void GameController::step()
{
    app.soundsPlayedThisTick.clear();

    app.soundCountTicks++;
    if (app.soundCountTicks >= constants::stepsPerSecond)
    {
        app.soundCountTicks = 0;
        app.soundPlayedThisSecond = false;
    }

    if (designActionTaken && puzzleMode != PuzzleMode::Design)
    {
        puzzleMode = PuzzleMode::Design;
        app.state.resetPuzzle();
        puzzleVerificationMode = false;
    }

    if (puzzleMode == PuzzleMode::Executing || puzzleVerificationMode)
    {
        bool startVictory = app.state.victory;

        if (fractionalSpeedTicksLeft)
        {
            fractionalSpeedTicksLeft--;
        }
        else
        {
            int tickCount = ticksFromSpeed(speed);
            if (speed == GameSpeed::Quarter)
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
            speed = GameSpeed::x1;
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
    if (util::endsWith(filename, ".txt"))
    {
        loadLegacyPuzzle(filename);
        return;
    }
    app.state.loadPuzzle(filename, puzzleName);
    app.undoBuffer.reset(app.state);
    designActionTaken = true;
    app.canvas.backgroundDirty = true;
    app.ui.selection.empty();
}

void GameController::loadLegacyPuzzle(const string &filename)
{
    LegacyLoader::load(filename, app.state);
    app.undoBuffer.reset(app.state);
    designActionTaken = true;
    app.canvas.backgroundDirty = true;
    app.ui.selection.empty();
}

PuzzleInfo& GameController::getCurrentPuzzle()
{
    return database().puzzles[currentPuzzleIndex];
}

void GameController::loadCurrentPuzzle()
{
    const PuzzleInfo &puzzle = database().puzzles[app.controller.currentPuzzleIndex];
    app.controller.loadPuzzle(params().assetDir + "levels/" + puzzle.filename + ".pzl", "Puzzle " + to_string(puzzle.index) + ": " + puzzle.name);

    //app.state.savePuzzle(params().assetDir + "levels/" + puzzle.filename + ".pzl");
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
    app.canvas.backgroundDirty = true;
    designActionTaken = true;
    puzzleVerificationMode = false;
}

void GameController::changeEditorMode(EditorMode newMode)
{
    editorMode = newMode;

    const ComponentPuzzleType newType = (editorMode == EditorMode::Campaign) ? ComponentPuzzleType::PuzzlePiece : ComponentPuzzleType::User;

    for (auto &c : app.state.components)
        c->modifiers.puzzleType = newType;
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
            if (app.controller.editorMode == EditorMode::LevelEditor || app.state.buildableComponents.canBuild(realInfo->name, ComponentModifiers(info)))
                buttons.push_back(GameButton(realInfo->name, info.menuCoordinate, ButtonType::Component, ComponentModifiers(info)));
        }
    }

    //
    // Add color, delay, preference, and boundary buttons
    //
    Component *component = app.ui.selection.singleElement();
    if (component != nullptr && component->inactiveCircuitMegaHold(app.state))
        component = &component->getSuperMegaHoldTarget(app.state);

    if (component != nullptr && component->modifiers.puzzleType == ComponentPuzzleType::User)
    {
        const ComponentInfo &info = *component->info;

        if (component->info->name != "Circuit" && component->info->name != "Blocker" && component->info->name != "PowerSource")
        {
            for (int chargePreference = 0; chargePreference <= 4; chargePreference++)
            {
                buttons.push_back(GameButton(info.name, vec2i(chargePreference, 3), ButtonType::ChargePreference, ComponentModifiers(component->modifiers.color, chargePreference)));
            }
        }

        if (info.name == "Wire")
        {
            for (int speed = 0; speed <= 4; speed++)
            {
                ComponentModifiers modifier(ChargeType::None, 2, (WireType)speed);
                if (app.controller.editorMode == EditorMode::LevelEditor || app.state.buildableComponents.canBuild(info.name, modifier))
                    buttons.push_back(GameButton("Wire", vec2i((int)speed, 4), ButtonType::WireSpeed, modifier));
            }
        }
        else if (info.name == "CircuitBoundary")
        {
            buttons.push_back(GameButton("CircuitBoundary", vec2i(0, 4), ButtonType::CircuitBoundary, ComponentModifiers(ChargeType::None, 2, WireType::Standard, CircuitBoundaryType::Open)));
            buttons.push_back(GameButton("CircuitBoundary", vec2i(1, 4), ButtonType::CircuitBoundary, ComponentModifiers(ChargeType::None, 2, WireType::Standard, CircuitBoundaryType::Closed)));
            buttons.push_back(GameButton("CloseAll", vec2i(2, 4), ButtonType::CircuitBoundary, ComponentModifiers()));
        }
        else
        {
            vector<ChargeType> chargeLevels;
            if (info.colorUpgrades)
            {
                ChargeType start = info.name == "FilteredAmplifier" ? ChargeType::Orange : ChargeType::Red;
                ChargeType end = info.grayUpgrade ? ChargeType::Gray : ChargeType::Blue;
                for (int charge = (int)start; charge <= (int)end; charge++)
                    chargeLevels.push_back((ChargeType)charge);
            }

            int chargeIndex = 0;
            for (ChargeType charge : chargeLevels)
            {
                if (app.controller.editorMode == EditorMode::LevelEditor || app.state.buildableComponents.canBuild(info.name, ComponentModifiers(charge)))
                    buttons.push_back(GameButton(info.name, vec2i(chargeIndex, 4), ButtonType::ChargeColor, ComponentModifiers(charge)));
                chargeIndex++;
            }

            //
            // some components have a toggleable state
            //
            if (info.name == "TrapSprung" || info.name == "TrapOpen")
            {
                buttons.push_back(GameButton("TrapOpen", vec2i(0, 5), ButtonType::TrapState, ComponentModifiers(component->modifiers.color)));
                buttons.push_back(GameButton("TrapSprung", vec2i(1, 5), ButtonType::TrapState, ComponentModifiers(component->modifiers.color)));
            }
            if (info.name == "GateOpen" || info.name == "GateClosed")
            {
                buttons.push_back(GameButton("GateOpen", vec2i(0, 5), ButtonType::GateState, ComponentModifiers(component->modifiers.color)));
                buttons.push_back(GameButton("GateClosed", vec2i(1, 5), ButtonType::GateState, ComponentModifiers(component->modifiers.color)));
            }
        }
    }

    //
    // Add puzzle control buttons
    //
    if (app.controller.puzzleMode == PuzzleMode::Design)
        buttons.push_back(GameButton("Start", vec2i(0, 0), ButtonType::PuzzleControl, ComponentModifiers()));
    if (app.controller.puzzleMode == PuzzleMode::Executing)
        buttons.push_back(GameButton("Stop", vec2i(0, 0), ButtonType::PuzzleControl, ComponentModifiers()));
    buttons.push_back(GameButton("Save", vec2i(2, 0), ButtonType::PuzzleControl, ComponentModifiers()));
    buttons.push_back(GameButton("Load", vec2i(3, 0), ButtonType::PuzzleControl, ComponentModifiers()));
    buttons.push_back(GameButton("ModePuzzle", vec2i(5, 0), ButtonType::PuzzleControl, ComponentModifiers()));
    buttons.push_back(GameButton("ModeLevelEditor", vec2i(6, 0), ButtonType::PuzzleControl, ComponentModifiers()));

	buttons.push_back(GameButton("CircuitRotate90", vec2i(8, 0), ButtonType::PuzzleControl, ComponentModifiers()));
	buttons.push_back(GameButton("CircuitRotateN90", vec2i(9, 0), ButtonType::PuzzleControl, ComponentModifiers()));
	buttons.push_back(GameButton("CircuitFlipVertical", vec2i(10, 0), ButtonType::PuzzleControl, ComponentModifiers()));
	buttons.push_back(GameButton("CircuitFlipHorizontal", vec2i(11, 0), ButtonType::PuzzleControl, ComponentModifiers()));


    for (int speed = (int)GameSpeed::x0; speed <= (int)GameSpeed::x5; speed++)
        buttons.push_back(GameButton(buttonNameFromSpeed((GameSpeed)speed), vec2i(speed, 1), ButtonType::PuzzleControl, ComponentModifiers()));

    buttons.push_back(GameButton("Music", vec2i(6, 1), ButtonType::PuzzleControl, ComponentModifiers()));
    buttons.push_back(GameButton("SoundEffect", vec2i(7, 1), ButtonType::PuzzleControl, ComponentModifiers()));

    //
    // Add power source indicators
    //
    if (component != nullptr && component->info->name == "PowerSource")
    {
        buttons.push_back(GameButton("TotalCharge", vec2i(0, 0), ButtonType::ComponentAttribute, "Total charges: " + to_string(component->intrinsics.totalCharges)));
        buttons.push_back(GameButton("FirstEmission", vec2i(0, 1), ButtonType::ComponentAttribute, "First charge at " + to_string(component->intrinsics.secondsBeforeFirstEmission) + "s"));
        buttons.push_back(GameButton("EmissionFrequency", vec2i(0, 2), ButtonType::ComponentAttribute, "New charge every " + to_string(component->intrinsics.secondsPerEmission) + "s"));
    }

    //
    // Add megahold indicators
    //
    if (component != nullptr && component->info->name == "MegaHold")
    {
        const double dischargeFreq = component->intrinsics.ticksPerDischarge * constants::secondsPerStep;
        buttons.push_back(GameButton("TicksPerDischarge", vec2i(0, 0), ButtonType::ComponentAttribute, "Discharge every: " + util::formatDouble(dischargeFreq, 2) + "s"));
        buttons.push_back(GameButton("ChargesLostPerDischarge", vec2i(0, 1), ButtonType::ComponentAttribute, "Discharge size: " + to_string(component->intrinsics.chargesLostPerDischarge)));
        buttons.push_back(GameButton("TotalChargeRequired", vec2i(0, 2), ButtonType::ComponentAttribute, "Total charge needed: " + to_string(component->intrinsics.totalChargeRequired)));
    }
}