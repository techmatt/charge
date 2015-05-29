
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
    storedCharge = ChargeNone;
    resetPowerSource();
}

void Component::resetPowerSource()
{
    stepsUntilEmission = secondsBeforeFirstEmission * constants::stepsPerSecond;
    totalChargesRemaining = totalCharges;
}

bool Component::willAcceptCharge(GameState &state, const Charge &charge)
{
    int currentTimeDifference = state.stepCount - lastChargeVisit;
    int requiredTimeDifference = constants::chargeRequiredTimeDifference;

    if (currentTimeDifference < requiredTimeDifference)
        return false;

    if (info->name == "PowerSource" || info->name == "TrapSprung" || info->name == "Blocker" || info->name == "GateClosed")
        return false;

    if (info->name == "ChargeFilter")
        return (charge.level == modifiers.color);

    if (info->name == "FilteredAmplifier")
    {
        const int combinedCharge = (int)storedCharge + (int)charge.level;
        return (storedCharge == ChargeNone || (combinedCharge >= modifiers.color));
    }

    if (modifiers.boundary == CircuitBoundaryClosed)
        return false;

    return true;
}

void Component::tick()
{
	// reset whether charges are targetting 
	numChargesTargetingThisTick = 0;
	
	if (deathTrapTimeLeft > 0)
        deathTrapTimeLeft--;

    if (info->hasStoredChargeLayer)
    {
        const float s = 0.95f;
        vec4f targetColor = storedCharge == ChargeNone ? GameUtil::chargeColor(ChargeGray) : GameUtil::chargeColor(storedCharge);
        modifiers.storedChargeColor = modifiers.storedChargeColor * s + targetColor * (1.0f - s);
    }

    if (info->name == "PowerSource"
        && totalChargesRemaining > 0)
    {
        if (stepsUntilEmission > 0)
        {
            stepsUntilEmission--;
        }
        else
        {
            chargesToEmit.push_back(modifiers.color);
            
            stepsUntilEmission = secondsPerEmission * constants::stepsPerSecond;
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

rect2f Component::boardFrameLocation() const {
	if (!(location.inCircuit())) return rect2f(vec2f(location.boardPos), location.boardPos + vec2f(1.0f, 1.0f));
	CoordinateFrame frame = CoordinateFrame(location.circuitPos, location.circuitPos + vec2f(1.0f, 1.0f), vec2i(constants::circuitBoardSize, constants::circuitBoardSize));
	return frame.toContainer(rect2f(location.circuitPos, location.circuitPos + vec2f(1.0f, 1.0f)));
}

rect2f Component::circuitFrameLocation() const {
	//if (!location.inCircuit()) return nullptr;  // this shouldn't happen anyway
	return rect2f(location.boardPos, location.boardPos + vec2f(1.0f, 1.0f));
}