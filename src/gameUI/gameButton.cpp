
#include "main.h"

// only used for GetAsyncKeyState
#include <Windows.h>

void GameButton::initTooltip()
{
    tooltip = component;
    hotkeyCode = 0;
    hotkey = "!";
    if (tooltip == nullptr)
        return;
    hotkey = component->hotkey;
    if (hotkey.size() == 0)
        hotkey = "!";

    if (type == ButtonType::ChargePreference)
    {
        tooltip = &database().getComponent("Preference" + to_string((int)modifiers.chargePreference));
        switch ((int)modifiers.chargePreference)
        {
        case 0: hotkey = "7"; break;
        case 1: hotkey = "8"; break;
        case 2: hotkey = "9"; break;
        case 3: hotkey = "0"; break;
        case 4: hotkey = "-"; break;
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

    if (type == ButtonType::TrapState || type == ButtonType::GateState)
    {
        if (component->name == "TrapOpen" || component->name == "GateOpen") hotkey = "Y";
        if (component->name == "TrapSprung" || component->name == "GateClosed") hotkey = "U";
    }

    if (type == ButtonType::CircuitBoundary)
    {
        if (modifiers.boundary == CircuitBoundaryType::Open) hotkey = "1";
        if (modifiers.boundary == CircuitBoundaryType::Closed) hotkey = "2";
    }

    if (hotkey[0] >= 'A' && hotkey[0] <= 'Z')
        hotkeyCode = SDLK_a + hotkey[0] - 'A';

    if (hotkey[0] >= '0' && hotkey[0] <= '9')
        hotkeyCode = SDLK_0 + hotkey[0] - '0';

    if (hotkey[0] == '-')
        hotkeyCode = SDLK_MINUS;
}

void GameButton::leftClick(AppData &app, Component *selectedComponent) const
{
    if (type == ButtonType::Component)
    {
        app.ui.selectedMenuComponent = component;
        app.ui.selectedMenuComponentColor = modifiers.color;

        app.ui.activePlacementBuffer.clear();
        app.ui.activePlacementBuffer = ComponentSet(app.ui.selectedMenuComponent, app.ui.selectedMenuComponentColor);
    }

    if (selectedComponent != nullptr && selectedComponent->modifiers.puzzleType == ComponentPuzzleType::User)
    {
        if (type == ButtonType::ChargeColor)
        {
            selectedComponent->modifiers.color = modifiers.color;
            app.controller.recordDesignAction();
        }
        if (type == ButtonType::ChargePreference)
        {
            selectedComponent->modifiers.chargePreference = modifiers.chargePreference;
            app.controller.recordDesignAction();
        }
        if (type == ButtonType::WireSpeed)
        {
            selectedComponent->modifiers.speed = modifiers.speed;
            app.controller.recordDesignAction();
        }
        if (type == ButtonType::CircuitBoundary)
        {
            selectedComponent->modifiers.boundary = modifiers.boundary;
            app.controller.recordDesignAction();
        }
        if (type == ButtonType::GateState || type == ButtonType::TrapState)
        {
            selectedComponent->baseInfo = selectedComponent->info = &database().getComponent(name);
            app.controller.recordDesignAction();
        }
    }
    if (name == "Start")
    {
        app.controller.designActionTaken = false;
        app.controller.puzzleMode = PuzzleMode::Executing;
        app.state.resetPuzzle();
        if (app.controller.speed == GameSpeed::x0)
            app.controller.speed = GameSpeed::x1;
    }
    if (name == "Stop")
    {
        app.controller.puzzleMode = PuzzleMode::Design;
        app.state.resetPuzzle();
    }
    if (name == "Pause")
    {
        app.controller.speed = GameSpeed::x0;
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

        if (GetAsyncKeyState(VK_SHIFT))
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
    if (name == "Save")
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
    if (name == "Load")
    {
        const string filename = FileDialog::showOpen();
        if (filename.size() > 0)
        {
            app.state.loadPuzzle(filename, util::removeExtensions(util::fileNameFromPath(filename)));
            app.undoBuffer.reset(app.state);
        }
    }
    if (name == "CircuitRotateN90")
        app.ui.activePlacementBuffer.rotate(1);
    if (name == "CircuitRotate90")
        app.ui.activePlacementBuffer.rotate(3);
    if (name == "CircuitFlipHorizontal")
        app.ui.activePlacementBuffer.flipAboutVerical();
    if (name == "CircuitFlipVertical")
        app.ui.activePlacementBuffer.flipAboutHorizonal();

    for (int speed = (int)GameSpeed::x0; speed <= (int)GameSpeed::x5; speed++)
        if (name == buttonNameFromSpeed((GameSpeed)speed))
        {
            app.controller.speed = (GameSpeed)speed;
        }
}