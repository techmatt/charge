
#include "main.h"

void GameController::init()
{
    puzzleMode = PuzzleMode::Design;
    speed = GameSpeed::x1;
    designActionTaken = false;
    puzzleVerificationMode = false;
    //viewMode = ControllerViewMode::BasePuzzle;
    editorMode = EditorMode::Campaign;
    fractionalSpeedTicksLeft = 0;

    loadLevelPackPuzzle("Campaign", 0, "BasePuzzle");
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
        if (fractionalSpeedTicksLeft)
        {
            fractionalSpeedTicksLeft--;
        }
        else
        {
            int tickCount = ticksFromSpeed(speed);
            if (speed == GameSpeed::Quarter)
                fractionalSpeedTicksLeft = 3;
			if (paused)
				tickCount = 0;

            if (puzzleVerificationMode)
                tickCount = 10;

            for (int tickIndex = 0; tickIndex < tickCount; tickIndex++)
            {
                app.state.step(app);
            }
        }
    }
}

void GameController::loadPuzzle(const string &filename, const string &puzzleName)
{
    //if (app.renderer.motionBlurActive() == 0)
    //    app.renderer.initMotionBlur(0.3f, 100);
    //else
    //    app.renderer.initMotionBlur(1.0f, 100);

    // TODO: we can allow copying components between levels, but only after we implment verifying if the components can be built in the other level.
    if (editorMode != EditorMode::LevelEditor)
        app.ui.copyBuffer.clear();

    if (util::endsWith(filename, ".txt"))
    {
        loadLegacyPuzzle(filename);
        return;
    }
    app.state.loadPuzzle(filename, puzzleName);

    /*if (app.state.basePuzzleFilename != "unknown")
    {
        currentPuzzleFilename = app.state.basePuzzleFilename;

		cout << "basePuzzleName: " << app.state.basePuzzleFilename << endl;

        currentCampaignIndex = -1;
        for (int i = 0; i < database().puzzles.size(); i++)
            if (database().puzzles[i].filename == currentPuzzleFilename)
                currentCampaignIndex = i;

		cout << "campaign index: " << currentCampaignIndex << endl;
    }*/

    app.undoBuffer.reset(app.state);
    designActionTaken = true;
    app.canvas.backgroundDirty = true;
    app.ui.selection.empty();

    app.ui.selectedMenuComponent = nullptr;
    app.ui.activePlacementBuffer.clear();
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

const PuzzleInfo& GameController::getActivePuzzle()
{
	//getPuzzleInfo(const string &levelPack, int levelPackIndex)
	return database().getPuzzleInfo(app.state.levelPack, app.state.levelPackPuzzleIndex);
}

void GameController::loadLevelPackPuzzle(const string &levelPack, int newIndex, const string &puzzleFileType)
{
    app.session.saveProgress(app);

    const PuzzleInfo &puzzle = database().getPuzzleInfo(levelPack, newIndex);
    loadPuzzle(params().assetDir + "levels/" + puzzle.filename + ".pzl", "Puzzle " + to_string(puzzle.index + 1) + ": " + puzzle.name);
    app.state.disableEditing();
    app.state.victoryInfo = GameVictoryInfo();
    app.state.puzzleFileType = puzzleFileType;
}

string GameController::providedSolutionFilename(int index)
{
	string suffix = "A";
	if (index < 26) suffix[0] += index;
	return params().assetDir + "providedSolutions/" + getActivePuzzle().filename + "_" + suffix + ".pzl";
}

void GameController::loadCurrentProvidedSolution(int index)
{
    const PuzzleInfo &puzzle = getActivePuzzle();
    const string puzzleFilename = params().assetDir + "levels/" + puzzle.filename + ".pzl";
    const string solutionFilename = providedSolutionFilename(index);

    if (!util::fileExists(puzzleFilename) || !util::fileExists(solutionFilename))
    {
        recordError("File not found!", "Couldn't find the provided solution for " + puzzle.filename + "!");
        return;
    }

	auto info = app.session.getLevelInfo(app.state.levelPack, app.state.levelPackPuzzleIndex);
    if ((info == nullptr || info->state != LevelState::Solved) && !params().godMode)
    {
        recordError("Cannot view solution", "You can't view the provided solution to this level until you have solved it yourself!");
        return;
    }

    //app.controller.loadPuzzle(solutionFilename, "Puzzle " + to_string(puzzle.index + 1) + ": " + puzzle.name + " (example solution " + to_string(index + 1) + ")");
	app.controller.loadPuzzle(solutionFilename, puzzle.name + " (example solution " + to_string(index + 1) + ")");
	app.state.providedSolutionIndex = index;
    app.state.disableEditing();
}

bool GameController::userSolutionExists()
{
    const PuzzleInfo &puzzle = getActivePuzzle();
    const string filenameProgress = app.session.getSolutionFilename(app.state.levelPack, app.state.levelPackPuzzleName, SolutionType::Progress);
    const string filenameRecent = app.session.getSolutionFilename(app.state.levelPack, app.state.levelPackPuzzleName, SolutionType::Recent);
    return util::fileExists(filenameProgress) || util::fileExists(filenameRecent);
}

void GameController::loadUserSolution()
{
	const PuzzleInfo &puzzle = getActivePuzzle();

    const string filenameProgress = app.session.getSolutionFilename(app.state.levelPack, app.state.levelPackPuzzleName, SolutionType::Progress);
    const string filenameRecent = app.session.getSolutionFilename(app.state.levelPack, app.state.levelPackPuzzleName, SolutionType::Recent);
    if (!util::fileExists(filenameProgress) && !util::fileExists(filenameRecent))
    {
        recordError("Not attempted yet?", "You haven't attempted this level yet!");
        return;
    }

    string description = "your progress";
    string filename = filenameProgress;
    if (util::fileExists(filenameRecent))
    {
        description = "your solution";
        filename = filenameRecent;
    }
    
    //app.controller.loadPuzzle(filename, "Puzzle " + to_string(puzzle.index + 1) + ": " + puzzle.name + " (" + description + ")");
	app.controller.loadPuzzle(filename, puzzle.name + " (" + description + ")");
}

void GameController::recordDesignAction()
{
    app.state.updateAll();
    app.canvas.backgroundDirty = true;
    designActionTaken = true;
    puzzleVerificationMode = false;
}

void GameController::recordError(const string &title, const string &description)
{
    tooltipErrorTitle = title;
    tooltipErrorDescription = description;
    app.canvas.errorResetBuffer = true;
    app.audio.playEffect("Error");
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
    affinityMenu = false;
    gateMenu = false;
    trapMenu = false;
    wireSpeedMenu = false;
    colorMenu = false;
    circuitBoundaryMenu = false;
    transformMenu = false;
	editMenu = false;
    victoryPanel = false;

    auto sessionInfo = app.session.getLevelInfo(app.state.levelPack, app.state.levelPackPuzzleIndex);
    const PuzzleInfo &curPuzzle = app.controller.getActivePuzzle();
    if (sessionInfo != nullptr && sessionInfo->state == LevelState::Solved && app.activeCircuit() == nullptr)
        victoryPanel = true;

    if (victoryPanel)
    {
        buttons.push_back(GameButton("ComponentCost", vec2i(-1, -1), ButtonType::TooltipOnly, ComponentModifiers()));
    }

    Component *singleComponent = app.ui.selection.singleElement();
    if (singleComponent != nullptr && singleComponent->inactiveCircuitMegaHold(app.state))
		singleComponent = &singleComponent->getSuperMegaHoldTarget(app.state);

    if (levelSelectMenu)
    {
        const int gridSize = 11;
        int levelIndex = 0;
        for (int y = 0; y < gridSize; y++)
            for (int x = 0; x < gridSize; x++)
            {
                const UserSessionLevelInfo *info = app.session.getLevelInfo("Campaign", levelIndex);
                if (info == nullptr)
                    continue;

                buttons.push_back(GameButton("ChoosePuzzle", vec2i(x, y), ButtonType::LevelSelect, levelIndex));
                levelIndex++;
            }
    }
    else
    {
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
                if ((app.controller.editorMode == EditorMode::LevelEditor && realInfo->name != "Eraser") ||
                    (app.controller.editorMode == EditorMode::Campaign && realInfo->name == "Eraser") ||
                    app.state.buildableComponents.canBuild(realInfo->name, ComponentModifiers(info)))
                    buttons.push_back(GameButton(realInfo->name, info.menuCoordinate, ButtonType::Component, ComponentModifiers(info)));
            }
        }

		vector<ComponentDefiningProperties> allComponents;
		if (app.ui.activePlacementBuffer.components.size() == 1)
		{
			allComponents.push_back(app.ui.activePlacementBuffer.components[0]);
		}
		else
		{
			if (singleComponent)
				allComponents.push_back(ComponentDefiningProperties(*singleComponent));
			else
			{
				for (Component *c : app.ui.selection.components)
					allComponents.push_back(ComponentDefiningProperties(*c));
			}
		}
		
		bool componentsModifyable = true;
		bool showAffinity = true;
		bool showSpeed = true;
		bool showCircuitBoundary = true;
		bool homogenous = true;
		for (const ComponentDefiningProperties &c : allComponents)
		{
			const ComponentInfo &info = *c.baseInfo;

			if (c.modifiers.puzzleType != ComponentPuzzleType::User)
				componentsModifyable = false;
			if (info.name == "Eraser" || info.name == "Circuit" || info.name == "Blocker" || info.name == "PowerSource") showAffinity = false;
			if (app.state.levelPackPuzzleIndex <= 3) showAffinity = false;
			if (info.name != "Wire") showSpeed = false;
			if (info.name != "CircuitBoundary") showCircuitBoundary = false;
			if (info.name != allComponents[0].baseInfo->name) homogenous = false;
		}
		bool showColor = homogenous && !showSpeed && !showCircuitBoundary;

		//
        // Add color, delay, preference, and boundary buttons
        //
        if (allComponents.size() > 0 && componentsModifyable)
        {
			const ComponentModifiers &referenceModifiers = allComponents[0].modifiers;
            const ComponentInfo &info = *allComponents[0].baseInfo;

            if (showAffinity)
            {
                for (int affinity = 0; affinity <= 4; affinity++)
                {
                    affinityMenu = true;
                    buttons.push_back(GameButton(info.name, vec2i(affinity, 0), ButtonType::ChargePreference, ComponentModifiers(referenceModifiers.color, affinity)));
                }
            }

            if (showSpeed)
            {
                int speedCount = 0;
                for (int speed = 0; speed <= 4; speed++)
                {
                    ComponentModifiers modifier(ChargeType::None, 2, (WireType)speed);
                    if (app.controller.editorMode == EditorMode::LevelEditor || app.state.buildableComponents.canBuild(info.name, modifier))
                        speedCount++;
                }

                if (speedCount > 1)
                    for (int speed = 0; speed <= 4; speed++)
                    {
                        ComponentModifiers modifier(ChargeType::None, 2, (WireType)speed);
                        if (app.controller.editorMode == EditorMode::LevelEditor || app.state.buildableComponents.canBuild(info.name, modifier))
                        {
                            wireSpeedMenu = true;
                            buttons.push_back(GameButton("Wire", vec2i((int)speed, 0), ButtonType::WireSpeed, modifier));
                        }
                    }
            }
			if (showCircuitBoundary)
            {
                circuitBoundaryMenu = true;
                buttons.push_back(GameButton("CircuitBoundary", vec2i(0, 0), ButtonType::CircuitBoundary, ComponentModifiers(ChargeType::None, 2, WireType::Standard, CircuitBoundaryType::Open)));
                buttons.push_back(GameButton("CircuitBoundary", vec2i(1, 0), ButtonType::CircuitBoundary, ComponentModifiers(ChargeType::None, 2, WireType::Standard, CircuitBoundaryType::Closed)));
                buttons.push_back(GameButton("CloseAll", vec2i(2, 0), ButtonType::CircuitBoundary, ComponentModifiers()));
            }
            if(showColor)
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
                    {
                        colorMenu = true;
                        buttons.push_back(GameButton(info.name, vec2i(chargeIndex, 0), ButtonType::ChargeColor, ComponentModifiers(charge)));
                    }
                    chargeIndex++;
                }

                //
                // some components have a toggleable state
                //
                if (info.name == "TrapSprung" || info.name == "TrapOpen")
                {
                    trapMenu = true;
                    buttons.push_back(GameButton("TrapOpen", vec2i(0, 0), ButtonType::TrapState, ComponentModifiers(referenceModifiers.color)));
                    buttons.push_back(GameButton("TrapSprung", vec2i(1, 0), ButtonType::TrapState, ComponentModifiers(referenceModifiers.color)));
                }
                if (info.name == "GateOpen" || info.name == "GateClosed")
                {
                    gateMenu = true;
                    buttons.push_back(GameButton("GateOpen", vec2i(0, 0), ButtonType::GateState, ComponentModifiers(referenceModifiers.color)));
                    buttons.push_back(GameButton("GateClosed", vec2i(1, 0), ButtonType::GateState, ComponentModifiers(referenceModifiers.color)));
                }
            }
        }
    }

    //
    // Add puzzle control buttons
    //
	if (app.controller.puzzleMode == PuzzleMode::Design)
		buttons.push_back(GameButton("Start", vec2i(0, 0), ButtonType::PuzzleControlA, ComponentModifiers()));
	if (app.controller.puzzleMode == PuzzleMode::Executing)
	{
		buttons.push_back(GameButton("Pause", vec2i(0, 0), ButtonType::PuzzleControlA, ComponentModifiers()));
		buttons.push_back(GameButton("Stop", vec2i(1, 0), ButtonType::PuzzleControlA, ComponentModifiers()));
	}
    
    buttons.push_back(GameButton("Save", vec2i(0, 0), ButtonType::PuzzleControlC, ComponentModifiers()));
    buttons.push_back(GameButton("Load", vec2i(1, 0), ButtonType::PuzzleControlC, ComponentModifiers()));

    buttons.push_back(GameButton("Music", vec2i(0, 1), ButtonType::PuzzleControlC, ComponentModifiers()));
    buttons.push_back(GameButton("SoundEffect", vec2i(1, 1), ButtonType::PuzzleControlC, ComponentModifiers()));
    
    buttons.push_back(GameButton("PrevLevel", vec2i(0, 0), ButtonType::PuzzleControlE, ComponentModifiers()));
    buttons.push_back(GameButton("NextLevel", vec2i(1, 0), ButtonType::PuzzleControlE, ComponentModifiers()));
    buttons.push_back(GameButton("LevelSelect", vec2i(0, 1), ButtonType::PuzzleControlE, ComponentModifiers()));

    auto levelInfo = app.session.getLevelInfo(app.state.levelPack, app.state.levelPackPuzzleIndex);
    if (levelInfo != nullptr)
    {
        if (levelInfo->state == LevelState::Solved || params().godMode)
        {
            buttons.push_back(GameButton("ViewProvidedSolution", vec2i(1, 1), ButtonType::PuzzleControlE, ComponentModifiers()));
			buttons.push_back(GameButton("ViewYourSolution", vec2i(2, 1), ButtonType::PuzzleControlE, ComponentModifiers()));
        }
        else if (util::fileExists(app.session.getSolutionFilename(app.state.levelPack, app.state.levelPackPuzzleName, SolutionType::Progress)))
        {
			buttons.push_back(GameButton("ClearPuzzle", vec2i(1, 1), ButtonType::PuzzleControlE, ComponentModifiers()));
            buttons.push_back(GameButton("ViewYourProgress", vec2i(2, 1), ButtonType::PuzzleControlE, ComponentModifiers()));
        }
        else
        {
            buttons.push_back(GameButton("ClearPuzzle", vec2i(1, 1), ButtonType::PuzzleControlE, ComponentModifiers()));
        }
    }

    if (params().godMode)
    {
        buttons.push_back(GameButton("ModePuzzle", vec2i(0, -1), ButtonType::PuzzleControlA, ComponentModifiers()));
        buttons.push_back(GameButton("ModeLevelEditor", vec2i(1, -1), ButtonType::PuzzleControlA, ComponentModifiers()));
    }

    if (app.ui.activePlacementBuffer.components.size() >= 2 || (singleComponent != nullptr && singleComponent->isCircuit() && singleComponent->modifiers.puzzleType != ComponentPuzzleType::PuzzlePiece))
    {
		transformMenu = true;
        buttons.push_back(GameButton("CircuitRotate90", vec2i(0, 0), ButtonType::PuzzleControlD, ComponentModifiers()));
        buttons.push_back(GameButton("CircuitRotateN90", vec2i(1, 0), ButtonType::PuzzleControlD, ComponentModifiers()));
        buttons.push_back(GameButton("CircuitFlipVertical", vec2i(2, 0), ButtonType::PuzzleControlD, ComponentModifiers()));
        buttons.push_back(GameButton("CircuitFlipHorizontal", vec2i(3, 0), ButtonType::PuzzleControlD, ComponentModifiers()));
    }
	else
	{
		editMenu = true;
		buttons.push_back(GameButton("CircuitCopy", vec2i(0, 0), ButtonType::PuzzleControlF, ComponentModifiers()));
		buttons.push_back(GameButton("CircuitCut", vec2i(1, 0), ButtonType::PuzzleControlF, ComponentModifiers()));
		buttons.push_back(GameButton("CircuitPaste", vec2i(2, 0), ButtonType::PuzzleControlF, ComponentModifiers()));
		buttons.push_back(GameButton("Undo", vec2i(3, 0), ButtonType::PuzzleControlF, ComponentModifiers()));
	}

    for (int speed = (int)GameSpeed::Quarter; speed <= (int)GameSpeed::x10; speed++)
        buttons.push_back(GameButton(buttonNameFromSpeed((GameSpeed)speed), vec2i(speed, 0), ButtonType::PuzzleControlB, ComponentModifiers()));

    
    //
    // Add indicators
    //
    if (singleComponent != nullptr && app.controller.editorMode == EditorMode::LevelEditor)
    {
        if (singleComponent->info->name == "PowerSource")
        {
            buttons.push_back(GameButton("TotalCharge", vec2i(0, 0), ButtonType::ComponentAttribute, "Total charges: " + to_string(singleComponent->intrinsics.totalCharges)));
            buttons.push_back(GameButton("FirstEmission", vec2i(0, 1), ButtonType::ComponentAttribute, "First charge at " + to_string(singleComponent->intrinsics.secondsBeforeFirstEmission) + "s"));
            buttons.push_back(GameButton("EmissionFrequency", vec2i(0, 2), ButtonType::ComponentAttribute, "New charge every " + to_string(singleComponent->intrinsics.secondsPerEmission) + "s"));
        }

        if (singleComponent->info->name == "MegaHold")
        {
            const double dischargeFreq = singleComponent->intrinsics.ticksPerDischarge * constants::secondsPerStep;
            buttons.push_back(GameButton("TicksPerDischarge", vec2i(0, 0), ButtonType::ComponentAttribute, "Discharge every: " + util::formatDouble(dischargeFreq, 2, true) + "s"));
            buttons.push_back(GameButton("ChargesLostPerDischarge", vec2i(0, 1), ButtonType::ComponentAttribute, "Discharge size: " + to_string(singleComponent->intrinsics.chargesLostPerDischarge)));
            buttons.push_back(GameButton("TotalChargeRequired", vec2i(0, 2), ButtonType::ComponentAttribute, "Total charge needed: " + to_string(singleComponent->intrinsics.totalChargeRequired)));
        }
    }
}

void GameController::recordVictory()
{
    app.renderer.initMotionBlur(0.4f, 200);

    //speed = GameSpeed::x1;
    app.ui.selectedMenuComponent = nullptr;
    app.ui.activePlacementBuffer.clear();
    app.ui.selection.empty();
    app.canvas.backgroundDirty = true;

    //if (viewMode != ControllerViewMode::ProvidedSolution)
    //BasePuzzle,ProvidedSolution,UserProgress,UserSolution,Custom
    if (app.state.puzzleFileType == "BasePuzzle" || app.state.puzzleFileType == "UserProgress" || app.state.puzzleFileType == "UserSolution")
    {
        app.session.recordVictory(app);
    }

    /*if (puzzleVerificationMode)
    {
        currentCampaignIndex = math::mod(currentCampaignIndex + 1, database().puzzles.size());
        loadCurrentProvidedSolution();
    }*/
}
