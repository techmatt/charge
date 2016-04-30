
enum class LevelState
{
    Unsolved = 0,
    Solved = 1,
};

enum class SolutionType
{
    Progress = 0,
    Recent = 1,
    Fastest = 2,
    Cheapest = 3,
};

struct UserSessionLevelInfo
{
    UserSessionLevelInfo()
    {
        state = LevelState::Unsolved;
        puzzleFilename = "none";

        bestStepCount = std::numeric_limits<int>::max();
    }

    ParameterTable toTable(const string &tableName) const
    {
        ParameterTable table(tableName);
        table.setString("puzzleFilename", puzzleFilename);
        table.setInt("state", (int)state);
        table.setInt("bestStepCount", bestStepCount);
        table.setInt("bestComponentCost", bestComponentCost);
        table.setInt("recentStepCount", recentStepCount);
        table.setInt("recentComponentCost", recentComponentCost);
        return table;
    }

    static UserSessionLevelInfo fromTable(const ParameterTable &table)
    {
        UserSessionLevelInfo result;
        result.puzzleFilename = table.getString("puzzleFilename");
        result.state = (LevelState)table.getInt("state");
        result.bestStepCount = table.getInt("bestStepCount");
        result.bestComponentCost = table.getInt("bestComponentCost");
        result.recentStepCount = table.getInt("recentStepCount");
        result.recentComponentCost = table.getInt("recentComponentCost");
        return result;
    }

    string puzzleFilename;
    LevelState state;
    int bestStepCount;
    int bestComponentCost;
    int recentStepCount;
    int recentComponentCost;
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
        return min(solved + params().maxSkippedLevels, (int)campaignLevels.size() - 1);
    }

    const UserSessionLevelInfo* getLevelInfo(const string &levelPack, int puzzleIndex) const
    {
		if (levelPack == "Campaign")
		{
			return &campaignLevels[puzzleIndex];
		}
        //if (customLevels.count(puzzleFilename) > 0)
        //    return &customLevels.find(puzzleFilename)->second;

        return nullptr;
    }

    string dataFile() const;
    void save();
    void load();

    void recordVictory(AppData &app);
    void saveProgress(AppData &app);

    // this folder contains your custom data and your solutions.
    // typically, this is just "SessionA", "SessionB", or "SessionC"
    string sessionFolder;

    string getSolutionFilename(const string &levelPack, const string &levelPackPuzzleName, SolutionType type) const;

    vector<UserSessionLevelInfo> campaignLevels;

    bool playMusic;
    bool playSounds;
};
