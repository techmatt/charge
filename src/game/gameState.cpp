
#include "main.h"

void GameState::clearBoard()
{
    for (Component *c : components)
        delete c;

    board.cells.allocate(params().boardDims.x, params().boardDims.y);
    resetPuzzle();
}

void GameState::init()
{
    clearBoard();

    circuitBoundaryNeighborOffsetTable.allocate(constants::circuitBoardSize / 2, constants::circuitBoardSize / 2, constants::invalidCoord);

    auto &table = circuitBoundaryNeighborOffsetTable;
    table(6, 1) = vec2i(2, -1);
    table(6, 3) = vec2i(2, 0);
    table(6, 5) = vec2i(2, 1);
}

void GameState::savePuzzle(const string &filename)
{
    ParameterTable puzzleTable("Puzzle");
    
    //puzzleTable.setTable("board", board.toTable("Main"));
    for (int componentIndex = 0; componentIndex < components.size(); componentIndex++)
    {
        const string componentName = "component" + util::zeroPad(componentIndex, 4);
        const ParameterTable componentTable = components[componentIndex]->toTable(componentName);
        puzzleTable.setTable(componentName, componentTable);
    }

    vector<string> lines;
    puzzleTable.appendLines(lines);
    util::saveLinesToFile(lines, filename);
}

void GameState::loadPuzzle(const string &filename)
{
    clearBoard();
}

void GameState::resetPuzzle()
{
    stepCount = 0;
    globalRotationOffset = 0.0f;
    charges.clear();
    explodingCharges.clear();
    for (Component *c : components)
        c->resetPuzzle();
}

void GameState::addNewComponent(Component *component, bool addCircuitComponents)
{
    components.push_back(component);

    board.addNewComponent(component);

    if (component->info->name == "Circuit")
    {
        component->circuitBoard = new Board();
		//component->circuitBoardFrame = new CoordinateFrame(component->location.boardPos, component->location.boardPos + 1, params().circuitDims);
		component->circuitBoard->cells.allocate(constants::circuitBoardSize, constants::circuitBoardSize);

        if (addCircuitComponents)
        {
            const int circuitEdge = constants::circuitBoardSize - 2;
            for (int i = 2; i <= constants::circuitBoardSize - 3; i += 2)
            {
                Component *boundaryA = new Component("CircuitBoundary", ChargeNone, GameLocation(component->location.boardPos, vec2i(i, 0)));
                addNewComponent(boundaryA);

                Component *boundaryB = new Component("CircuitBoundary", ChargeNone, GameLocation(component->location.boardPos, vec2i(i, circuitEdge)));
                addNewComponent(boundaryB);

                Component *boundaryC = new Component("CircuitBoundary", ChargeNone, GameLocation(component->location.boardPos, vec2i(0, i)));
                addNewComponent(boundaryC);

                Component *boundaryD = new Component("CircuitBoundary", ChargeNone, GameLocation(component->location.boardPos, vec2i(circuitEdge, i)));
                addNewComponent(boundaryD);
            }
        }
    }

    board.updateBlockedGrid();
    updateCircuitBoundaries();
}

void GameState::removeComponent(Component *component)
{
    //
    // TODO: make sure circuit's components are deleted correctly.
    //
    for (auto &cell : board.cells)
    {
        if (cell.value.c != nullptr && cell.value.c->circuitBoard != nullptr)
        {
            for (auto &cellInner : cell.value.c->circuitBoard->cells)
                if (cellInner.value.c == component) cellInner.value.c = nullptr;
        }
        if (cell.value.c == component) cell.value.c = nullptr;
    }

    util::removeSwap(components, util::findFirstIndex(components, component));

    delete component;

    board.updateBlockedGrid();
    updateCircuitBoundaries();
}

void GameState::step()
{
    //
    // Advance exploding charges
    //

    for (int chargeIndex = 0; chargeIndex < int(explodingCharges.size()); chargeIndex++)
    {
        ExplodingCharge &charge = explodingCharges[chargeIndex];
        charge.advance();
        if (charge.done())
        {
            util::removeSwap(explodingCharges, chargeIndex);
            chargeIndex--;
        }
    }

    //
    // Move and update charges
    //

    for (Charge &c : charges)
        c.advance(*this);

    for (Charge &c : charges)
        c.interactWithDestination(*this);

    for (Charge &c : charges)
        c.updateDestination(*this);

    //
    // Remove dead charges
    //
    for (int chargeIndex = 0; chargeIndex < int(charges.size()); chargeIndex++)
    {
        Charge &charge = charges[chargeIndex];
        
        if (charge.markedForDeletion && charge.showDeathAnimation)
        {
            explodingCharges.push_back(ExplodingCharge(charge.source, charge.destination, (float)charge.timeInTransit / (float)charge.totalTransitTime, charge.level, constants::explodingChargeDuration, charge.randomRotationOffset + globalRotationOffset, stepCount));
        }

        if (charge.markedForDeletion)
        {
            util::removeSwap(charges, chargeIndex);
            chargeIndex--;
        }
    }

    //
    // Update buildings
    //
    for (const auto &component : components)
    {
        component->tick();

        while(component->chargesToEmit.size() > 0)
        {
            component->lastChargeVisit = stepCount;
            charges.push_back(Charge(component->location, component->chargesToEmit.back()));
            component->chargesToEmit.pop_back();
        }
    }

    stepCount++;
    globalRotationOffset += 1.0f / constants::stepsPerSecond * constants::chargeRotationsPerSecond * 360.0f;
    globalRotationOffset = fmod(globalRotationOffset, 360.0f);
}

Component& GameState::getCircuit(const GameLocation &pos)
{
    Component *c = getComponent(GameLocation(pos.boardPos));
    MLIB_ASSERT_STR(c != nullptr && c->circuitBoard != nullptr, "circuit not found");
    return *c;
}

Component* GameState::getComponent(const GameLocation &pos)
{
    if (!pos.valid()) return nullptr;

    Component *component = board.cells(pos.boardPos).c;

    if (pos.inCircuit())
    {
        MLIB_ASSERT_STR(component != nullptr && component->circuitBoard != nullptr, "no circit at location");
        return component->circuitBoard->cells(pos.circuitPos).c;
    }
    else
    {
        return component;
    }
}

Component* GameState::findCircuitBoundaryNeighbor(Component &component)
{
    const auto &table = circuitBoundaryNeighborOffsetTable;
    const vec2i worldOffset = table(component.location.circuitPos / 2);
    const vec2i worldCoord = worldOffset + component.location.boardPos;

    if (worldOffset == constants::invalidCoord ||
        !board.cells.coordValid(worldCoord))
        return nullptr;

    Component *result = board.cells(worldCoord).c;

    if (result != nullptr && result->location.boardPos == worldCoord)
        return result;
    else
        return nullptr;
}

void GameState::updateCircuitBoundaries()
{
    for (Component *c : components)
    {
        if (c->info->name == "CircuitBoundary")
            c->circuitBoundaryNeighbor = findCircuitBoundaryNeighbor(*c);
    }
}