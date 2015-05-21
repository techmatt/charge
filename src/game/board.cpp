
#include "main.h"

void Board::updateBlockedGrid()
{
    for (auto &cell : cells)
    {
        cell.value.blocked = false;
        if (cell.value.c != nullptr && cell.value.c->info->name == "Blocker")
        {
            cell.value.blocked = true;
        }
    }

    while (true)
    {
        auto cellBlocked = [&](int x, int y)
        {
            if (!cells.coordValid(x, y))
                return true;
            if (cells(x, y).blocked)
                return true;
            return false;
        };
        bool changed = false;
        for (auto &cell : cells)
        {
            if (!cell.value.blocked)
            {
                if (cellBlocked((int)cell.x - 1, (int)cell.y) &&
                    cellBlocked((int)cell.x + 1, (int)cell.y))
                {
                    cell.value.blocked = true;
                    changed = true;
                }
                if (cellBlocked((int)cell.x, (int)cell.y - 1) &&
                    cellBlocked((int)cell.x, (int)cell.y + 1))
                {
                    cell.value.blocked = true;
                    changed = true;
                }
            }
        }
        if (!changed) return;
    }
    
}

bool Board::coordValidForNewComponent(const vec2i &v) const
{
    if (!math::between(v.x, 0, params().boardDims.x - 2) || !math::between(v.x, 0, params().boardDims.x - 2))
        return false;
    for (int xOffset = 0; xOffset <= 1; xOffset++)
        for (int yOffset = 0; yOffset <= 1; yOffset++)
        {
            vec2i coord = v + vec2i(xOffset, yOffset);
            if (!cells.coordValid(coord))
                return false;
            const BoardCell &c = cells(coord);
            if (c.blocked || c.c != nullptr)
                return false;
        }
    return true;
}

void Board::addNewComponent(Component *component)
{
    if (!isCircuitBoard() && component->location.inCircuit())
    {
        cells(component->location.boardPos).c->circuitBoard->addNewComponent(component);
        return;
    }

    const vec2i basePos = isCircuitBoard() ? component->location.circuitPos : component->location.boardPos;
    for (int xOffset = 0; xOffset <= 1; xOffset++)
        for (int yOffset = 0; yOffset <= 1; yOffset++)
        {
            BoardCell &c = cells(basePos.x + xOffset, basePos.y + yOffset);
            MLIB_ASSERT_STR(!c.blocked && c.c == nullptr, "invalid building location");
            c.c = component;
        }
}

void Board::findAdjacentBuildings(const vec2i &source, Component *result[6], int &resultCount)
{
    auto insertAdjacentBuilding = [&](const vec2i &offset)
    {
        if (cells.coordValid(offset) && cells(offset).c != nullptr)
        {
            Component *comp = cells(offset).c;
            for (int componentIndex = 0; componentIndex < resultCount; componentIndex++)
            {
                if (result[componentIndex] == comp)
                    return;
            }
            result[resultCount++] = comp;
        }
    };

    resultCount = 0;

    //
    // Top row
    //
    insertAdjacentBuilding(vec2i(source.x + 1, source.y - 1));

    //
    // Right column
    //
    insertAdjacentBuilding(vec2i(source.x + 2, source.y + 0));
    insertAdjacentBuilding(vec2i(source.x + 2, source.y + 1));

    //
    // Bottom row
    //
    insertAdjacentBuilding(vec2i(source.x + 1, source.y + 2));
    insertAdjacentBuilding(vec2i(source.x + 0, source.y + 2));

    //
    // Left column
    //
    insertAdjacentBuilding(vec2i(source.x - 1, source.y + 0));
    insertAdjacentBuilding(vec2i(source.x - 1, source.y + 1));

    //
    // Top row
    //
    insertAdjacentBuilding(vec2i(source.x + 0, source.y - 1));
}

/*ParameterTable Board::toTable(const string &tableName) const
{
    ParameterTable table(tableName);
    for (auto &cell : cells)
    {
        ParameterTable cellTable("BoardCell");
        cellTable.setInt("x", (int)cell.x);
        cellTable.setInt("y", (int)cell.y);
        if (cell.value.c != nullptr)
        {
            cell.value.c->toTable();
        }
        table.setTable("cell-" + util::zeroPad(cell.x, 2) + "-" + util::zeroPad(cell.y, 2), cellTable);
    }
    return table;
}*/
