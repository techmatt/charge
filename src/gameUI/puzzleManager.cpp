
#include "main.h"

void PuzzleManager::init()
{
    currentPuzzle = 0;

    for (const string &s : util::getFileLines(params().assetDir + "../legacy/legacyLevelList.txt", 3))
    {
        PuzzleInfo puzzle;
        puzzle.name = s;
        puzzleList.push_back(puzzle);
    }
}
