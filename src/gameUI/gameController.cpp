
#include "main.h"

void GameController::init()
{
    puzzleMode = ModeDesign;
    speed = Speed1x;
    designActionTaken = false;
    editorMode = ModeEditLevel;
    currentPuzzleIndex = 0;
    fractionalSpeedTicksLeft = 0;
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

void GameController::recordDesignAction()
{
    app.state.updateAll();
    designActionTaken = true;
}
