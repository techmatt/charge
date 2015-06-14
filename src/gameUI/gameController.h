
enum class PuzzleMode
{
    Design,
    Executing,
};

enum class EditorMode
{
    Campaign,
    LevelEditor,
};

enum class ControllerViewMode
{
    ProvidedSolution,
    BasePuzzle,
    UserSolutionRecent,
    UserSolutionBestTime,
    UserSolutionFewestPieces,
};

enum class GameSpeed
{
    x0,
    Quarter,
    x1,
    x3,
    x5,
    x10,
};

inline int ticksFromSpeed(GameSpeed speed)
{
    switch (speed)
    {
    case GameSpeed::x0: return 0;
    case GameSpeed::Quarter: return 1;
    case GameSpeed::x1: return 1;
    case GameSpeed::x3: return 3;
    case GameSpeed::x5: return 5;
    case GameSpeed::x10: return 10;
    default: return 0;
    }
}

inline string buttonNameFromSpeed(GameSpeed speed)
{
    switch (speed)
    {
    case GameSpeed::x0: return "speedPause";
    case GameSpeed::Quarter: return "speedQuarter";
    case GameSpeed::x1: return "speedX1";
    case GameSpeed::x3: return "speedX3";
    case GameSpeed::x5: return "speedX5";
    case GameSpeed::x10: return "speedX10";
    default: return 0;
    }
}

class GameController
{
public:
    GameController(AppData &_app) : app(_app) {}
    void init();
    void step();

    void recordVictory();

    void loadLegacyPuzzle(const string &filename);
    void loadPuzzle(const string &filename, const string &puzzleName);
    
    PuzzleInfo& getCurrentPuzzle();

    void loadCurrentPuzzle();
    void loadCurrentProvidedSolution();
    void cycleUserSolution();
    void loadUserSolution();

    void recordDesignAction();
    
    void updateButtonList();

    void changeEditorMode(EditorMode newMode);

    void recordError(const string &title, const string &description);

    const GameButton* getHitButton(const vec2i &mouse) const
    {
        const GameButton *hitButton = nullptr;
        for (const auto &button : buttons)
        {
            const rect2f screenRect = GameUtil::canonicalToWindow(GameUtil::getCanonicalSize(), button.canonicalRect);
            if (screenRect.intersects(vec2f((float)mouse.x, (float)mouse.y)))
            {
                hitButton = &button;
            }
        }
        return hitButton;
    }

    vector<GameButton> buttons;

    PuzzleMode puzzleMode;
    EditorMode editorMode;
    ControllerViewMode viewMode;

    GameSpeed speed;
    bool designActionTaken;

    int currentPuzzleIndex;

    int fractionalSpeedTicksLeft;

    // mode that rapidly goes through all levels and tries to solve them.
    // not intended as a release feature.
    bool puzzleVerificationMode;

    string tooltipErrorTitle;
    string tooltipErrorDescription;

private:
    AppData &app;
};