
#include "main.h"

void GameButton::initTooltip()
{
    tooltip = component;
	modifierKey = ModifierKey::None;

    hotkeyCode = 0;
    hotkey = "!";
    if (tooltip == nullptr)
        return;
    hotkey = component->hotkey;
    if (hotkey.size() == 0)
        hotkey = "!";
	if (util::contains(hotkey, "Alt"))
	{
		modifierKey = ModifierKey::Alt;
		hotkey = util::remove(hotkey, "Alt");
	}
	if (util::contains(hotkey, "Shift"))
	{
		modifierKey = ModifierKey::Shift;
		hotkey = util::remove(hotkey, "Shift");
	}
	if (util::contains(hotkey, "Ctrl"))
	{
		modifierKey = ModifierKey::Ctrl;
		hotkey = util::remove(hotkey, "Ctrl");
	}

    if (hotkey == "Enter") hotkeyCode = SDLK_RETURN;
    if (hotkey == "Left") hotkeyCode = SDLK_LEFT;
    if (hotkey == "Right") hotkeyCode = SDLK_RIGHT;
    if (hotkey == "Up") hotkeyCode = SDLK_UP;
    if (hotkey == "Down") hotkeyCode = SDLK_DOWN;

	if (hotkey == "F1") hotkeyCode = SDLK_F1;
	if (hotkey == "F2") hotkeyCode = SDLK_F2;
	if (hotkey == "F3") hotkeyCode = SDLK_F3;
	if (hotkey == "F4") hotkeyCode = SDLK_F4;
	if (hotkey == "F5") hotkeyCode = SDLK_F5;
	if (hotkey == "F6") hotkeyCode = SDLK_F6;

    if (type == ButtonType::ChargePreference)
    {
        tooltip = &database().getComponent("Preference" + to_string((int)modifiers.chargePreference));
        switch ((int)modifiers.chargePreference)
        {
		case 0: hotkey = "1"; modifierKey = ModifierKey::Shift; break;
        case 1: hotkey = "2"; modifierKey = ModifierKey::Shift; break;
        case 2: hotkey = "3"; modifierKey = ModifierKey::Shift; break;
        case 3: hotkey = "4"; modifierKey = ModifierKey::Shift; break;
        case 4: hotkey = "5"; modifierKey = ModifierKey::Shift; break;
        }
    }

    if (type == ButtonType::WireSpeed)
    {
        tooltip = &database().getComponent(GameUtil::speedToTextureName(modifiers.speed));
        switch ((int)modifiers.speed)
        {
        case 0: hotkey = "1"; break;
        case 1: hotkey = "2"; break;
        case 2: hotkey = "3"; break;
        case 3: hotkey = "4"; break;
        case 4: hotkey = "5"; break;
        }
    }

    if (type == ButtonType::ChargeColor)
        hotkey[0] = '0' + (int)modifiers.color;

    if (type == ButtonType::Component && modifiers.color == ChargeType::Gray)
        hotkey = database().getComponent(name + "GrayProxy").hotkey;

    if (modifiers.color == ChargeType::Gray && (component->name == "GateSwitch" || component->name == "TrapReset" || component->name == "MegaHold"))
        tooltip = &database().getComponent(component->name + "GrayProxy");

	if (modifiers.color == ChargeType::Red && (component->name == "TeleportSource" || component->name == "TeleportDestination" || component->name == "GateClosed"))
		tooltip = &database().getComponent(component->name + "Rainbow");

    if (type == ButtonType::TrapState || type == ButtonType::GateState)
    {
        if (component->name == "TrapOpen" || component->name == "GateOpen") hotkey = "~";
        if (component->name == "TrapSprung" || component->name == "GateClosed") hotkey = "~";
    }

    if (type == ButtonType::CircuitBoundary)
    {
        if (modifiers.boundary == CircuitBoundaryType::Open) hotkey = "1";
        if (modifiers.boundary == CircuitBoundaryType::Closed) hotkey = "2";
        if (name == "CloseAll") hotkey = "3";
    }

    if (hotkey.size() == 1 && hotkey[0] >= 'A' && hotkey[0] <= 'Z')
        hotkeyCode = SDLK_a + hotkey[0] - 'A';

    if (hotkey.size() == 1 && hotkey[0] >= '0' && hotkey[0] <= '9')
        hotkeyCode = SDLK_0 + hotkey[0] - '0';

    //if (hotkey[0] == '-')
        //hotkeyCode = SDLK_MINUS;

	if (modifierKey == ModifierKey::Alt) hotkey = "Alt+" + hotkey;
	if (modifierKey == ModifierKey::Shift) hotkey = "Shift+" + hotkey;
	if (modifierKey == ModifierKey::Ctrl) hotkey = "Ctrl+" + hotkey;

    if (tooltip == nullptr && database().hasComponent(name))
        tooltip = &database().getComponent(name);
}

