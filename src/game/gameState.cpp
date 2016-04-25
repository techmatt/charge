
#include "main.h"

int GameState::componentCost() const
{
    int cost = 0;

    for (Component *c : components)
    {
        if (c->modifiers.puzzleType == ComponentPuzzleType::PuzzlePiece || c->info->name == "CircuitBoundary")
            continue;

        if (c->modifiers.puzzleType == ComponentPuzzleType::CopiedCircuit)
            cost++;
        else if (c->isCircuit())
            cost += 10;
        else if (c->info->name == "Wire" && c->modifiers.speed == WireType::Standard)
        {
            cost++;
        }
        else
        {
            cost += 2;
        }
    }

    return cost;
}

void GameState::clearComponents()
{
	app.ui.selection.empty();
    for (Component *c : components)
        delete c;
    components.clear();
    board.cells.allocate(params().boardDims.x, params().boardDims.y);
    resetPuzzle();
}

void GameState::clearAndResetBoard()
{
    clearComponents();

    puzzleName = "untitled";

    victoryInfo = GameVictoryInfo();

    buildableComponents.loadDefaults();
}

void GameState::init()
{
    clearAndResetBoard();

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
    for (Component *c : components)
        c->info = c->baseInfo;

    ParameterTable puzzleTable("Puzzle");
    
    puzzleTable.setInt("componentCount", (int)components.size());

    for (int componentIndex = 0; componentIndex < components.size(); componentIndex++)
    {
        const string componentName = "component" + util::zeroPad(componentIndex, 4);
        const ParameterTable componentTable = components[componentIndex]->toTable(componentName);
        puzzleTable.setTable(componentName, componentTable);
    }

    puzzleTable.setTable("buildableComponents", buildableComponents.toTable());

    puzzleTable.setInt("componentCost", victoryInfo.componentCost);
    puzzleTable.setInt("stepCount", victoryInfo.stepCount);

    puzzleTable.setString("puzzleFileType", puzzleFileType);
	puzzleTable.setString("levelPack", levelPack);
	puzzleTable.setString("levelPackPuzzleName", levelPackPuzzleName);
	puzzleTable.setInt("levelPackPuzzleIndex", levelPackPuzzleIndex);

    puzzleTable.save(filename);

    
}

void GameState::allowEditing()
{
	for (auto &c : components)
		c->modifiers.puzzleType = ComponentPuzzleType::PuzzlePiece;
}

void GameState::loadPuzzle(const string &filename, const string &_puzzleName)
{
    clearAndResetBoard();

    //name = util::removeExtensions(util::fileNameFromPath(filename));
    puzzleName = _puzzleName;

    ParameterTable puzzleTable;
    puzzleTable.load(filename);

    buildableComponents = ComponentBuildState(puzzleTable.getTable("buildableComponents"));

    if (puzzleTable.hasParameter("componentCost"))
    {
        victoryInfo.componentCost = puzzleTable.getInt("componentCost");
        victoryInfo.stepCount = puzzleTable.getInt("stepCount");
    }

    const int componentCount = puzzleTable.getInt("componentCount");

    vector<Component*> newComponents(componentCount);

    //
    // load all components
    //
    for (int componentIndex = 0; componentIndex < componentCount; componentIndex++)
    {
        const string componentName = "component" + util::zeroPad(componentIndex, 4);
        ParameterTable componentTable = puzzleTable.getTable(componentName);
        Component *newComponent = Component::fromTable(componentTable);
        newComponents[componentIndex] = newComponent;
    }

    //
    // add all circuits
    //
    for (Component* c : newComponents)
        if (c->info->name == "Circuit")
            addNewComponent(c, false, false);

    //
    // add all non-circuits
    //
    for (Component* c : newComponents)
        if (c->info->name != "Circuit")
            addNewComponent(c, false, false);

	puzzleFileType = "Unknown";
    if (puzzleTable.hasParameter("puzzleFileType"))
		puzzleFileType = puzzleTable.getString("puzzleFileType");

	levelPack = "Unknown";
	if (puzzleTable.hasParameter("levelPack"))
		levelPack = puzzleTable.getString("levelPack");

	levelPackPuzzleName = "Unknown";
	if (puzzleTable.hasParameter("levelPackPuzzleName"))
		levelPackPuzzleName = puzzleTable.getString("levelPackPuzzleName");

	levelPackPuzzleIndex = -1;
	if (puzzleTable.hasParameter("levelPackPuzzleIndex"))
		levelPackPuzzleIndex = puzzleTable.getInt("levelPackPuzzleIndex");

    //
    // normally this is done in addNewComponent, but this is noticably slower so is avoided until the puzzle is fully loaded.
    //
    updateAll();
    resetPuzzle();
}

