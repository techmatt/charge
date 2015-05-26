
struct PuzzleInfo
{
    string name;
};

struct PuzzleManager
{
    void init();

    vector<PuzzleInfo> puzzleList;

    int currentPuzzle;
};