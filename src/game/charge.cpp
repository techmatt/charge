
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

void Charge::interactWithDestination(GameState &state, AppData &app)
{
    if (markedForDeletion || totalTransitTime == 0) return;

    if (timeInTransit < totalTransitTime ||
        source == destination) // do not interact with the charge origin
    {
        return;
    }

    Component *current = state.getComponent(destination);

    if (current->info->name == "MegaHold")
    {
        markedForDeletion = true;
        showDeathAnimation = false;
        current->megaHoldTotalCharge = min(current->intrinsics.totalChargeRequired, current->megaHoldTotalCharge + (int)level);
        app.playEffect("MegaHold", current->location);
    }

    if (current->info->name == "Amplifier")
    {
        markedForDeletion = true;
        showDeathAnimation = false;
        if (current->storedCharge == ChargeType::None)
        {
            app.playEffect("AmpCharge", current->location);
            current->storedCharge = level;
        }
        else
        {
            app.playEffect("AmpDischarge", current->location);
            const ChargeType emittedLevel = (ChargeType)(std::min( std::max((int)current->storedCharge, (int)level) + 1, (int)ChargeType::Blue));
            current->chargesToEmit.push_back( make_pair(emittedLevel, source) );
            current->modifiers.storedChargeColor = GameUtil::chargeColor(emittedLevel);
            current->storedCharge = ChargeType::None;
        }
    }

    if (current->info->name == "FilteredAmplifier")
    {
        markedForDeletion = true;
        showDeathAnimation = false;
        if (current->storedCharge == ChargeType::None)
        {
            app.playEffect("AmpCharge", current->location);
            current->storedCharge = (ChargeType)std::min((int)level, (int)current->modifiers.color);
        }
        else
        {
            app.playEffect("AmpDischarge", current->location);
            const ChargeType emittedLevel = current->modifiers.color;
            current->chargesToEmit.push_back( make_pair(emittedLevel, source) );
            current->modifiers.storedChargeColor = GameUtil::chargeColor(emittedLevel);
            current->storedCharge = ChargeType::None;
        }
    }

    if (current->info->name == "TrapOpen")
    {
        current->info = &database().getComponent("TrapSprung");
    }

    if (current->info->name == "TrapReset")
    {
        if (current->modifiers.color == ChargeType::Gray)
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
        if (current->modifiers.color == ChargeType::Gray)
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
        app.playEffect("Splitter", current->location);
        if (level != ChargeType::Red)
        {
            markedForDeletion = true;
            showDeathAnimation = false;

            auto red = make_pair(ChargeType::Red, source);
            auto orange = make_pair(ChargeType::Orange, source);
            auto yellow = make_pair(ChargeType::Yellow, source);

            switch (level)
            {
            case ChargeType::Orange:
                current->chargesToEmit.push_back(red);
                current->chargesToEmit.push_back(red);
                break;
            case ChargeType::Yellow:
                current->chargesToEmit.push_back(red);
                current->chargesToEmit.push_back(red);
                current->chargesToEmit.push_back(red);
                break;
            case ChargeType::Green:
                current->chargesToEmit.push_back(orange);
                current->chargesToEmit.push_back(orange);
                break;
            case ChargeType::Blue:
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
			showDeathAnimation = true;
		}
		else
		{
            app.playEffect("Teleporter", current->location);
			setNewDestination(state,*(current->target), true);
		}
	}

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

    if (newDestination.modifiers.speed == WireType::MinorDelay)
    {
        totalTransitTime *= 2;
    }
    if (newDestination.modifiers.speed == WireType::MajorDelay)
    {
        totalTransitTime *= 4;
    }
    if (newDestination.modifiers.speed == WireType::MinorAccelerator)
    {
        totalTransitTime = totalTransitTime * 2 / 3;
    }
    if (newDestination.modifiers.speed == WireType::MajorAccelerator)
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
