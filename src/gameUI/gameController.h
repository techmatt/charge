
enum GamePuzzleMode
{
    ModeDesign,
    ModeExecuting,
};

enum GameEditorMode
{
    ModePlayLevel,
    ModeEditLevel,
};

enum GameSpeed
{
    Speed0x,
    Speed1x,
    Speed2x,
    Speed10x,
    Speed50x,
};

inline int ticksFromSpeed(GameSpeed speed)
{
    switch (speed)
    {
    case Speed0x: return 0;
    case Speed1x: return 1;
    case Speed2x: return 2;
    case Speed10x: return 10;
    case Speed50x: return 50;
    default: return 0;
    }
}

class GameController
{
public:
    GameController(AppData &_app) : app(_app) {}
    void init();
    void step();

    void loadPuzzle(const string &filename);

    void recordDesignAction();
    
    GamePuzzleMode puzzleMode;
    GameEditorMode editorMode;

    GameSpeed speed;
    bool designActionTaken;

    int currentPuzzleIndex;

private:
    AppData &app;
};