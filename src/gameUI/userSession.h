
enum LevelState
{
    Unsolved = 0,
    Solved = 1,
};

enum SolutionType
{
    MostRecent,
    Fastest,
    Cheapest,
};

struct UserSessionLevelInfo
{
    UserSessionLevelInfo()
    {
        state = LevelState::Unsolved;
        filename = "none";

        bestStepCount = std::numeric_limits<int>::max();
        bestComponentCost = std::numeric_limits<int>::max();
    }

    ParameterTable toTable(const string &tableName) const
    {
        ParameterTable table(tableName);
        table.setString("filename", filename);
        table.setInt("state", (int)state);
        table.setInt("bestStepCount", bestStepCount);
        table.setInt("bestComponentCost", bestComponentCost);
        return table;
    }
    static UserSessionLevelInfo fromTable(const ParameterTable &table)
    {
        UserSessionLevelInfo result;
        result.filename = table.getString("filename");
        result.state = (LevelState)table.getInt("state");
        result.bestStepCount = table.getInt("bestStepCount");
        result.bestComponentCost = table.getInt("bestComponentCost");
        return result;
    }

    string filename;
    LevelState state;
    int bestStepCount;
    int bestComponentCost;
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
        return min(result, (int)campaignLevels.size());
    }

    int highestAccessiblePuzzle() const
    {
        int solved = 0;
        for (int i = 0; i < campaignLevels.size(); i++)
            if (campaignLevels[i].state == LevelState::Solved)
                solved++;
        return min(solved + 3, (int)campaignLevels.size());
    }

    string dataFile() const;
    void save();
    void load();

    void recordVictoryCampaign(AppData &app);
    void saveProgress(AppData &app);

    // this folder contains your custom data and your solutions.
    // typically, this is just "SessionA", "SessionB", or "SessionC"
    string folder;

    string getSolutionFilename(const string &puzzleName, bool campaign, SolutionType type) const;

    vector<UserSessionLevelInfo> campaignLevels;

    map<string, UserSessionLevelInfo> customLevels;

    bool playMusic;
    bool playSounds;
};
