
#include "main.h"

void GameController::init()
{
    puzzleMode = ModeDesign;
    speed = Speed1x;
    designActionTaken = false;
    editorMode = ModeEditLevel;
    currentPuzzleIndex = 0;
    fractionalSpeedTicksLeft = 0;

    loadCurrentPuzzle();
}

void GameController::step()
{
    if (designActionTaken && puzzleMode != ModeDesign)
    {
        puzzleMode = ModeDesign;
        app.state.resetPuzzle();
    }

    if (puzzleMode == ModeExecuting)
    {
        if (fractionalSpeedTicksLeft)
        {
            fractionalSpeedTicksLeft--;
        }
        else
        {
            const int tickCount = ticksFromSpeed(speed);
            if (speed == SpeedQuarter)
                fractionalSpeedTicksLeft = 3;
            for (int tickIndex = 0; tickIndex < tickCount; tickIndex++)
            {
                app.state.step(app);
            }
        }

        if (app.state.victory)
        {
            speed = Speed1x;
            const string defaultSolution = params().assetDir + "providedSolutions/" + database().puzzles[app.controller.currentPuzzleIndex].filename + "_A.pzl";
            if (!util::fileExists(defaultSolution))
            {
                app.state.savePuzzle(defaultSolution);
            }
        }
    }
}

void GameController::loadPuzzle(const string &filename, const string &puzzleName)
{
    app.state.loadPuzzle(filename, puzzleName);
    designActionTaken = true;
}

void GameController::loadLegacyPuzzle(const string &filename)
{
    LegacyLoader::load(filename, app.state);
    designActionTaken = true;
}

void GameController::loadCurrentPuzzle()
{
    const PuzzleInfo &puzzle = database().puzzles[app.controller.currentPuzzleIndex];
    app.controller.loadPuzzle(params().assetDir + "levels/" + puzzle.filename + ".pzl", "Puzzle " + to_string(puzzle.index) + ": " + puzzle.name);
    app.ui.backgroundDirty = true;
    app.ui.selection.empty();
}

void GameController::recordDesignAction()
{
    app.state.updateAll();
    designActionTaken = true;
}
