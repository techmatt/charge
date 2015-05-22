
#include "main.h"

Charge::Charge(const GameLocation &originLocation, ChargeType _level)
{
    level = _level;
    source = originLocation;
    destination = originLocation;
    timeInTransit = 0;
    totalTransitTime = 1;
    randomRotationOffset = 360.0f * (float)rand() / (float)RAND_MAX;

    markedForDeletion = false;
    showDeathAnimation = true;
}

void Charge::advance(GameState &state)
{
    if (markedForDeletion || totalTransitTime == 0) return;

    if (timeInTransit < totalTransitTime)
    {
        timeInTransit++;
    }

    Component *current = state.getComponent(destination);
    if (current->deathTrapTimeLeft > 0)
    {
        markedForDeletion = true;
    }
}

void Charge::interactWithDestination(GameState &state)
{
    if (markedForDeletion || totalTransitTime == 0) return;

    if (timeInTransit < totalTransitTime)
    {
        return;
    }

    Component *current = state.getComponent(destination);

    if (current->info->name == "Amplifier")
    {
        markedForDeletion = true;
        showDeathAnimation = false;
        if (current->modifiers.storedColor == ChargeNone)
        {
            current->modifiers.storedColor = level;
        }
        else
        {
            const ChargeType emittedLevel = (ChargeType)(std::min((int)current->modifiers.storedColor + (int)level, (int)ChargeBlue));
            current->chargesToEmit.push_back(emittedLevel);
            current->modifiers.storedColor = ChargeNone;
        }
    }

    if (current->info->name == "TrapOpen")
    {
        current->info = &database().getComponent("TrapSprung");
    }

    if (current->info->name == "TrapReset")
    {
        if (current->modifiers.color == ChargeGray)
        {
            // TODO: find closest gray trap and reset it
        }
        else
        {
            for (auto &component : state.components)
            {
                if (component->modifiers.color == current->modifiers.color && component->info->name == "TrapSprung")
                {
                    component->info = &database().getComponent("TrapOpen");
                }
            }
        }
    }

    if (current->info->name == "GateSwitch")
    {
        if (current->modifiers.color == ChargeGray)
        {
            // TODO: find closest gray door and reset it
        }
        else
        {
            for (auto &component : state.components)
            {
                if (component->modifiers.color == current->modifiers.color && component->info->name == "GateOpen")
                {
                    component->info = &database().getComponent("GateClosed");
                }
                else if (component->modifiers.color == current->modifiers.color && component->info->name == "GateClosed")
                {
                    component->info = &database().getComponent("GateOpen");
                }
            }
        }
    }

    if (current->info->name == "Splitter")
    {
        if (level != ChargeRed)
        {
            markedForDeletion = true;
            showDeathAnimation = false;

            switch (level)
            {
            case ChargeOrange:
                current->chargesToEmit.push_back(ChargeRed);
                current->chargesToEmit.push_back(ChargeRed);
                break;
            case ChargeYellow:
                current->chargesToEmit.push_back(ChargeRed);
                current->chargesToEmit.push_back(ChargeRed);
                current->chargesToEmit.push_back(ChargeRed);
                break;
            case ChargeGreen:
                current->chargesToEmit.push_back(ChargeOrange);
                current->chargesToEmit.push_back(ChargeOrange);
                break;
            case ChargeBlue:
                current->chargesToEmit.push_back(ChargeYellow);
                current->chargesToEmit.push_back(ChargeYellow);
                break;
            }
        }
    }

    /*if (result.destination->info->holdsCharge)
    {
        result.destination->lastChargeVisit = state.stepCount - constants::chargeRequiredTimeDifference + 2;
    }*/
}

void Charge::updateDestination(GameState &state)
{
    if (markedForDeletion) return;
    if (timeInTransit < totalTransitTime) return;

    if (!findBestDestination(state))
    {
        Component *previous = state.getComponent(source);
        Component *current = state.getComponent(destination);
        //previous->deathTrapTimeLeft = constants::deathTrapDuration;
        current->deathTrapTimeLeft = constants::deathTrapDuration;
        markedForDeletion = true;
    }

    /*if (result.destination->info->holdsCharge)
    {
        result.destination->lastChargeVisit = state.stepCount - constants::chargeRequiredTimeDifference + 2;
    }*/

    //
    // Handle towers that consume charge.  Do not allow amplifiers or splitters to consume the charge
    // they just produced.
    //
    
}

void Charge::setNewDestination(GameState &state, Component &newDestination)
{
    source = destination;
    destination = newDestination.location;
    newDestination.lastChargeVisit = state.stepCount;

    timeInTransit = 0;
    totalTransitTime = constants::chargeTransitTime;
    
    if (newDestination.modifiers.speed == WireMinorDelay)
    {
        totalTransitTime *= 2;
    }
    if (newDestination.modifiers.speed == WireMajorDelay)
    {
        totalTransitTime *= 4;
    }
    if (newDestination.modifiers.speed == WireMinorAccelerator)
    {
        totalTransitTime = totalTransitTime * 2 / 3;
    }
    if (newDestination.modifiers.speed == WireMajorAccelerator)
    {
        totalTransitTime /= 2;
    }
}

bool Charge::findBestDestination(GameState &state)
{
    Component *previous = state.getComponent(source);
    Component *current = state.getComponent(destination);

    int adjacentCount;
    Component *adjacentComponents[6];
    if (current->location.inCircuit())
    {
        if (current->info->name == "CircuitBoundary")
        {
            // TODO
            adjacentCount = 0;
        }
        else
        {
            state.getCircuit(current->location).circuitBoard->findAdjacentBuildings(current->location.boardPos, adjacentComponents, adjacentCount);
        }
    }
    else
    {
        state.board.findAdjacentBuildings(destination.boardPos, adjacentComponents, adjacentCount);
    }

    double strongestPreference = 0.0;
    Component *bestComponent = nullptr;

    for (int adjacentIndex = 0; adjacentIndex < adjacentCount; adjacentIndex++)
    {
        Component *candidate = adjacentComponents[adjacentIndex];

        bool isPreviousBuilding = previous->location == candidate->location;
        bool buildingWillAccept = candidate->willAcceptCharge(state, *this);
        
        if (!isPreviousBuilding &&
            buildingWillAccept)
        {
            double currentPreference = computePreference(state, *candidate);

            if (currentPreference > strongestPreference)
            {
                strongestPreference = currentPreference;
                bestComponent = candidate;
            }
        }
    }
    if (bestComponent != nullptr)
    {
        setNewDestination(state, *bestComponent);
        return true;
    }
    return false;
}

double Charge::computePreference(GameState &state, Component &targetComponent)
{
    double preference = targetComponent.modifiers.chargePreference * 1000000.0;

    const int currentTimeDifference = std::min(state.stepCount - targetComponent.lastChargeVisit, 900000);
    preference += currentTimeDifference;

    if (targetComponent.deathTrapTimeLeft > 0)
        preference = -1000000.0;

    return preference;
}
