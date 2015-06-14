
enum LevelState
{
    Unsolved = 0,
    Solved = 1,
};

enum SolutionType
{
    MostRecent,
    BestStepCount,
    BestPiecesUsed,
};

struct UserSessionLevelInfo
{
    UserSessionLevelInfo()
    {
        state = LevelState::Unsolved;
        filename = "none";

        bestStepCount = std::numeric_limits<int>::max();
        bestPiecesUsed = std::numeric_limits<int>::max();
    }

    ParameterTable toTable(const string &tableName) const
    {
        ParameterTable table(tableName);
        table.setString("filename", filename);
        table.setInt("state", (int)state);
        table.setInt("bestStepCount", bestStepCount);
        table.setInt("bestPiecesUsed", bestPiecesUsed);
        return table;
    }
    static UserSessionLevelInfo fromTable(const ParameterTable &table)
    {
        UserSessionLevelInfo result;
        result.filename = table.getString("filename");
        result.state = (LevelState)table.getInt("state");
        result.bestStepCount = table.getInt("bestStepCount");
        result.bestPiecesUsed = table.getInt("bestPiecesUsed");
        return result;
    }

    string filename;
    LevelState state;
    int bestStepCount;
    int bestPiecesUsed;
};

//
// information on a single user session. Automatically saved and loaded at each level transition.
//
struct UserSession
{
    void init(int slotIndex);

    int activePuzzle() const
    {
        int result = 0;
        for (int i = 0; i < campaignLevels.size(); i++)
            if (campaignLevels[i].state == LevelState::Solved)
                result = i;
        return min(result + 1, (int)campaignLevels.size());
    }

    string dataFile() const;
    void save();
    void load();

    void recordVictoryCampaign(AppData &app);

    // this folder contains your custom data and your solutions.
    // typically, this is just "SessionA", "SessionB", or "SessionC"
    string folder;

    string getSolutionFilename(const string &puzzleName, bool campaign, SolutionType type) const;

    vector<UserSessionLevelInfo> campaignLevels;

    map<string, UserSessionLevelInfo> customLevels;

    bool playMusic;
    bool playSounds;
};
