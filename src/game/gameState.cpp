
#include "main.h"

void GameState::init()
{
	board.cells.allocate(params().boardDims.x, params().boardDims.y);
}

void GameState::addNewComponent(Component *component)
{
    components.push_back(component);
    board.addNewComponent(component);
}