
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

	held = false;
}

void Charge::advance(GameState &state)
{
    if (markedForDeletion || totalTransitTime == 0) return;

	// chrage is moving.
	if (timeInTransit < totalTransitTime && source != destination && totalTransitTime != 1)
	{
		timeInTransit++;
		resolvedThisTick = true;
	
		//seach for the source and destination and block off the corresponding things
		Component* sourceComponent = state.getComponent(source);
		Component* destinationComponent = state.getComponent(destination);

		destinationComponent->connections[(intendedConnectionIndex + 6) % 12].blocked = true;
	
	}
}

void Charge::interactWithDestination(GameState &state)
{
    if (markedForDeletion || totalTransitTime == 0) return;

    if (timeInTransit < totalTransitTime ||
        source == destination) // do not interact with the charge origin
    {
        return;
    }

    Component *current = state.getComponent(destination);

    if (current->info->name == "Amplifier")
    {
        markedForDeletion = true;
        showDeathAnimation = false;
        if (current->storedCharge == ChargeNone)
        {
            current->storedCharge = level;
        }
        else
        {
            const ChargeType emittedLevel = (ChargeType)(std::min( std::max((int)current->storedCharge, (int)level) + 1, (int)ChargeBlue));
            current->chargesToEmit.push_back( make_pair(emittedLevel, source) );
            current->modifiers.storedChargeColor = GameUtil::chargeColor(emittedLevel);
            current->storedCharge = ChargeNone;
        }
    }

    if (current->info->name == "FilteredAmplifier")
    {
        markedForDeletion = true;
        showDeathAnimation = false;
        if (current->storedCharge == ChargeNone)
        {
            current->storedCharge = (ChargeType)std::min((int)level, (int)current->modifiers.color);
        }
        else
        {
            const ChargeType emittedLevel = current->modifiers.color;
            current->chargesToEmit.push_back( make_pair(emittedLevel, source) );
            current->modifiers.storedChargeColor = GameUtil::chargeColor(emittedLevel);
            current->storedCharge = ChargeNone;
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
            // find closest gray trap and reset it
			if (current->target!=nullptr && current->target->info->name == "TrapSprung")
				current->target->info = &database().getComponent("TrapOpen");
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
			// find closest gray door and reset it
			if (current->target != nullptr && current->target->info->name == "GateOpen")
				current->target->info = &database().getComponent("GateClosed");
			else if (current->target != nullptr && current->target->info->name == "GateClosed")
				current->target->info = &database().getComponent("GateOpen");
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

            auto red = make_pair(ChargeRed, source);
            auto orange = make_pair(ChargeOrange, source);
            auto yellow = make_pair(ChargeYellow, source);

            switch (level)
            {
            case ChargeOrange:
                current->chargesToEmit.push_back(red);
                current->chargesToEmit.push_back(red);
                break;
            case ChargeYellow:
                current->chargesToEmit.push_back(red);
                current->chargesToEmit.push_back(red);
                current->chargesToEmit.push_back(red);
                break;
            case ChargeGreen:
                current->chargesToEmit.push_back(orange);
                current->chargesToEmit.push_back(orange);
                break;
            case ChargeBlue:
                current->chargesToEmit.push_back(yellow);
                current->chargesToEmit.push_back(yellow);
                break;
            }
        }
    }

    /*if (result.destination->info->holdsCharge)
    {
        result.destination->lastChargeVisit = state.stepCount - constants::chargeRequiredTimeDifference + 2;
    }*/

	if (current->info->name == "TeleportSource")
	{
		// if the teleporter has no target, kill the charge
		if (current->target == nullptr || !current->target->willAcceptCharge(state, *this))
		{
			markedForDeletion = true;
			showDeathAnimation = false;
		}
		else
		{
			setNewDestination(state,*(current->target), true);
		}
	}

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

void Charge::setNewDestination(GameState &state, Component &newDestination, bool teleport)
{
	newDestination.connections[(intendedConnectionIndex + 6) % 12].blocked = true; //block off the things coming in the opposite direction
	
	source = destination;
    destination = newDestination.location;
    newDestination.lastChargeVisit = state.stepCount;

    timeInTransit = 0;
    totalTransitTime = constants::chargeTransitTime;
    
	if (teleport)
	{
		totalTransitTime += 1;
		return;
	}


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

    double strongestPreference = 0.0;
    Component *bestComponent = nullptr;
	int bestIndex = -1;

    for (int adjacentIndex = 0; adjacentIndex < 12; adjacentIndex++)
    {
		if (current->connections[adjacentIndex].blocked) continue;
			
		Component *candidate = current->connections[adjacentIndex].component;
		if (candidate == nullptr) continue;


        bool isPreviousBuilding = previous->location == candidate->location;
        bool buildingWillAccept = candidate->willAcceptCharge(state, *this);
        
        if (!isPreviousBuilding &&
            buildingWillAccept)
        {
            double currentPreference = computePreference(state, *candidate);

            if (currentPreference > strongestPreference)
            {
                strongestPreference = currentPreference;
				bestIndex = adjacentIndex;
                bestComponent = candidate;
            }
        }
    }
    if (bestComponent != nullptr)
    {
		intendedDestination = bestComponent;
		intendedConnectionIndex = bestIndex;
		bestComponent->sourceOfLastChargeToAttemptToMoveHere = source;
		bestComponent->connections[(intendedConnectionIndex + 6) % 12].desired = true;
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
