#include "main.h"

void UserSession::init(int slotIndex)
{
    string s = "A";
    s[0] += slotIndex;
    folder = "Session" + s + "/";

    campaignLevels.resize(database().puzzles.size());

    for (int i = 0; i < campaignLevels.size(); i++)
    {
        campaignLevels[i].filename = database().puzzles[i].filename;
    }

    playMusic = false;
    playSounds = true;

    load();
}

void UserSession::save()
{
    if (folder.size() == 0)
        return;

    ParameterTable table("Session");

    table.setString("folder", folder);

    for (int levelIndex = 0; levelIndex < campaignLevels.size(); levelIndex++)
    {
        table.setTable("campaignLevel" + util::zeroPad(levelIndex, 3), campaignLevels[levelIndex].toTable("campaignLevel"));
    }

    table.setInt("customLevelCount", (int)customLevels.size());

    int customLevelIndex = 0;
    for (auto &level : customLevels)
    {
        table.setTable("customLevel" + util::zeroPad(customLevelIndex++, 3), level.second.toTable("customLevel"));
    }

    table.setBool("playMusic", playMusic);
    table.setBool("playSounds", playSounds);

    table.save(dataFile());
}

void UserSession::load()
{
    if (folder.size() == 0)
        return;

    if (!util::fileExists(dataFile()))
        return;

    ParameterTable table;
    table.load(dataFile());

    folder = table.getString("folder");
    playMusic = table.getBool("playMusic");
    playSounds = table.getBool("playSounds");

    for (int levelIndex = 0; levelIndex < campaignLevels.size(); levelIndex++)
    {
        string paramName = "campaignLevel" + util::zeroPad(levelIndex, 3);
        if (table.hasParameter(paramName))
            campaignLevels[levelIndex] = UserSessionLevelInfo::fromTable(table.getTable(paramName));
    }

    int customLevelCount = table.getInt("customLevelCount");

    for (int customLevelIndex = 0; customLevelIndex < customLevelCount; customLevelIndex++)
    {
        string paramName = "customLevel" + util::zeroPad(customLevelIndex, 3);
        if (table.hasParameter(paramName))
        {
            auto info = UserSessionLevelInfo::fromTable(table.getTable(paramName));
            customLevels[info.filename] = info;
        }
    }

}

string UserSession::dataFile() const
{
    util::makeDirectory(params().rootDir + folder);
    return params().rootDir + folder + "progress.dat";
}

string UserSession::getSolutionFilename(const string &puzzleName, bool campaign, SolutionType type) const
{
    const string subFolder = campaign ? "campaign/" : "custom/";

    util::makeDirectory(params().rootDir + folder);
    util::makeDirectory(params().rootDir + folder + subFolder);

    string suffix = "";
    if (type == SolutionType::MostRecent) suffix = "_Recent";
    if (type == SolutionType::BestStepCount) suffix = "_BestTime";
    if (type == SolutionType::BestPiecesUsed) suffix = "_FewestComponents";

    return params().rootDir + folder + subFolder + puzzleName + suffix + ".pzl";
}

void UserSession::saveProgress(AppData &app)
{
    int userPieceCount = 0;
    for (Component *c : app.state.components)
        if (c->modifiers.puzzleType != ComponentPuzzleType::PuzzlePiece)
            userPieceCount++;

    if (userPieceCount == 0)
        return;

    const string filenameRecent = app.session.getSolutionFilename(database().puzzles[app.controller.currentPuzzleIndex].filename, true, SolutionType::MostRecent);
    const string filenameStepCount = app.session.getSolutionFilename(database().puzzles[app.controller.currentPuzzleIndex].filename, true, SolutionType::BestStepCount);
    
    if (!util::fileExists(filenameStepCount))
        app.state.savePuzzle(filenameRecent);
}

void UserSession::recordVictoryCampaign(AppData &app)
{
    const string filenameRecent = app.session.getSolutionFilename(database().puzzles[app.controller.currentPuzzleIndex].filename, true, SolutionType::MostRecent);
    const string filenameStepCount = app.session.getSolutionFilename(database().puzzles[app.controller.currentPuzzleIndex].filename, true, SolutionType::BestStepCount);
    const string filenamePiecesUsed = app.session.getSolutionFilename(database().puzzles[app.controller.currentPuzzleIndex].filename, true, SolutionType::BestPiecesUsed);

    auto &info = campaignLevels[app.controller.currentPuzzleIndex];

    info.state = LevelState::Solved;

    app.state.savePuzzle(filenameRecent);

    if (app.state.victoryInfo.piecesUsed <= info.bestPiecesUsed)
    {
        info.bestPiecesUsed = app.state.victoryInfo.piecesUsed;
        app.state.savePuzzle(filenamePiecesUsed);
    }

    if (app.state.victoryInfo.stepCount <= info.bestStepCount)
    {
        info.bestStepCount = app.state.victoryInfo.stepCount;
        app.state.savePuzzle(filenameStepCount);
    }

    save();
}
