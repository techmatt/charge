
//
// this is just the database of puzzles; the actual selection of puzzles is managed by GameController
//
struct PuzzleManager
{
    void init();

private:
    vector<PuzzleInfo> puzzleList;
};
