
enum class GamePuzzleMode
{
    ModeDesign,
    ModeExecuting,
};

enum class GameEditorMode
{
    ModePlayLevel,
    ModeEditLevel,
};

enum class GameSpeed
{
    Speed0x,
    SpeedQuarter,
    Speed1x,
    Speed3x,
    Speed5x,
};

inline int ticksFromSpeed(GameSpeed speed)
{
    switch (speed)
    {
    case GameSpeed::Speed0x: return 0;
    case GameSpeed::SpeedQuarter: return 1;
    case GameSpeed::Speed1x: return 1;
    case GameSpeed::Speed3x: return 3;
    case GameSpeed::Speed5x: return 5;
    default: return 0;
    }
}

inline string buttonNameFromSpeed(GameSpeed speed)
{
    switch (speed)
    {
    case GameSpeed::Speed0x: return "speedPause";
    case GameSpeed::SpeedQuarter: return "speedQuarter";
    case GameSpeed::Speed1x: return "speedX1";
    case GameSpeed::Speed3x: return "speedX3";
    case GameSpeed::Speed5x: return "speedX5";
    default: return 0;
    }
}

class GameController
{
public:
    GameController(AppData &_app) : app(_app) {}
    void init();
    void step();

    void loadLegacyPuzzle(const string &filename);
    void loadPuzzle(const string &filename, const string &puzzleName);
    
    void loadCurrentPuzzle();
    void loadCurrentProvidedSolution();

    void recordDesignAction();
    
    void updateButtonList();

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

    bool canEdit(const Component &c) const
    {
        return (editorMode == ModeEditLevel || c.modifiers.puzzleType != ComponentPuzzlePiece);
    }

    vector<GameButton> buttons;

    GamePuzzleMode puzzleMode;
    GameEditorMode editorMode;

    GameSpeed speed;
    bool designActionTaken;

    int currentPuzzleIndex;

    int fractionalSpeedTicksLeft;

    // mode that rapidly goes through all levels and tries to solve them.
    // not intended as a release feature.
    bool puzzleVerificationMode;

private:
    AppData &app;
};