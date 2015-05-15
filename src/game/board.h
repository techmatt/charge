
struct BoardCell
{
    BoardCell() { c = nullptr; }
    BoardCell(Component &_c) { c = &_c; }

    Component *c;
};

struct Board
{
    Grid2<BoardCell> cells;
};
