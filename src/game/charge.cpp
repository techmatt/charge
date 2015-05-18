
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

ChargeUpdateResult Charge::update(GameState &state, bool secondPass)
{
    ChargeUpdateResult result;
    result.showDeathAnimation = true;
    result.destroyCharge = false;
    
    Component *sourceCircuit = nullptr, *destCircuit = nullptr;

    const auto &cells = state.board.cells;

    if (destination.inCircuit())
    {
        destCircuit = cells(destination.boardPos).c;
        result.destination = destCircuit->circuitBoard->cells(destination.circuitPos).c;
    }
    else
    {
        result.destination = cells(destination.boardPos).c;
    }

    if (secondPass && (result.destination == nullptr || !result.destination->info->holdsCharge))
    {
        return result;
    }

    if (result.destination == nullptr)
    {
        result.destroyCharge = true;
        return result;
    }

    if (source.inCircuit())
    {
        sourceCircuit = cells(source.boardPos).c;
        result.source = sourceCircuit->circuitBoard->cells(source.circuitPos).c;
    }
    else
    {
        result.source = cells(source.boardPos).c;
    }

    if (timeInTransit >= totalTransitTime)
    {
        //Assert(DestBuilding->Type() != BuildingCircuit && (SourceBuilding == NULL || SourceBuilding->Type() != BuildingCircuit), "Cannot move towards circuits");

        if (result.destination->info->holdsCharge)
        {
            result.destination->lastChargeVisit = state.stepCount - constants::chargeRequiredTimeDifference + 2;
        }

        //
        // Handle towers that consume charge.  Do not allow amplifiers or splitters to consume the charge
        // they just produced.
        //
        if (result.destination->info->holdsCharge)
        {
            if (result.destination->absorbedCharge == 0)
            {
                //result.destination->eatCharge(state, *this);
            }
        }
        else if (totalTransitTime != 0)
        {
            result.destroyCharge = true;
            return result;
        }

        //
        // Find a new target in adjacent squares
        //
        if (findNewDestination(state, *result.destination, *result.source))
        {
            if (result.destination->info->holdsCharge)
            {
                result.destination->discharge();
            }
            return result;
        }
        else
        {
            result.source = result.destination;
            return result;
        }
    }
    else
    {
        if (!secondPass)
        {
            timeInTransit++;
        }
        return result;
    }
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

bool Charge::findNewDestination(GameState &state, Component &current, Component &previous)
{
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
        bool IsPreviousBuilding = previous.location == candidate->location;

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
    if (bestComponent != NULL)
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