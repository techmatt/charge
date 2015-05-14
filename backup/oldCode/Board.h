struct BoardCell
{
	BoardCell() {Bldg = NULL;}
	BoardCell(Building *_Bldg) {Bldg = _Bldg;}
	Building *Bldg;
};

class Board
{
public:
	void Init(UINT Width, UINT Height);

	__forceinline Grid<BoardCell>& Cells()
	{
		return _Cells;
	}
	
	void AddBuilding(Building *Bldg);
	void RemoveBuilding(Building *Bldg);
	bool BuildingPlacementValid(const Coordinate &C);
	void FindAdjacentBuildings(const Coordinate &Source, Building *Result[6], UINT &ResultCount);

	void Transpose();
	void Rotate90();
	void RotateN90();
	void FlipHorizontal();
	void FlipVertical();

private:
	void InsertAdjacentBuilding(const Coordinate &Offset, Building *Result[6], UINT &ResultCount);

	Grid<BoardCell> _Cells;
};