#include "Main.h"

void Board::Init(UINT Width, UINT Height)
{
	_Cells.Allocate(Height, Width);
}

void Board::AddBuilding(Building *Bldg)
{
	const Coordinate &C = Bldg->Location();
	for(UINT XOffset = 0; XOffset <= 1; XOffset++)
	{
		for(UINT YOffset = 0; YOffset <= 1; YOffset++)
		{
			PersistentAssert(_Cells.ValidCoordinates(C.y + YOffset, C.x + XOffset), "Invalid coordinates");
			BoardCell &CurCell = _Cells.GetElement(C.y + YOffset, C.x + XOffset);
			PersistentAssert(CurCell.Bldg == NULL, "Invalid build location");
			CurCell.Bldg = Bldg;
		}
	}
}

void Board::RemoveBuilding(Building *Bldg)
{
	const Coordinate &C = Bldg->Location();
	for(UINT XOffset = 0; XOffset <= 1; XOffset++)
	{
		for(UINT YOffset = 0; YOffset <= 1; YOffset++)
		{
			PersistentAssert(_Cells.ValidCoordinates(C.y + YOffset, C.x + XOffset), "Invalid coordinates");
			BoardCell &CurCell = _Cells.GetElement(C.y + YOffset, C.x + XOffset);
			PersistentAssert(CurCell.Bldg == Bldg, "Destroying invalid building");
			CurCell.Bldg = NULL;
		}
	}
}

bool Board::BuildingPlacementValid(const Coordinate &C)
{
	for(UINT XOffset = 0; XOffset <= 1; XOffset++)
	{
		for(UINT YOffset = 0; YOffset <= 1; YOffset++)
		{
			if(!_Cells.ValidCoordinates(C.y + YOffset, C.x + XOffset))
			{
				return false;
			}
			BoardCell &CurCell = _Cells.GetElement(C.y + YOffset, C.x + XOffset);
			if(CurCell.Bldg != NULL)
			{
				return false;
			}
		}
	}
	return true;
}

void Board::FindAdjacentBuildings(const Coordinate &Source, Building *Result[6], UINT &ResultCount)
{
	ResultCount = 0;

	//
	// Top row
	//
	InsertAdjacentBuilding(Coordinate(Source.x + 0, Source.y - 1), Result, ResultCount);
	InsertAdjacentBuilding(Coordinate(Source.x + 1, Source.y - 1), Result, ResultCount);

	//
	// Right column
	//
	InsertAdjacentBuilding(Coordinate(Source.x + 2, Source.y + 0), Result, ResultCount);
	InsertAdjacentBuilding(Coordinate(Source.x + 2, Source.y + 1), Result, ResultCount);

	//
	// Bottom row
	//
	InsertAdjacentBuilding(Coordinate(Source.x + 0, Source.y + 2), Result, ResultCount);
	InsertAdjacentBuilding(Coordinate(Source.x + 1, Source.y + 2), Result, ResultCount);

	//
	// Left column
	//
	InsertAdjacentBuilding(Coordinate(Source.x - 1, Source.y + 0), Result, ResultCount);
	InsertAdjacentBuilding(Coordinate(Source.x - 1, Source.y + 1), Result, ResultCount);

	
}

void Board::InsertAdjacentBuilding(const Coordinate &Offset, Building *Result[6], UINT &ResultCount)
{
	if(_Cells.ValidCoordinates(Offset.y, Offset.x))
	{
		Building *CurBldg = _Cells.GetElement(Offset.y, Offset.x).Bldg;
		if(CurBldg != NULL)
		{
			for(UINT BldgIndex = 0; BldgIndex < ResultCount; BldgIndex++)
			{
				if(Result[BldgIndex] == CurBldg)
				{
					return;
				}
			}
			Assert(ResultCount < 6, "Too many adjacent buildings");
			Result[ResultCount++] = CurBldg;
		}
	}
}

void Board::Transpose()
{
	Grid<BoardCell> Copy = _Cells;
	//ofstream File("Debug.txt");
	for(UINT Row = 0; Row < Copy.Rows(); Row++)
	{
		for(UINT Col = 0; Col < Copy.Cols(); Col++)
		{	
			BoardCell &NewCell = _Cells.GetElement(Col, Row);
			BoardCell &OldCell = Copy.GetElement(Row, Col);
			NewCell.Bldg = OldCell.Bldg;	
		}
	}

	for(UINT Row = 0; Row < Copy.Rows() - 1; Row++)
	{
		for(UINT Col = 0; Col < Copy.Cols() - 1; Col++)
		{	
			BoardCell &NewCell = _Cells.GetElement(Row, Col);
			BoardCell &NextRowCell = _Cells.GetElement(Row + 1, Col);
			BoardCell &NextColCell = _Cells.GetElement(Row, Col + 1);
			if(NewCell.Bldg != NULL && NewCell.Bldg == NextRowCell.Bldg && NewCell.Bldg == NextColCell.Bldg)
			{
				NewCell.Bldg->Move(Coordinate(Col, Row));
			}
			//Console::File() << _Cells.GetElement(Row, Col).Bldg << '\t';
			//File << Copy.GetElement(Row, Col).Bldg << '\t';
		}
		//Console::File() << endl;
		//File << endl;
	}

	
}

void Board::Rotate90()
{
	Transpose();
	FlipHorizontal();
}

void Board::RotateN90()
{
	Rotate90();
	Rotate90();
	Rotate90();
}

void Board::FlipHorizontal()
{
	Grid<BoardCell> Copy = _Cells;
	for(UINT Row = 0; Row < Copy.Rows(); Row++)
	{
		for(UINT Col = 0; Col < Copy.Cols(); Col++)
		{	
			BoardCell &NewCell = _Cells.GetElement(Row, Col);
			BoardCell &OldCell = Copy.GetElement(Row, Copy.Cols() - 1 - Col);
			NewCell.Bldg = OldCell.Bldg;	
		}
	}

	for(UINT Row = 0; Row < Copy.Rows() - 1; Row++)
	{
		for(UINT Col = 0; Col < Copy.Cols() - 1; Col++)
		{	
			BoardCell &NewCell = _Cells.GetElement(Row, Col);
			BoardCell &NextRowCell = _Cells.GetElement(Row + 1, Col);
			BoardCell &NextColCell = _Cells.GetElement(Row, Col + 1);
			if(NewCell.Bldg != NULL && NewCell.Bldg == NextRowCell.Bldg && NewCell.Bldg == NextColCell.Bldg)
			{
				NewCell.Bldg->Move(Coordinate(Col, Row));
			}
		}
	}
}

void Board::FlipVertical()
{
	Rotate90();
	FlipHorizontal();
	RotateN90();
}
