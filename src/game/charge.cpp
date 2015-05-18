
#include "main.h"

Charge::Charge(const GameLocation &originLocation, ChargeType _level)
{
    level = _level;
    source = originLocation;
    destination = originLocation;
    timeInTransit = 0;
    totalTransitTime = 0;
    randomRotationOffset = 360.0f * (float)rand() / (float)RAND_MAX;
}

void Charge::advance(GameState &state)
{
    if (timeInTransit < totalTransitTime)
    {
        timeInTransit++;
    }
}

ChargeUpdateResult Charge::interactWithDestination(GameState &state)
{
    ChargeUpdateResult result;

    if (timeInTransit < totalTransitTime)
    {
        return result;
    }

    

    /*if (result.destination->info->holdsCharge)
    {
        result.destination->lastChargeVisit = state.stepCount - constants::chargeRequiredTimeDifference + 2;
    }*/

    return result;
}

ChargeUpdateResult Charge::updateDestination(GameState &state)
{
    ChargeUpdateResult result;

    if (timeInTransit < totalTransitTime)
    {
        return result;
    }

    if (findBestDestination(state))
    {
        return result;
    }
    else
    {
        result.destroyCharge = true;
        return result;
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
    /*if (NewDestination->Type() == BuildingWireMinorDelay)
    {
        _TotalTransitTime *= 2;
    }
    if (NewDestination->Type() == BuildingWireMajorDelay)
    {
        _TotalTransitTime *= 4;
    }*/
    /*if (NewDestination->Type() == BuildingWireMinorAccelerator)
    {
        _TotalTransitTime = _TotalTransitTime * 2 / 3;
    }
    if (NewDestination->Type() == BuildingWireMajorAccelerator)
    {
        _TotalTransitTime /= 2;
    }*/
}

bool Charge::findBestDestination(GameState &state)
{
    Component *previous = state.getComponent(source);
    Component *current = state.getComponent(destination);

    int adjacentCount;
    Component *adjacentComponents[6];
    if (!destination.inCircuit())
    {
        state.board.findAdjacentBuildings(destination.boardPos, adjacentComponents, adjacentCount);
    }
    else
    {
        //Building *CircuitBuilding = G.CircuitFromLocation(_Destination.Circuit);
        //CircuitBuilding->CircuitBoard().FindAdjacentBuildings(_Destination.Location, AdjacentBuildings, AdjacentCount);
    }

    double strongestPreference = 0.0;
    Component *bestComponent = nullptr;

    for (int adjacentIndex = 0; adjacentIndex < adjacentCount; adjacentIndex++)
    {
        Component *candidate = adjacentComponents[adjacentIndex];

        bool BuildingWillAccept = candidate->willAcceptCharge(state, *this);
        bool IsPreviousBuilding = previous->location == candidate->location;

        if (!IsPreviousBuilding &&
            BuildingWillAccept)
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
    double preference = targetComponent.chargePreference * 1000000.0;

    const int currentTimeDifference = std::min(state.stepCount - targetComponent.lastChargeVisit, 900000);
    preference += currentTimeDifference;

    return preference;
}