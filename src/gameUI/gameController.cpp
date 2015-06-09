
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
