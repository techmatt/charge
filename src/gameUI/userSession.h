
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
        puzzleFilename = "none";

        bestStepCount = std::numeric_limits<int>::max();
        bestComponentCost = std::numeric_limits<int>::max();
    }

    ParameterTable toTable(const string &tableName) const
    {
        ParameterTable table(tableName);
        table.setString("puzzleFilename", puzzleFilename);
        table.setInt("state", (int)state);
        table.setInt("bestStepCount", bestStepCount);
        table.setInt("bestComponentCost", bestComponentCost);
        return table;
    }

    static UserSessionLevelInfo fromTable(const ParameterTable &table)
    {
        UserSessionLevelInfo result;
        result.puzzleFilename = table.getString("puzzleFilename");
        result.state = (LevelState)table.getInt("state");
        result.bestStepCount = table.getInt("bestStepCount");
        result.bestComponentCost = table.getInt("bestComponentCost");
        return result;
    }

    string puzzleFilename;
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

    int currentCampaignLevel() const
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

    const UserSessionLevelInfo* getLevelInfo(const string &puzzleFilename) const
    {
        for (int i = 0; i < campaignLevels.size(); i++)
        {
            if (campaignLevels[i].puzzleFilename == puzzleFilename)
                return &campaignLevels[i];
        }

        if (customLevels.count(puzzleFilename) > 0)
            return &customLevels.find(puzzleFilename)->second;

        return nullptr;
    }

    string dataFile() const;
    void save();
    void load();

    void recordVictoryCampaign(AppData &app);
    void saveProgress(AppData &app);

    // this folder contains your custom data and your solutions.
    // typically, this is just "SessionA", "SessionB", or "SessionC"
    string folder;

    string getSolutionFilename(const string &puzzleFilename, SolutionType type) const;

    vector<UserSessionLevelInfo> campaignLevels;

    map<string, UserSessionLevelInfo> customLevels;

    bool playMusic;
    bool playSounds;
};
