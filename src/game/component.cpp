
#include "main.h"

Component::~Component()
{
	if (circuitBoard)
	{
		delete circuitBoard;
	}
}

void Component::resetPuzzle()
{
    info = baseInfo;
    lastChargeVisit = -10000;
    modifiers.storedColor = ChargeNone;
    resetPowerSource();
}

void Component::resetPowerSource()
{
    timeUntilEmission = 60;
    totalChargesRemaining = 0xFFFFFF;
}

bool Component::willAcceptCharge(GameState &state, const Charge &charge)
{
    int currentTimeDifference = state.stepCount - lastChargeVisit;
    int requiredTimeDifference = constants::chargeRequiredTimeDifference;

    if (currentTimeDifference < requiredTimeDifference)
    {
        return false;
    }

    if (info->name == "PowerSource" || info->name == "TrapSprung" || info->name == "Blocker" || info->name == "GateClosed")
    {
        return false;
    }

    if (info->name == "ChargeFilter")
    {
        return (charge.level == modifiers.color);
    }

    return true;
}

void Component::tick()
{
    if (deathTrapTimeLeft > 0)
        deathTrapTimeLeft--;

    if (info->name == "PowerSource"
        && totalChargesRemaining > 0)
    {
        if (timeUntilEmission > 0)
        {
            timeUntilEmission--;
        }
        else
        {
            chargesToEmit.push_back(modifiers.color);
            
            //TODO: parameterize this better
            timeUntilEmission = 60;
            totalChargesRemaining--;
        }
    }
}

ParameterTable Component::toTable(const string &tableName) const
{
    ParameterTable result("Component");

    result.set("name", info->name);
    result.set("color", modifiers.color);
    result.set("chargePreference", modifiers.chargePreference);
    result.set("speed", modifiers.speed);
    result.set("boardPos", location.boardPos);
    result.set("circuitPos", location.circuitPos);

    return result;
}