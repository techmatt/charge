
#include "main.h"

bool Board::coordValidForNewComponent(const vec2i &v) const
{
    if (!math::between(v.x, 0, params().boardDims.x - 2) || !math::between(v.x, 0, params().boardDims.x - 2))
        return false;
    for (int xOffset = 0; xOffset <= 1; xOffset++)
        for (int yOffset = 0; yOffset <= 1; yOffset++)
        {
            const BoardCell &c = cells(v.x + xOffset, v.y + yOffset);
            if (c.blocked || c.c != nullptr)
                return false;
        }
    return true;
}

void Board::addNewComponent(Component *component)
{
    for (int xOffset = 0; xOffset <= 1; xOffset++)
        for (int yOffset = 0; yOffset <= 1; yOffset++)
        {
            BoardCell &c = cells(component->location.boardPos.x + xOffset, component->location.boardPos.y + yOffset);
            MLIB_ASSERT_STR(!c.blocked && c.c == nullptr, "invalid building location");
            c.c = component;
        }
}
