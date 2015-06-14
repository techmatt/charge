
enum LevelState
{
    Unsolved = 0,
    Solved = 1,
};

struct UserSessionLevelInfo
{
    UserSessionLevelInfo()
    {
        state = LevelState::Unsolved;
    }
    LevelState state;
};

//
// information on a single user session. Automatically saved and loaded at each level transition.
//
struct UserSession
{
    void init(int slotIndex);

    void save();
    void load();

    // this folder contains your custom data and your solutions.
    // typically, this is just "SessionA", "SessionB", or "SessionC"
    string folder;

    vector<UserSessionLevelInfo> levels;

    bool playMusic;
    bool playSounds;
};
