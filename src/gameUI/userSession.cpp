#include "main.h"

void UserSession::init(int slotIndex)
{
    string s = "A";
    s[0] += slotIndex;
	sessionFolder = "Session" + s + "/";

    campaignLevels.resize(database().allPuzzles.size());

    for (int i = 0; i < campaignLevels.size(); i++)
    {
        campaignLevels[i].puzzleFilename = database().allPuzzles[i].filename;
    }

    playMusic = false;
    playSounds = true;

    load();
}

void UserSession::save()
{
    if (sessionFolder.size() == 0)
        return;

    ParameterTable table("Session");

    table.setString("sessionFolder", sessionFolder);

    for (int levelIndex = 0; levelIndex < campaignLevels.size(); levelIndex++)
    {
        table.setTable("campaignLevel" + util::zeroPad(levelIndex, 3), campaignLevels[levelIndex].toTable("campaignLevel"));
    }

    /*table.setInt("customLevelCount", (int)customLevels.size());
	int customLevelIndex = 0;
    for (auto &level : customLevels)
    {
        table.setTable("customLevel" + util::zeroPad(customLevelIndex++, 3), level.second.toTable("customLevel"));
    }*/

    table.setBool("playMusic", playMusic);
    table.setBool("playSounds", playSounds);

    table.save(dataFile());
}

void UserSession::load()
{
    if (sessionFolder.size() == 0)
        return;

    if (!util::fileExists(dataFile()))
        return;

    ParameterTable table;
    table.load(dataFile());

	sessionFolder = table.getString("sessionFolder");
    playMusic = table.getBool("playMusic");
    playSounds = table.getBool("playSounds");

    for (int levelIndex = 0; levelIndex < campaignLevels.size(); levelIndex++)
    {
        string paramName = "campaignLevel" + util::zeroPad(levelIndex, 3);
        if (table.hasParameter(paramName))
            campaignLevels[levelIndex] = UserSessionLevelInfo::fromTable(table.getTable(paramName));
    }

    /*int customLevelCount = table.getInt("customLevelCount");
	for (int customLevelIndex = 0; customLevelIndex < customLevelCount; customLevelIndex++)
    {
        string paramName = "customLevel" + util::zeroPad(customLevelIndex, 3);
        if (table.hasParameter(paramName))
        {
            auto info = UserSessionLevelInfo::fromTable(table.getTable(paramName));
            customLevels[info.puzzleFilename] = info;
        }
    }*/

}

string UserSession::dataFile() const
{
    util::makeDirectory(params().rootDir + sessionFolder);
    return params().rootDir + sessionFolder + "progress.dat";
}

string UserSession::getSolutionFilename(const string &levelPack, const string &levelPackPuzzleName, SolutionType type) const
{
    const string subFolder = levelPack + "/";

    util::makeDirectory(params().rootDir + sessionFolder);
    util::makeDirectory(params().rootDir + sessionFolder + subFolder);

    string suffix = "";
    if (type == SolutionType::Progress) suffix = "_Progress";
    if (type == SolutionType::Recent) suffix = "_Recent";
    if (type == SolutionType::Fastest) suffix = "_Fastest";
    if (type == SolutionType::Cheapest) suffix = "_Cheapest";

    return params().rootDir + sessionFolder + subFolder + levelPackPuzzleName + suffix + ".pzl";
}

void UserSession::saveProgress(AppData &app)
{
	int userPieceCount = 0;
    for (Component *c : app.state.components)
        if (c->modifiers.puzzleType != ComponentPuzzleType::PuzzlePiece)
            userPieceCount++;

    cout << "UserPieceCount: " << userPieceCount << endl;
    if (userPieceCount == 0)
        return;

    if (app.state.puzzleFileType != "BasePuzzle" && app.state.puzzleFileType != "UserProgress" && app.state.puzzleFileType != "UserSolution")
		return;

    const string filenameProgress = app.session.getSolutionFilename(app.state.levelPack, app.state.levelPackPuzzleName, SolutionType::Progress);
    const string filenameRecent = app.session.getSolutionFilename(app.state.levelPack, app.state.levelPackPuzzleName, SolutionType::Recent);
    
    app.state.puzzleFileType = "UserProgress";
    app.state.savePuzzle(filenameProgress);

    /*if (app.state.victoryInfo.valid())
	{
        app.state.puzzleFileType = "UserSolution";
        app.state.savePuzzle(filenameRecent);
	}*/
}

void UserSession::recordVictory(AppData &app)
{
    const string filenameRecent = app.session.getSolutionFilename(app.state.levelPack, app.state.levelPackPuzzleName, SolutionType::Recent);
    const string filenameStepCount = app.session.getSolutionFilename(app.state.levelPack, app.state.levelPackPuzzleName, SolutionType::Fastest);
    const string filenameComponentCost = app.session.getSolutionFilename(app.state.levelPack, app.state.levelPackPuzzleName, SolutionType::Cheapest);

	if (app.state.levelPack != "Campaign")
	{
		return;
	}

    auto &info = campaignLevels[app.state.levelPackPuzzleIndex];

    info.state = LevelState::Solved;

    app.state.savePuzzle(filenameRecent);

    if (app.state.victoryInfo.componentCost <= info.bestComponentCost)
    {
        info.bestComponentCost = app.state.victoryInfo.componentCost;
        app.state.savePuzzle(filenameComponentCost);
    }

    if (app.state.victoryInfo.stepCount <= info.bestStepCount)
    {
        info.bestStepCount = app.state.victoryInfo.stepCount;
        app.state.savePuzzle(filenameStepCount);
    }

    save();
}
