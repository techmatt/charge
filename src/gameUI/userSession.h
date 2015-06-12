
struct UserSessionLevelState
{
    bool solved;
};

//
// information on a single user session. Automatically saved and loaded at each level transition or every X frames.
//
struct UserSession
{
    void init();

    void saveToFile();

    // typically, this is just "SessionA", "SessionB", or "SessionC"
    // this folder contains your custom data and your solutions.
    string folder;

    vector<UserSessionLevelState> levels;

    bool playMusic;
    bool playSounds;
};

//
// all session data
//
struct UserSessionSet
{
    vector<UserSession> slots;
};
