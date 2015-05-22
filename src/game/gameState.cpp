
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
    charges.clear();
    explodingCharges.clear();
    for (Component *c : components)
        c->resetPuzzle();
}

void GameState::addNewComponent(Component *component)
{
    components.push_back(component);

    board.addNewComponent(component);
    board.updateBlockedGrid();

    if (component->info->name == "Circuit")
    {
        component->circuitBoard = new Board();
        component->circuitBoard->cells.allocate(constants::circuitBoardSize, constants::circuitBoardSize);

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

void GameState::removeComponent(Component *component)
{
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
            explodingCharges.push_back(ExplodingCharge(charge.source, charge.destination, (float)charge.timeInTransit / (float)charge.totalTransitTime, charge.level, constants::explodingChargeDuration, charge.rotationOffset(stepCount), stepCount));
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
}

Component* GameState::getComponent(const GameLocation &pos)
{
    if (!pos.valid()) return nullptr;

    Component *component = board.cells(pos.boardPos).c;

    if (pos.inCircuit())
    {
        MLIB_ASSERT_STR(component != nullptr && component->circuitBoard != nullptr, "No circit at location");
        return component->circuitBoard->cells(pos.circuitPos).c;
    }
    else
    {
        return component;
    }
}
