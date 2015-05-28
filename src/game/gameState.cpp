
#include "main.h"

void GameState::clearBoard()
{
    for (Component *c : components)
        delete c;
    components.clear();

    board.cells.allocate(params().boardDims.x, params().boardDims.y);
    resetPuzzle();
}

void GameState::init()
{
    clearBoard();

    auto &tableA = circuitBoundaryNeighborOffsetTable;
    auto &tableB = circuitBoundaryNeighborOffsetTableAligned;
    tableA.allocate(constants::circuitBoardSize / 2, constants::circuitBoardSize / 2, constants::invalidCoord);
    tableB = tableA;

    tableA(0, 1) = vec2i(-2, -1);
    tableA(0, 3) = vec2i(-2, 0);
    tableA(0, 5) = vec2i(-2, 1);

    tableA(6, 1) = vec2i(2, -1);
    tableA(6, 3) = vec2i(2, 0);
    tableA(6, 5) = vec2i(2, 1);

    tableA(1, 0) = vec2i(-1, -2);
    tableA(3, 0) = vec2i(0, -2);
    tableA(5, 0) = vec2i(1, -2);

    tableA(1, 6) = vec2i(-1, 2);
    tableA(3, 6) = vec2i(0, 2);
    tableA(5, 6) = vec2i(1, 2);

    
    for (int i = 1; i <= 5; i++)
    {
        tableB(0, i) = vec2i(-2, 0);
        tableB(6, i) = vec2i(2, 0);
        tableB(i, 0) = vec2i(0, -2);
        tableB(i, 6) = vec2i(0, 2);
    }
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

void GameState::addNewComponent(Component *component, bool addCircuitComponents, bool updateConnections)
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

    if (updateConnections)
        updateAll();
}

void GameState::updateAll()
{
    board.updateBlockedGrid();
    updateCircuitBoundaries();
    updateComponentConnections();
}

void GameState::removeComponent(Component *component, bool updateConnections)
{
    if (component->circuitBoard != nullptr)
    {
        // we have to be careful here because we are deleting elements from the list we are iterating over.
        while (true)
        {
            bool childFound = false;
            for (Component *child : components)
            {
                if (child->location.inCircuit() && child->location.boardPos == component->location.boardPos)
                {
                    childFound = true;
                    removeComponent(child, false);
                    break;
                }
            }
            if (!childFound)
                break;
        }
    }

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

    if (updateConnections)
        updateAll();
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
    globalRotationOffset += constants::secondsPerStep * constants::chargeRotationsPerSecond * 360.0f;
    globalRotationOffset = fmod(globalRotationOffset, 360.0f);
}

Component& GameState::getCircuit(const GameLocation &pos)
{
    Component *c = getComponent(GameLocation(pos.boardPos));
    MLIB_ASSERT_STR(c != nullptr && c->circuitBoard != nullptr, "circuit not found");
    return *c;
}

Component* GameState::getComponent(const GameLocation &pos, bool skipInactiveBoundary)
{
    if (!pos.valid()) return nullptr;

	if (0 > pos.boardPos.x || pos.boardPos.x >= params().boardDims.x)
		return nullptr;
	if (0 > pos.boardPos.y || pos.boardPos.y >= params().boardDims.y)
		return nullptr;

    Component *component = board.cells(pos.boardPos).c;

    if (pos.inCircuit())
    {
		if (0 > pos.circuitPos.x || pos.circuitPos.x>=constants::circuitBoardSize)
			return nullptr;
		if (0 > pos.circuitPos.y || pos.circuitPos.y>=constants::circuitBoardSize)
			return nullptr;


        if (component == nullptr || component->circuitBoard == nullptr)
        {
            return nullptr;
        }
        Component *result = component->circuitBoard->cells(pos.circuitPos).c;
        if (skipInactiveBoundary && result != nullptr && result->inactiveBoundary())
            return nullptr;
        return result;
    }
    else
    {
        return component;
    }
}

Component* GameState::findCircuitBoundaryNeighbor(Component &component)
{
    Component *alignedResult = findCircuitBoundaryNeighborAligned(component);
    if (alignedResult != nullptr)
        return alignedResult;

    const auto &table = circuitBoundaryNeighborOffsetTable;
    const vec2i worldOffset = table(component.location.circuitPos / 2);
    const vec2i worldCoord = worldOffset + component.location.boardPos;

    if (worldOffset == constants::invalidCoord ||
        !board.cells.coordValid(worldCoord))
        return nullptr;

    Component *result = board.cells(worldCoord).c;

    if (result != nullptr && result->location.boardPos == worldCoord)
        // TODO: for circuits, this needs to introspect into the circuit
        return result;
    else
        return nullptr;
}

