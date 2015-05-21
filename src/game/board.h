
struct BoardCell
{
	BoardCell() { c = nullptr; blocked = false; }
    BoardCell(Component &_c) { c = &_c; }

    Component *c;

	//
	// a cell is blocked if a blocker is preventing something from being built here.
	// this includes any area smaller than 2x2 squished between blockers.
	//
	bool blocked;
};

struct Board
{
    void updateBlockedGrid();

    bool isCircuitBoard() const
    {
        return (cells.dimX() == constants::circuitBoardSize);
    }

    bool coordValidForNewComponent(const vec2i &v) const;
    void addNewComponent(Component *component);

    void findAdjacentBuildings(const vec2i &source, Component *result[6], int &resultCount);

    //ParameterTable toTable(const string &tableName) const;

    Grid2<BoardCell> cells;
};
