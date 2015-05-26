
struct PuzzleInfo
{
    string name;
};

struct PuzzleManager
{
    void init();


    vector<PuzzleInfo> puzzles;

    int currentPuzzle;
};