Component* GameState::findCircuitBoundaryNeighborAligned(Component &component)
{
    const auto &table = circuitBoundaryNeighborOffsetTableAligned;
    const vec2i worldOffset = table(component.location.circuitPos / 2);
    const vec2i worldCoord = worldOffset + component.location.boardPos;

    if (worldOffset == constants::invalidCoord ||!board.cells.coordValid(worldCoord))
        return nullptr;

    Component *result = board.cells(worldCoord).c;

    if (result != nullptr && result->location.boardPos == worldCoord && result->info->name == "Circuit")
        // TODO: this returns the circuit, but not the mapping into the circuit
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

int GameState::findNeighboringComponents(Component &component, Component *neighbors[6])
{
    if (component.location.inCircuit())
    {
        if (component.info->name == "CircuitBoundary")
        {
            // TODO: the circuit boundary connects to either another component in the circuit, a component on the mian board, or a component in another circuit.
            return 0;
        }
        else
        {
            return getCircuit(component.location).circuitBoard->findNeighboringComponents(component.location.circuitPos, neighbors);
        }
    }
    else
    {
        return board.findNeighboringComponents(component.location.boardPos, neighbors);
    }
}












// This should be run after any change to the board and possibly right before playing the simulation.
void GameState::updateComponentConnections()
{
	for (const auto &component : components)
	{
		// clear the last attempt to find connections
		if (component->circuitBoard != nullptr) continue;
		for (int i = 0; i < 12; i++)
			component->connections[i] = connectableComponentAtRelativePosition(component, constants::nearbyComponents[i]);
	}
}

Component* GameState::connectableComponentAtRelativePosition(Component* component, vec2i relativePosition)
{
	GameLocation pos = GameLocation(component->location);
	bool cInCircuit = false;

	if (component->location.inCircuit())
	{
		// check if the displacement would send it off the bounds of the circuit.  If it would, displace the circuit instead
		pos.circuitPos += relativePosition;

		if (0 > pos.circuitPos.x || pos.circuitPos.x >= constants::circuitBoardSize || 0 > pos.circuitPos.y || pos.circuitPos.y >= constants::circuitBoardSize)
		{
			pos.circuitPos = constants::invalidCoord;
			pos.boardPos += relativePosition;
			cInCircuit = true;
		}
	}
	else
		pos.boardPos += relativePosition;

	// find the component we think might be there
	Component *target = getComponent(pos);
	if (target == nullptr) return nullptr;

	// verify that it is exactly there.
	if (target->location != pos) return nullptr;

	// circuit edges cannot connect to each other
	if (component->info->name =="CircuitBoundary"&& target->info->name == "CircuitBoundary")
		return nullptr;



	bool tInCircuit = (target->circuitBoard != nullptr); //is the target going to be a component in some other circuit?
	if (!cInCircuit && !tInCircuit)
		return target;
	// now deal with connection where one of the parts is in a circuit
	if (cInCircuit && !tInCircuit)
	{
		// we check whether the connection actually exists.  This isn't the prettiest way of doing this, but it works
		if (component->location.circuitPos == 2*BoardToCircuitTargetLocation(-1 * relativePosition))
			return target;
		else
			return nullptr;
	}
	if (!cInCircuit && tInCircuit)
	{
		// the target is in the position indicated by BoardToCircuitLocation
		pos.circuitPos = 2*BoardToCircuitTargetLocation(relativePosition);
		if (pos.circuitPos == constants::invalidCoord) return nullptr;//this shouldn't happen
		return getComponent(pos);
	}
	if (cInCircuit && tInCircuit) {
		pos.circuitPos = 2*CircuitToCircuitTargetLocation(relativePosition,component->location.circuitPos/2);
		if (pos.circuitPos == constants::invalidCoord) return nullptr;//this one might happen
		return getComponent(pos);
	}

	return nullptr; // THIS SHOULD NEVER BE REACHED
}

// This is really ugly, but there are only a couple tens of things to check, for both of these so it works.
// Screw pretty, generalizable code.
vec2i GameState::BoardToCircuitTargetLocation(vec2i displacement) {
	// the four cardinal directions
	if (displacement == vec2i(2, 0)) return vec2i(0, 3);
	if (displacement == vec2i(-2, 0)) return vec2i(6, 3);
	if (displacement == vec2i(0,2)) return vec2i(3,0);
	if (displacement == vec2i(0,-2)) return vec2i(3, 6);

	// the 8 offset directions
	if (displacement == vec2i(2, 1)) return vec2i(0,1);
	if (displacement == vec2i(2, -1)) return  vec2i(0,5);
	if (displacement == vec2i(-2, 1)) return vec2i(6, 1);
	if (displacement == vec2i(-2, -1)) return vec2i(6,5);
	if (displacement == vec2i(1,2)) return vec2i(1,0);
	if (displacement == vec2i(-1,2)) return vec2i(5, 0);
	if (displacement == vec2i(1,-2)) return vec2i(1, 6);
	if (displacement == vec2i(-1,-2)) return vec2i(5, 6);

	return constants::invalidCoord;
}

vec2i GameState::CircuitToCircuitTargetLocation(vec2i displacement, vec2i circuitPosition) {
	// the four cardinal directions
	if (displacement == vec2i(-2, 0) && circuitPosition == vec2i(0, 1)) return vec2i(6,1);
	if (displacement == vec2i(-2, 0) && circuitPosition == vec2i(0, 2)) return vec2i(6,2);
	if (displacement == vec2i(-2, 0) && circuitPosition == vec2i(0, 3)) return vec2i(6,3);
	if (displacement == vec2i(-2, 0) && circuitPosition == vec2i(0, 4)) return vec2i(6,4);
	if (displacement == vec2i(-2, 0) && circuitPosition == vec2i(0, 5)) return vec2i(6,5);

	if (displacement == vec2i(2, 0) && circuitPosition == vec2i(6, 1)) return vec2i(0, 1);
	if (displacement == vec2i(2, 0) && circuitPosition == vec2i(6, 2)) return vec2i(0, 2);
	if (displacement == vec2i(2, 0) && circuitPosition == vec2i(6, 3)) return vec2i(0, 3);
	if (displacement == vec2i(2, 0) && circuitPosition == vec2i(6, 4)) return vec2i(0, 4);
	if (displacement == vec2i(2, 0) && circuitPosition == vec2i(6, 5)) return vec2i(0, 5);

	if (displacement == vec2i(0,-2) && circuitPosition == vec2i(1,0)) return vec2i(6, 1);
	if (displacement == vec2i(0,-2) && circuitPosition == vec2i(2,0)) return vec2i(6, 2);
	if (displacement == vec2i(0,-2) && circuitPosition == vec2i(3,0)) return vec2i(6, 3);
	if (displacement == vec2i(0,-2) && circuitPosition == vec2i(4,0)) return vec2i(6, 4);
	if (displacement == vec2i(0,-2) && circuitPosition == vec2i(5,0)) return vec2i(6, 5);

	if (displacement == vec2i(0,2) && circuitPosition == vec2i(1,6)) return vec2i(1,0);
	if (displacement == vec2i(0, 2) && circuitPosition == vec2i(2, 6)) return vec2i(2, 0);
	if (displacement == vec2i(0, 2) && circuitPosition == vec2i(3, 6)) return vec2i(3, 0);
	if (displacement == vec2i(0, 2) && circuitPosition == vec2i(4, 6)) return vec2i(4, 0);
	if (displacement == vec2i(0, 2) && circuitPosition == vec2i(5, 6)) return vec2i(5, 0);


	// the 8 offset directions
	if (displacement == vec2i(-2, -1) && circuitPosition == vec2i(0, 1)) return vec2i(6,5);
	if (displacement == vec2i(-2, 1) && circuitPosition == vec2i(0, 5)) return vec2i(6,1);
	if (displacement == vec2i(2, -1) && circuitPosition == vec2i(6, 1)) return vec2i(0, 5);
	if (displacement == vec2i(2, 1) && circuitPosition == vec2i(6, 5)) return vec2i(0, 1);
	if (displacement == vec2i(-1, -2) && circuitPosition == vec2i(1, 0)) return vec2i(5,6);
	if (displacement == vec2i(1, -2) && circuitPosition == vec2i(5, 0)) return vec2i(1,6);
	if (displacement == vec2i(-1, 2) && circuitPosition == vec2i(1, 6)) return vec2i(5,0);
	if (displacement == vec2i(1, 2) && circuitPosition == vec2i(5, 6)) return vec2i(1,0);

	return constants::invalidCoord;
}