void GameButton::leftClick(AppData &app, const vector<Component*> &selectedComponents) const
{
    if (type == ButtonType::Component)
    {
        app.ui.selectedMenuComponent = component;
        app.ui.selectedMenuComponentColor = modifiers.color;

        app.ui.activePlacementBuffer.clear();
        app.ui.activePlacementBuffer = ComponentSet(app.ui.selectedMenuComponent, modifiers.color);
    }

	if (app.ui.activePlacementBuffer.components.size() == 1)
	{
		ComponentDefiningProperties &component = app.ui.activePlacementBuffer.components[0];
		if (type == ButtonType::ChargeColor)
		{
			component.modifiers.color = modifiers.color;
			app.controller.recordDesignAction();
		}
		if (type == ButtonType::ChargePreference)
		{
			component.modifiers.chargePreference = modifiers.chargePreference;
			app.controller.recordDesignAction();
		}
		if (type == ButtonType::WireSpeed)
		{
			component.modifiers.speed = modifiers.speed;
			app.controller.recordDesignAction();
		}
		if (type == ButtonType::GateState || type == ButtonType::TrapState)
		{
			component.baseInfo = &database().getComponent(name);
			app.controller.recordDesignAction();
		}
	}
    else if (selectedComponents.size() > 0 && selectedComponents[0]->modifiers.puzzleType == ComponentPuzzleType::User)
    {
		if (type == ButtonType::ChargeColor)
		{
			for (Component *c : selectedComponents)
				if (c->modifiers.puzzleType == ComponentPuzzleType::User)
					c->modifiers.color = modifiers.color;
			app.controller.recordDesignAction();
		}
		if (type == ButtonType::ChargePreference)
		{
			for (Component *c : selectedComponents)
				if (c->modifiers.puzzleType == ComponentPuzzleType::User)
					c->modifiers.chargePreference = modifiers.chargePreference;
			app.controller.recordDesignAction();
		}
		if (type == ButtonType::WireSpeed)
		{
			for (Component *c : selectedComponents)
				if (c->modifiers.puzzleType == ComponentPuzzleType::User)
					c->modifiers.speed = modifiers.speed;
			app.controller.recordDesignAction();
		}
		if (type == ButtonType::GateState || type == ButtonType::TrapState)
		{
			for (Component *c : selectedComponents)
				if (c->modifiers.puzzleType == ComponentPuzzleType::User)
					c->baseInfo = c->info = &database().getComponent(name);
			app.controller.recordDesignAction();
		}
		if (type == ButtonType::CircuitBoundary)
		{
			if (name == "CloseAll")
			{
				if (app.activeCircuit())
				{
					for (auto &component : app.state.components)
					{
						if (component->location.boardPos == app.activeCircuit()->location.boardPos &&
							component->inactiveBoundary())
						{
							component->modifiers.boundary = CircuitBoundaryType::Closed;
						}
					}
				}
			}
			else
			{
				for (Component *c : selectedComponents)
					if (c->modifiers.puzzleType == ComponentPuzzleType::User)
						c->modifiers.boundary = modifiers.boundary;
			}
			app.controller.recordDesignAction();
		}
    }
    if (name == "Start")
    {
        app.controller.designActionTaken = false;
        app.controller.puzzleMode = PuzzleMode::Executing;
		app.controller.paused = false;
		app.state.resetPuzzle();
        app.renderer.initMotionBlur(0.5f, 5);
    }
    if (name == "Stop")
    {
        app.controller.puzzleMode = PuzzleMode::Design;
        app.state.resetPuzzle();
    }
    if (name == "Pause")
    {
		app.controller.paused = !app.controller.paused;
    }
    if (name == "ModePuzzle")
    {
        app.controller.puzzleVerificationMode = false;
        app.controller.changeEditorMode(EditorMode::Campaign);
    }
    if (name == "ModeLevelEditor")
    {
        app.controller.puzzleVerificationMode = false;
        app.controller.changeEditorMode(EditorMode::LevelEditor);

        if (SDL_GetModState() & KMOD_SHIFT)
        {
            app.controller.puzzleVerificationMode = true;
            app.controller.loadCurrentProvidedSolution();
        }
    }
    if (name == "Music")
    {
        app.audio.setMusic(!app.audio.playMusic);
    }
    if (name == "SoundEffect")
    {
        app.audio.playSoundEffects = !app.audio.playSoundEffects;
    }
    if (name == "LevelSelect")
    {
        app.controller.levelSelectMenu = true;
        app.ui.clearSelection();
        if (app.controller.puzzleMode != PuzzleMode::Design)
        {
            app.controller.puzzleMode = PuzzleMode::Design;
            app.state.resetPuzzle();
        }
    }
    if (name == "Save")
    {
        string filename = FileDialog::showSave();
        if (filename.size() > 0)
        {
            if (util::endsWith(filename, ".pzl"))
                filename = util::remove(filename, ".pzl");
            filename = util::replace(filename, '.', ' ');
            filename = util::replace(filename, ',', ' ');
            //const string puzzleName = util::fileNameFromPath(filename);
            filename += ".pzl";
            app.state.savePuzzle(filename);
        }
    }
    if (name == "Load")
    {
        const string filename = FileDialog::showOpen();
        if (filename.size() > 0)
        {
            app.controller.loadPuzzle(filename, util::removeExtensions(util::fileNameFromPath(filename)));
            app.undoBuffer.reset(app.state);
        }
    }

    if (name == "PrevLevel")
    {
		int newPuzzleIndex = 0;
		if (app.state.levelPackPuzzleIndex == 0)
			newPuzzleIndex = (int)app.session.highestAccessiblePuzzle();
		else
			newPuzzleIndex = math::max(app.state.levelPackPuzzleIndex - 1, 0);
			
        app.controller.loadLevelPackPuzzle("Campaign", newPuzzleIndex, "BasePuzzle");
    }

    if (name == "NextLevel")
    {
        if (!params().godMode &&
            (app.state.levelPackPuzzleIndex != (int)app.session.campaignLevels.size() - 1) &&
            app.state.levelPackPuzzleIndex >= app.session.highestAccessiblePuzzle())
            app.controller.recordError("Not enough puzzles completed!", "You can only skip up to three puzzles ahead.  Try going back and beating an earlier puzzle.");
        else
        {
            int newPuzzleIndex = math::mod(app.state.levelPackPuzzleIndex + 1, database().allPuzzles.size());
            app.controller.loadLevelPackPuzzle("Campaign", newPuzzleIndex, "BasePuzzle");
        }
    }

    if (name == "ViewProvidedSolution")
    {
        app.controller.loadCurrentProvidedSolution();
    }

	if (name == "ViewYourProgress" || name == "ViewYourSolution")
	{
		//app.controller.viewMode = ControllerViewMode::UserSolution;
		app.controller.loadUserSolution();
	}

    if (name == "ClearPuzzle")
    {
        const PuzzleInfo &puzzle = app.controller.getActivePuzzle();
        app.controller.loadPuzzle(params().assetDir + "levels/" + puzzle.filename + ".pzl", "Puzzle " + to_string(puzzle.index) + ": " + puzzle.name);
        app.state.disableEditing();
        app.state.victoryInfo = GameVictoryInfo();
        //app.controller.viewMode = ControllerViewMode::BasePuzzle;
    }

    auto transformComponentSet = [&](ComponentSet &cSet)
    {
        if (name == "CircuitRotateN90")
            cSet.rotate(1);
        if (name == "CircuitRotate90")
            cSet.rotate(3);
        if (name == "CircuitFlipHorizontal")
            cSet.flipAboutVerical();
        if (name == "CircuitFlipVertical")
            cSet.flipAboutHorizonal();
    };

    if (app.ui.activePlacementBuffer.isEmpty())
    {
		bool selectionHasCircuit=false;
		for (Component *c : app.ui.selection.components) {
			if (c->info->name == "Circuit")
			{
				selectionHasCircuit = true;
				break;
			}
		}


		if (selectionHasCircuit)
		{
			vector<Component*> oldComponents = app.ui.selection.components;
			ComponentSelection tempSelection;
			for (Component *c : oldComponents) {


				if (c->info->name == "Circuit" && c->modifiers.puzzleType != ComponentPuzzleType::PuzzlePiece &&
					(name == "CircuitRotateN90" || name == "CircuitRotate90" || name == "CircuitFlipHorizontal" || name == "CircuitFlipVertical"))
				{
                    app.controller.recordDesignAction();
					tempSelection.newSelectionFromComponent(c);

					ComponentSet cSet;
					tempSelection.copyToComponentSet(cSet, app.state);

					GameLocation circuitLocation = c->location;
                    transformComponentSet(cSet);

					for (ComponentDefiningProperties &cdf : cSet.components)
						cdf.location.boardPos = circuitLocation.boardPos;

					app.ui.selection.remove(c);
					app.state.removeComponent(c);

					cSet.addToComponents(app.state,vec2i(0,0));
					app.ui.selection.add(app.state.getComponent(circuitLocation));
				}
			}

			app.undoBuffer.save(app.state);
		}
    }
    else
    {
        transformComponentSet(app.ui.activePlacementBuffer);
    }

    for (int speed = (int)GameSpeed::Quarter; speed <= (int)GameSpeed::x10; speed++)
        if (name == buttonNameFromSpeed((GameSpeed)speed))
        {
            app.controller.speed = (GameSpeed)speed;
        }

	if (name == "CircuitCopy")
	{
		app.ui.copy();
	}
	if (name == "CircuitPaste")
	{
		app.ui.paste();
	}
	if (name == "CircuitCut")
	{
		app.ui.cut();
	}
	if (name == "Undo")
	{
		app.undoBuffer.back(app.state);
	}

    if (name == "ChoosePuzzle")
    {
        if (levelIndex > app.session.highestAccessiblePuzzle())
        {
            app.controller.recordError("Not enough puzzles completed!", "You can only skip up to three puzzles ahead.  Try going back and beating an earlier puzzle.");
        }
        else
        {
            app.controller.levelSelectMenu = false;
        }
    }
}