void GameState::resetPuzzle()
{
    victory = false;
    stepCount = 0;
    globalRotationOffset = 0.0f;
    victoryChargeScaleFactorPositive = 1.0f;
    victoryChargeScaleFactorNegative = 1.0f;
	
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
		component->circuitBoard->cells.allocate(constants::circuitBoardSize, constants::circuitBoardSize);

        if (addCircuitComponents)
        {
            const int circuitEdge = constants::circuitBoardSize - 2;
            for (int i = 2; i <= constants::circuitBoardSize - 3; i += 2)
            {
                Component *boundaryA = new Component("CircuitBoundary", ChargeType::None, GameLocation(component->location.boardPos, vec2i(i, 0)));
                addNewComponent(boundaryA);

                Component *boundaryB = new Component("CircuitBoundary", ChargeType::None, GameLocation(component->location.boardPos, vec2i(i, circuitEdge)));
                addNewComponent(boundaryB);

                Component *boundaryC = new Component("CircuitBoundary", ChargeType::None, GameLocation(component->location.boardPos, vec2i(0, i)));
                addNewComponent(boundaryC);

                Component *boundaryD = new Component("CircuitBoundary", ChargeType::None, GameLocation(component->location.boardPos, vec2i(circuitEdge, i)));
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

    for (const auto &cell : board.cells)
    {
        if (cell.value.c != nullptr && cell.value.c->circuitBoard != nullptr)
        {
            for (const auto &cellInner : cell.value.c->circuitBoard->cells)
                if (cellInner.value.c == component) cellInner.value.c = nullptr;
        }
        if (cell.value.c == component) cell.value.c = nullptr;
    }

    util::removeSwap(components, util::findFirstIndex(components, component));

    delete component;

    if (updateConnections)
        updateAll();
}

void GameState::step(AppData &app)
{
    bool playGoalHoldSound = false;

    for (Component * c : components)
    {
        c->tickGraphics();
    }

    globalRotationOffset += constants::secondsPerStep * constants::chargeRotationsPerSecond * 360.0f;
    globalRotationOffset = fmod(globalRotationOffset, 360.0f);

    if (victory)
    {
        victoryChargeScaleFactorPositive = min(victoryChargeScaleFactorPositive * 1.01f, 1.5f);
        victoryChargeScaleFactorNegative *= 0.995f;

        explodingCharges.clear();

        return;
    }

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

	// reset components
	for (const auto &component : components)
	{
		component->willTrigger = false;
		component->tick(app);
		component->numChargesTargetingThisTick = 0;
        component->prevHeldCharge = component->heldCharge;
        component->heldCharge = ChargeType::None;
		component->sourceOfLastChargeToAttemptToMoveHere.boardPos = constants::invalidCoord;

        for (Connection &connection : component->connections)
        {
            connection.blocked = false;
            connection.desired = false;
        }
	}

	for (Charge &c : charges)
	{
		c.resolvedThisTick = false;
		c.advance(*this);
		Component* destination = getComponent(c.destination);
		Component* source = getComponent(c.source);

		if (c.held) {
			destination->lastChargeVisit = stepCount - constants::chargeRequiredTimeDifference + 1;
            destination->heldCharge = c.level;

            if (c.level == destination->modifiers.color && destination->prevHeldCharge == ChargeType::None)
                playGoalHoldSound = true;
		} 
		c.held = false;
	}
	
    if (constants::useScanlinePriority)
    {
        //
        // make a list of all charges, in priority order
        //

        if (chargePriorityStorage.size() != charges.size())
            chargePriorityStorage.resize(charges.size());
        for (int chargeIndex = 0; chargeIndex < charges.size(); chargeIndex++)
        {
            Charge &c = charges[chargeIndex];
            chargePriorityStorage[chargeIndex] = make_pair(&c, c.scanlinePriority( getComponent(c.destination) ));
        }
        sort(chargePriorityStorage.begin(), chargePriorityStorage.end(), [](const pair<Charge*, int> &a, const pair<Charge*, int> &b) { return a.second < b.second; });

        // We are going to try multiple times to move all the charges.  This should allow us to deal sanely with charges that try to move to the same location
        bool keepAttempting = true;
        while (keepAttempting)
        {
            keepAttempting = false;

            //figure out what the best destination is for all the remaining charges
            for (Charge &c : charges)
            {
                if (c.resolvedThisTick) continue;
                bool hasDestination = c.findBestDestination(*this);
                if (!hasDestination)  // charge has nowhere to go.  Kill it or hold it.
                {
                    if (getComponent(c.destination)->info->holdsCharge) {
                        c.held = true;
                    }
                    else
                    {
                        getComponent(c.destination)->deathTrapTimeLeft = constants::deathTrapDuration;
                        c.markedForDeletion = true;
                    }
                    c.resolvedThisTick = true;
                }
                else // there's a destination. mark it.
                {
                    c.intendedDestination->numChargesTargetingThisTick++;
                }
            }

            // check to see whether there are intermediate-wire collisions
            for (Charge &c : charges)
            {
                if (c.resolvedThisTick) continue;
                Component* destination = getComponent(c.destination);

                // if the movement attempt dies because the charge connection, it doesn't count as hitting the target
                auto &connection = destination->connections[c.intendedConnectionIndex];
                if (connection.desired)
                {
                    c.intendedDestination->numChargesTargetingThisTick--;
                    c.notMovingBecauseOfDesiredConnection = true;
                    connection.blocked = true;
                    keepAttempting = true;
                }
                else
                    c.notMovingBecauseOfDesiredConnection = false;
            }

            // attempt to move the charges there.  If theres somewhere for them to go
            for (const auto &cPriority : chargePriorityStorage)
            {
                Charge &c = *cPriority.first;
                if (c.resolvedThisTick) continue;
                if (c.notMovingBecauseOfDesiredConnection) continue;

                if (c.intendedDestination->numChargesTargetingThisTick >= 1 && (c.intendedDestination->lastChargeVisit != stepCount || c.intendedDestination->info->name == "MegaHold"))
                {
                    c.setNewDestination(*this, *c.intendedDestination);
                    c.resolvedThisTick = true;
                }
                else
                {
                    keepAttempting = true;
                    //c.intendedDestination->lastChargeVisit = stepCount - constants::chargeRequiredTimeDifference + 1;
                }
            }
        }
    }
    else
    {
        // We are going to try multiple times to move all the charges.  This should allow us to deal sanely with charges that try to move to the same location
        bool keepAttempting = true;
        while (keepAttempting)
        {
            keepAttempting = false;

            //figure out what the best destination is for all the remaining charges
            for (Charge &c : charges)
            {
                if (c.resolvedThisTick) continue;
                bool hasDestination = c.findBestDestination(*this);
                if (!hasDestination)  // charge has nowhere to go.  Kill it or hold it.
                {
                    if (getComponent(c.destination)->info->holdsCharge) {
                        c.held = true;
                    }
                    else
                    {
                        c.markedForDeletion = true;
                    }
                    c.resolvedThisTick = true;
                }
                else // there's a destination. mark it.
                {
                    c.intendedDestination->numChargesTargetingThisTick++;
                }
            }

            // check to see whether there are intermediate-wire collisions
            for (Charge &c : charges)
            {
                if (c.resolvedThisTick) continue;
                Component* destination = getComponent(c.destination);

                // if the movement attempt dies because the charge connection, it doesn't count as hitting the target
                auto &connection = destination->connections[c.intendedConnectionIndex];
                if (connection.desired)
                {
                    c.intendedDestination->numChargesTargetingThisTick--;
                    c.notMovingBecauseOfDesiredConnection = true;
                    connection.blocked = true;
                    keepAttempting = true;
                }
                else
                    c.notMovingBecauseOfDesiredConnection = false;
            }

            // attempt to move the charges there.  If theres somewhere for them to go
            for (Charge &c : charges)
            {
                if (c.resolvedThisTick) continue;
                if (c.notMovingBecauseOfDesiredConnection) continue;

                if (c.intendedDestination->numChargesTargetingThisTick == 1)
                {
                    c.setNewDestination(*this, *c.intendedDestination);
                    c.resolvedThisTick = true;
                }
                else
                {
                    keepAttempting = true;
                    c.intendedDestination->lastChargeVisit = stepCount - constants::chargeRequiredTimeDifference + 1;
                }
            }
        }
    }

	// check all the held charges to see whether they should forget their sources
	for (Charge &c : charges)
	{
		if (!c.held) continue;
		
		// this is a slightly more aggressive forgeting strategy: having charges pass by will trigger release
		//if (!(getComponent(c.source)->willAcceptCharge(*this,c)))
		//	c.source = c.destination; //if we can't go back, forget the original source

		// this is a less aggressive strategy: only trigger release if a blocker/held charge was removed.
		Component* source = getComponent(c.source);
		if (source->info->name == "TrapSprung" || source->info->name == "GateClosed")
			c.source = c.destination;
		if (source->heldCharge != ChargeType::None)
			c.source = c.destination;
	}

	// now deal with charges that interact with their location
    for (Charge &c : charges)
        c.interactWithDestination(*this, app);

    //
    // Remove dead charges
    //
	set<GameLocation> explodingChargesLocations = {};
    for (int chargeIndex = 0; chargeIndex < int(charges.size()); chargeIndex++)
    {
        Charge &charge = charges[chargeIndex];
        
        Component *destination = getComponent(charge.destination);
        if (destination->deathTrapTimeLeft > 0)
            charge.markedForDeletion = true;

        if (charge.markedForDeletion && charge.showDeathAnimation)
        {
           //add this charge to the list of where charges explode, then delete the charge
			explodingChargesLocations.insert(charge.destination);

			app.playEffect("ChargeDeath", charge.destination);
            explodingCharges.push_back(ExplodingCharge(charge.source, charge.destination, charge.interpolation(), charge.level, constants::explodingChargeDuration, charge.randomRotationOffset + globalRotationOffset, stepCount));
        }

        if (charge.markedForDeletion)
        {
            util::removeSwap(charges, chargeIndex);
            chargeIndex--;
        }
    }
	
	//
	// Remove charges that chain off the first set of charge deaths
	//
	for (int chargeIndex = 0; chargeIndex < int(charges.size()); chargeIndex++)
	{
		Charge &charge = charges[chargeIndex];

		if (explodingChargesLocations.find(charge.destination) != explodingChargesLocations.end())
		{
			// this charge is targeting a place where a charge exploded.  Remove it.
			app.playEffect("ChargeDeath", charge.destination);
			explodingCharges.push_back(ExplodingCharge(charge.source, charge.destination, charge.interpolation(), charge.level, constants::explodingChargeDuration, charge.randomRotationOffset + globalRotationOffset, stepCount));
		
			util::removeSwap(charges, chargeIndex);
			chargeIndex--;
		}
	}


    //
    // Emit charges from components
    //
    for (const auto &component : components)
    {
        while(component->chargesToEmit.size() > 0)
        {
            component->lastChargeVisit = stepCount;
            auto toEmit = component->chargesToEmit.back();
            charges.push_back(Charge(component->location, toEmit.first));
            charges.back().source = toEmit.second;
            component->chargesToEmit.pop_back();
        }
    }

    //
    // check victory conditions
    //
    victory = true;
    bool goalFound = false; // useful so people can still experiment with levels even with no goals
    for (const auto &component : components)
    {
        if (component->info->name == "ChargeGoal")
        {
            goalFound = true;
            if(component->heldCharge != component->modifiers.color)
                victory = false;
        }
        if (component->info->name == "MegaHold")
        {
            goalFound = true;
            if ((!component->inactiveCircuitMegaHold(*this) || component->isSuperMegaHold(*this)) &&
                component->megaHoldTotalCharge < component->intrinsics.totalChargeRequired)
                victory = false;
        }
    }

    if (!goalFound)
        victory = false;

    if (victory)
    {
        victoryInfo.componentCost = componentCost();
        victoryInfo.stepCount = stepCount;
        app.audio.playEffect("Victory");
        app.controller.recordVictory();
    }
    else
    {
        if(playGoalHoldSound) app.playEffect("GoalHold", GameLocation(vec2i(0, 0)));
    }
    
    stepCount++;
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

// This should be run after any change to the board and possibly right before playing the simulation.
void GameState::updateComponentConnections()
{
	for (const auto &component : components)
	{
		// clear the last attempt to find connections
		if (component->circuitBoard != nullptr) continue;
		for (int i = 0; i < 12; i++)
			component->connections[i].component = connectableComponentAtRelativePosition(component, constants::nearbyComponents[i]);

		if (component->info->name == "TeleportSource" || component->info->name == "TrapReset" || component->info->name == "GateSwitch")
			component->target = findClosestMatch(component);
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
	if (component->info->name =="CircuitBoundary" && target->info->name == "CircuitBoundary")
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

	if (displacement == vec2i(0,-2) && circuitPosition == vec2i(1,0)) return vec2i(1, 6);
	if (displacement == vec2i(0,-2) && circuitPosition == vec2i(2,0)) return vec2i(2, 6);
	if (displacement == vec2i(0,-2) && circuitPosition == vec2i(3,0)) return vec2i(3, 6);
	if (displacement == vec2i(0,-2) && circuitPosition == vec2i(4,0)) return vec2i(4, 6);
	if (displacement == vec2i(0,-2) && circuitPosition == vec2i(5,0)) return vec2i(5, 6);

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


Component* GameState::findClosestMatch(Component *start) {
	// this finds the closest component of the corresponding type.  It allways prioritizes components that are in the same circuit.
	float minDist = 1000.0f;
	Component* bestComponent = nullptr;

	for (const auto &c : components)
	{
		// check if the target component is the type of component that we want.
		if (start->info->name == "TeleportSource")
		{
			if (c->info->name != "TeleportDestination" || start->modifiers.color != c->modifiers.color)
				continue;
		}
		else if (start->info->name == "TrapReset")
		{
			if (start->modifiers.color != ChargeType::Gray || c->modifiers.color != ChargeType::Gray || (c->info->name != "TrapOpen" && c->info->name != "TrapSprung"))
				continue;
		}
		else if (start->info->name == "GateSwitch")
		{
			if (start->modifiers.color != ChargeType::Gray || c->modifiers.color != ChargeType::Gray || (c->info->name != "GateOpen" && c->info->name != "GateClosed"))
				continue;
		}
		else continue;

		// minimize
		float dist = (float) (start->location.modifiedDistanceTo(c->location));
		if (dist < minDist)
		{
			minDist = dist;
			bestComponent = c;
		}
	}

	return bestComponent;
}
