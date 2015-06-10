
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
    storedCharge = ChargeType::None;
    heldCharge = ChargeType::None;
    modifiers.storedChargeColor = GameUtil::chargeColor(ChargeType::Gray);
    resetPowerSource();
}

void Component::resetPowerSource()
{
    stepsUntilEmission = intrinsics.secondsBeforeFirstEmission * constants::stepsPerSecond;
    totalChargesRemaining = intrinsics.totalCharges;
}

bool Component::willAcceptCharge(GameState &state, const Charge &charge)
{
	if (charge.source == sourceOfLastChargeToAttemptToMoveHere)
		return false;

    int currentTimeDifference = state.stepCount - lastChargeVisit;
    int requiredTimeDifference = constants::chargeRequiredTimeDifference;

    if (currentTimeDifference < requiredTimeDifference && info->name != "MegaHold") //Mega holds can accept multiple charges simultaniously.
        return false;
	
    if (info->name == "PowerSource" || info->name == "TrapSprung" || info->name == "Blocker" || info->name == "GateClosed")
        return false;

    if (info->name == "ChargeFilter")
        return (charge.level == modifiers.color);

    if (info->name == "FilteredAmplifier")
    {
        const int combinedCharge = (int)storedCharge + (int)charge.level;
        return (storedCharge == ChargeType::None || (combinedCharge >= (int)modifiers.color));
    }

    if (heldCharge != ChargeType::None)
        return false;

    if (modifiers.boundary == CircuitBoundaryType::Closed)
        return false;

    return true;
}

void Component::tickGraphics()
{
    if (info->hasStoredChargeLayer)
    {
        const float s = 0.95f;
        vec4f targetColor = GameUtil::chargeColor(ChargeType::Gray);

        if (storedCharge != ChargeType::None) targetColor = GameUtil::chargeColor(storedCharge);
        if (heldCharge != ChargeType::None) targetColor = GameUtil::chargeColor(heldCharge);

        modifiers.storedChargeColor = modifiers.storedChargeColor * s + targetColor * (1.0f - s);
    }
}

void Component::tick(AppData &app)
{
	if (deathTrapTimeLeft > 0)
        deathTrapTimeLeft--;

    if (info->name == "PowerSource"
        && totalChargesRemaining > 0)
    {
        if (stepsUntilEmission > 0)
        {
            stepsUntilEmission--;
        }
        else
        {
            // TODO: make a good power source sound
            //app.playEffect("PowerSource");
            chargesToEmit.push_back( make_pair(modifiers.color, location) );
            
            stepsUntilEmission = intrinsics.secondsPerEmission * constants::stepsPerSecond;
            totalChargesRemaining--;
        }
    }
}

ParameterTable Component::toTable(const string &tableName) const
{
    ParameterTable result("Component");

    result.set("name", info->name);

    if (modifiers.color != ChargeType::None)
        result.set("color", (int)modifiers.color);

    result.set("boardPos", location.boardPos);

    if (location.circuitPos != constants::invalidCoord)
        result.set("circuitPos", location.circuitPos);

    if (modifiers.chargePreference != 2)
        result.set("chargePreference", (int)modifiers.chargePreference);

    if (modifiers.speed != WireType::Standard)
        result.set("speed", (int)modifiers.speed);

    if (modifiers.boundary != CircuitBoundaryType::Invalid)
        result.set("boundary", (int)modifiers.boundary);

    result.set("puzzleType", (int)modifiers.puzzleType);

    if (info->name == "PowerSource")
    {
        result.set("secondsBeforeFirstEmission", intrinsics.secondsBeforeFirstEmission);
        result.set("secondsPerEmission", intrinsics.secondsPerEmission);
        result.set("totalCharges", intrinsics.totalCharges);
    }

    if (info->name == "MegaHold")
    {
        result.set("ticksPerDischarge", intrinsics.ticksPerDischarge);
        result.set("chargesLostPerDischarge", intrinsics.chargesLostPerDischarge);
    }

    return result;
}

Component* Component::fromTable(const ParameterTable &table)
{
    ChargeType color = ChargeType::None;
    vec2i circuitPos = constants::invalidCoord;

    if (table.hasParameter("color"))
        color = (ChargeType)table.getInt("color");

    if (table.hasParameter("circuitPos"))
        circuitPos = table.getVec2i("circuitPos");

    Component *result = new Component(table.getString("name"), color, GameLocation(table.getVec2i("boardPos"), circuitPos));

    if (table.hasParameter("chargePreference"))
        result->modifiers.chargePreference = table.getInt("chargePreference");

    if (table.hasParameter("speed"))
        result->modifiers.speed = (WireType)table.getInt("speed");

    if (table.hasParameter("boundary"))
        result->modifiers.boundary = (CircuitBoundaryType)table.getInt("boundary");

    result->modifiers.puzzleType = (ComponentPuzzleType)table.getInt("puzzleType");

    if (result->info->name == "PowerSource")
    {
        result->intrinsics.secondsBeforeFirstEmission = table.getInt("secondsBeforeFirstEmission");
        result->intrinsics.secondsPerEmission = table.getInt("secondsPerEmission");
        result->intrinsics.totalCharges = table.getInt("totalCharges");
    }

    if (result->info->name == "MegaHold")
    {
        if (table.hasParameter("ticksPerDischarge"))
        {
            result->intrinsics.ticksPerDischarge = table.getInt("ticksPerDischarge");
            result->intrinsics.chargesLostPerDischarge = table.getInt("chargesLostPerDischarge");
        }
        else
        {
            cout << "MegaHold with no metadata found" << endl;
        }
    }

    return result;
}

rect2f Component::boardFrameLocation() const {
	if (!(location.inCircuit())) return rect2f(vec2f(location.boardPos), location.boardPos + vec2f(2.0f, 2.0f));
	CoordinateFrame frame = CoordinateFrame(location.boardPos, location.boardPos + vec2f(2.0f, 2.0f), vec2i(constants::circuitBoardSize, constants::circuitBoardSize));
	return frame.toContainer(rect2f(location.circuitPos, location.circuitPos + vec2f(1.0f, 1.0f)));
}

rect2f Component::circuitFrameLocation() const {
	//if (!location.inCircuit()) return nullptr;  // this shouldn't happen anyway
	return rect2f(location.boardPos, location.boardPos + vec2f(1.0f, 1.0f));
}