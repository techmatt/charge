
#include "main.h"

void GameState::init()
{
	board.cells.allocate(params().boardDims.x, params().boardDims.y);
}
