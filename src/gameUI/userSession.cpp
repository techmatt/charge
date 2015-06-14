#include "main.h"

void UserSession::init(int slotIndex)
{
    string s = "A";
    s[0] += slotIndex;
    folder = "Session" + s + "/";

    levels.resize(database().puzzles.size());

    playMusic = false;
    playSounds = true;
}

void UserSession::save()
{
    ParameterTable table("Session");

    /*    string folder;

    vector<UserSessionLevelInfo> levels;

    bool playMusic;
    bool playSounds;*/
    table.setString("folder", folder);

    for (int levelIndex = 0; levelIndex < levels.size(); levelIndex++)
    {
        table.setInt("level" + util::zeroPad(levelIndex, 3), (int)levels[levelIndex].state);
    }

    table.setBool("playMusic", playMusic);
    table.setBool("playSounds", playSounds);

    table.save(params().rootDir + folder + "progress.dat");
}

void UserSession::load()
{

}
