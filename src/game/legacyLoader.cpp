
#include "main.h"

enum LegacyComponentType
{
    ComponentPowerSourceRed,
    ComponentPowerSourceOrange,
    ComponentPowerSourceYellow,
    ComponentPowerSourceGreen,
    ComponentPowerSourceBlue,

    ComponentSplitter,
    ComponentAmplifier,
    ComponentAmplifierL1,
    ComponentAmplifierL2,
    ComponentAmplifierL3,
    ComponentAmplifierL4,
    ComponentStreamer,

    ComponentRedChargeGoal,
    ComponentOrangeChargeGoal,
    ComponentYellowChargeGoal,
    ComponentGreenChargeGoal,
    ComponentBlueChargeGoal,

    ComponentTeleportRedSource,
    ComponentTeleportOrangeSource,
    ComponentTeleportYellowSource,
    ComponentTeleportGreenSource,
    ComponentTeleportBlueSource,

    ComponentRedChargeFilter,
    ComponentOrangeChargeFilter,
    ComponentYellowChargeFilter,
    ComponentGreenChargeFilter,
    ComponentBlueChargeFilter,

    ComponentTeleportRedDestination,
    ComponentTeleportOrangeDestination,
    ComponentTeleportYellowDestination,
    ComponentTeleportGreenDestination,
    ComponentTeleportBlueDestination,

    ComponentCircuit,
    ComponentMultiplexer,
    ComponentHold,

    //
    // Upgrades from wire component
    //
    ComponentWireMajorDelay,
    ComponentWireMinorDelay,
    ComponentWire,
    ComponentWireMinorAccelerator,
    ComponentWireMajorAccelerator,

    ComponentBlocker,

    ComponentCircuitBoundaryOpen,
    ComponentCircuitBoundaryBase,
    ComponentCircuitBoundaryBlocked,

    ComponentCircuitRotate90,
    ComponentCircuitRotateN90,
    ComponentCircuitFlipHorizontal,
    ComponentCircuitFlipVertical,
    ComponentCircuitCopy,
    ComponentCircuitPaste,

    ComponentGateOpenRed,
    ComponentGateOpenOrange,
    ComponentGateOpenYellow,
    ComponentGateOpenGreen,
    ComponentGateOpenBlue,

    ComponentGateClosedRed,
    ComponentGateClosedOrange,
    ComponentGateClosedYellow,
    ComponentGateClosedGreen,
    ComponentGateClosedBlue,

    ComponentGateSwitchRed,
    ComponentGateSwitchOrange,
    ComponentGateSwitchYellow,
    ComponentGateSwitchGreen,
    ComponentGateSwitchBlue,

    ComponentMusicBasic,
    ComponentMusicSharp,
    ComponentMusicFlat,

    ComponentMegaHoldRed,
    ComponentMegaHoldOrange,
    ComponentMegaHoldYellow,
    ComponentMegaHoldGreen,
    ComponentMegaHoldBlue,

    ComponentMegaHoldAll,

    ComponentTrapOpenGray,
    ComponentTrapSprungGray,

    ComponentGateOpenGray,
    ComponentGateClosedGray,
    ComponentGateSwitchGray,

    ComponentTrapOpenRed,
    ComponentTrapOpenOrange,
    ComponentTrapOpenYellow,
    ComponentTrapOpenGreen,
    ComponentTrapOpenBlue,

    ComponentTrapSprungRed,
    ComponentTrapSprungOrange,
    ComponentTrapSprungYellow,
    ComponentTrapSprungGreen,
    ComponentTrapSprungBlue,

    ComponentTrapResetRed,
    ComponentTrapResetOrange,
    ComponentTrapResetYellow,
    ComponentTrapResetGreen,
    ComponentTrapResetBlue,
    ComponentTrapResetGray,

    ComponentCount,
};

struct LegacyComponent
{
    LegacyComponentType legacyType;
    GameLocation location;
    int chargePreference;
    ComponentPuzzleType puzzleType;

    static const int fieldCount = 8;
    int fieldValues[fieldCount];
};

struct LegacyComponentInfo
{
    LegacyComponentInfo(const string _name, ChargeType _color = ChargeType::None, WireType _speed = WireType::Standard)
    {
        name = _name;
        color = _color;
        speed = _speed;
        boundary = CircuitBoundaryType::Invalid;
    }
    LegacyComponentInfo(CircuitBoundaryType _boundary)
    {
        name = "CircuitBoundary";
        color = ChargeType::None;
        speed = WireType::Standard;
        boundary = _boundary;
    }
    string name;
    ChargeType color;
    WireType speed;
    CircuitBoundaryType boundary;
};

LegacyComponentInfo getLegacyInfo(LegacyComponentType type)
{
    switch (type)
    {
    case ComponentCircuit: return LegacyComponentInfo("Circuit");

    case ComponentPowerSourceRed: return LegacyComponentInfo("PowerSource", ChargeType::Red);
    case ComponentPowerSourceOrange: return LegacyComponentInfo("PowerSource", ChargeType::Orange);
    case ComponentPowerSourceYellow: return LegacyComponentInfo("PowerSource", ChargeType::Yellow);
    case ComponentPowerSourceGreen: return LegacyComponentInfo("PowerSource", ChargeType::Green);
    case ComponentPowerSourceBlue: return LegacyComponentInfo("PowerSource", ChargeType::Blue);

    case ComponentSplitter: return LegacyComponentInfo("Splitter");

    case ComponentAmplifier: return LegacyComponentInfo("Amplifier");
    case ComponentAmplifierL1: return LegacyComponentInfo("FilteredAmplifier", ChargeType::Orange);
    case ComponentAmplifierL2: return LegacyComponentInfo("FilteredAmplifier", ChargeType::Yellow);
    case ComponentAmplifierL3: return LegacyComponentInfo("FilteredAmplifier", ChargeType::Green);
    case ComponentAmplifierL4: return LegacyComponentInfo("FilteredAmplifier", ChargeType::Blue);

    case ComponentRedChargeGoal: return LegacyComponentInfo("ChargeGoal", ChargeType::Red);
    case ComponentOrangeChargeGoal: return LegacyComponentInfo("ChargeGoal", ChargeType::Orange);
    case ComponentYellowChargeGoal: return LegacyComponentInfo("ChargeGoal", ChargeType::Yellow);
    case ComponentGreenChargeGoal: return LegacyComponentInfo("ChargeGoal", ChargeType::Green);
    case ComponentBlueChargeGoal: return LegacyComponentInfo("ChargeGoal", ChargeType::Blue);

    case ComponentTeleportRedSource: return LegacyComponentInfo("TeleportSource", ChargeType::Red);
    case ComponentTeleportOrangeSource: return LegacyComponentInfo("TeleportSource", ChargeType::Orange);
    case ComponentTeleportYellowSource: return LegacyComponentInfo("TeleportSource", ChargeType::Yellow);
    case ComponentTeleportGreenSource: return LegacyComponentInfo("TeleportSource", ChargeType::Green);
    case ComponentTeleportBlueSource: return LegacyComponentInfo("TeleportSource", ChargeType::Blue);

    case ComponentTeleportRedDestination: return LegacyComponentInfo("TeleportDestination", ChargeType::Red);
    case ComponentTeleportOrangeDestination: return LegacyComponentInfo("TeleportDestination", ChargeType::Orange);
    case ComponentTeleportYellowDestination: return LegacyComponentInfo("TeleportDestination", ChargeType::Yellow);
    case ComponentTeleportGreenDestination: return LegacyComponentInfo("TeleportDestination", ChargeType::Green);
    case ComponentTeleportBlueDestination: return LegacyComponentInfo("TeleportDestination", ChargeType::Blue);

    case ComponentRedChargeFilter: return LegacyComponentInfo("ChargeFilter", ChargeType::Red);
    case ComponentOrangeChargeFilter: return LegacyComponentInfo("ChargeFilter", ChargeType::Orange);
    case ComponentYellowChargeFilter: return LegacyComponentInfo("ChargeFilter", ChargeType::Yellow);
    case ComponentGreenChargeFilter: return LegacyComponentInfo("ChargeFilter", ChargeType::Green);
    case ComponentBlueChargeFilter: return LegacyComponentInfo("ChargeFilter", ChargeType::Blue);

    case ComponentWireMajorDelay: return LegacyComponentInfo("Wire", ChargeType::None, WireType::MajorDelay);
    case ComponentWireMinorDelay: return LegacyComponentInfo("Wire", ChargeType::None, WireType::MinorDelay);
    case ComponentWire: return LegacyComponentInfo("Wire", ChargeType::None, WireType::Standard);
    case ComponentWireMinorAccelerator: return LegacyComponentInfo("Wire", ChargeType::None, WireType::MinorAccelerator);
    case ComponentWireMajorAccelerator: return LegacyComponentInfo("Wire", ChargeType::None, WireType::MajorAccelerator);

    case ComponentBlocker: return LegacyComponentInfo("Blocker");

    //NOTE: I am not sure where exactly ComponentCircuitBoundaryOpen occurs; Open components seem to actually be represented as Base.
    case ComponentCircuitBoundaryOpen: return LegacyComponentInfo(CircuitBoundaryType::Open);
    case ComponentCircuitBoundaryBase: return LegacyComponentInfo(CircuitBoundaryType::Open);
    case ComponentCircuitBoundaryBlocked: return LegacyComponentInfo(CircuitBoundaryType::Closed);

    case ComponentHold: return LegacyComponentInfo("Hold");

    case ComponentGateOpenRed: return LegacyComponentInfo("GateOpen", ChargeType::Red);
    case ComponentGateOpenOrange: return LegacyComponentInfo("GateOpen", ChargeType::Orange);
    case ComponentGateOpenYellow: return LegacyComponentInfo("GateOpen", ChargeType::Yellow);
    case ComponentGateOpenGreen: return LegacyComponentInfo("GateOpen", ChargeType::Green);
    case ComponentGateOpenBlue: return LegacyComponentInfo("GateOpen", ChargeType::Blue);
    case ComponentGateOpenGray: return LegacyComponentInfo("GateOpen", ChargeType::Gray);

    case ComponentGateClosedRed: return LegacyComponentInfo("GateClosed", ChargeType::Red);
    case ComponentGateClosedOrange: return LegacyComponentInfo("GateClosed", ChargeType::Orange);
    case ComponentGateClosedYellow: return LegacyComponentInfo("GateClosed", ChargeType::Yellow);
    case ComponentGateClosedGreen: return LegacyComponentInfo("GateClosed", ChargeType::Green);
    case ComponentGateClosedBlue: return LegacyComponentInfo("GateClosed", ChargeType::Blue);
    case ComponentGateClosedGray: return LegacyComponentInfo("GateClosed", ChargeType::Gray);

    case ComponentGateSwitchRed: return LegacyComponentInfo("GateSwitch", ChargeType::Red);
    case ComponentGateSwitchOrange: return LegacyComponentInfo("GateSwitch", ChargeType::Orange);
    case ComponentGateSwitchYellow: return LegacyComponentInfo("GateSwitch", ChargeType::Yellow);
    case ComponentGateSwitchGreen: return LegacyComponentInfo("GateSwitch", ChargeType::Green);
    case ComponentGateSwitchBlue: return LegacyComponentInfo("GateSwitch", ChargeType::Blue);
    case ComponentGateSwitchGray: return LegacyComponentInfo("GateSwitch", ChargeType::Gray);

    case ComponentMegaHoldRed: return LegacyComponentInfo("MegaHold", ChargeType::Red);
    case ComponentMegaHoldOrange: return LegacyComponentInfo("MegaHold", ChargeType::Orange);
    case ComponentMegaHoldYellow: return LegacyComponentInfo("MegaHold", ChargeType::Yellow);
    case ComponentMegaHoldGreen: return LegacyComponentInfo("MegaHold", ChargeType::Green);
    case ComponentMegaHoldBlue: return LegacyComponentInfo("MegaHold", ChargeType::Blue);
    case ComponentMegaHoldAll: return LegacyComponentInfo("MegaHold", ChargeType::Gray);

    case ComponentTrapOpenRed: return LegacyComponentInfo("TrapOpen", ChargeType::Red);
    case ComponentTrapOpenOrange: return LegacyComponentInfo("TrapOpen", ChargeType::Orange);
    case ComponentTrapOpenYellow: return LegacyComponentInfo("TrapOpen", ChargeType::Yellow);
    case ComponentTrapOpenGreen: return LegacyComponentInfo("TrapOpen", ChargeType::Green);
    case ComponentTrapOpenBlue: return LegacyComponentInfo("TrapOpen", ChargeType::Blue);
    case ComponentTrapOpenGray: return LegacyComponentInfo("TrapOpen", ChargeType::Gray);

    case ComponentTrapSprungRed: return LegacyComponentInfo("TrapSprung", ChargeType::Red);
    case ComponentTrapSprungOrange: return LegacyComponentInfo("TrapSprung", ChargeType::Orange);
    case ComponentTrapSprungYellow: return LegacyComponentInfo("TrapSprung", ChargeType::Yellow);
    case ComponentTrapSprungGreen: return LegacyComponentInfo("TrapSprung", ChargeType::Green);
    case ComponentTrapSprungBlue: return LegacyComponentInfo("TrapSprung", ChargeType::Blue);
    case ComponentTrapSprungGray: return LegacyComponentInfo("TrapSprung", ChargeType::Gray);

    case ComponentTrapResetRed: return LegacyComponentInfo("TrapReset", ChargeType::Red);
    case ComponentTrapResetOrange: return LegacyComponentInfo("TrapReset", ChargeType::Orange);
    case ComponentTrapResetYellow: return LegacyComponentInfo("TrapReset", ChargeType::Yellow);
    case ComponentTrapResetGreen: return LegacyComponentInfo("TrapReset", ChargeType::Green);
    case ComponentTrapResetBlue: return LegacyComponentInfo("TrapReset", ChargeType::Blue);
    case ComponentTrapResetGray: return LegacyComponentInfo("TrapReset", ChargeType::Gray);
    }

    MLIB_ERROR("unepxected component found");
    return LegacyComponentInfo("error");
}

void LegacyLoader::load(const string &filename, GameState &result)
{
    const vector<string> lines = util::getFileLines(filename);

    int lineIndex = 0;
    int componentCount = convert::toInt(lines[lineIndex++]);

    vector<LegacyComponent> components(componentCount);

    // some files encode the buildable-component list directly as the second line of the file. ignore it.
    if (util::startsWith(lines[1], "true") || util::startsWith(lines[1], "false"))
        lineIndex++;

    for (int componentIndex = 0; componentIndex < componentCount; componentIndex++)
    {
        LegacyComponent &c = components[componentIndex];
        const string &line = lines[lineIndex++];
        auto words = util::split(line, '\t');
        MLIB_ASSERT_STR(convert::toInt(words[0]) == componentIndex, "invalid component index");

        c.legacyType = LegacyComponentType(convert::toInt(words[1]));
        c.location.boardPos = vec2i(convert::toInt(words[2]), convert::toInt(words[3]));
        c.location.circuitPos = vec2i(convert::toInt(words[4]), convert::toInt(words[5]));
        if (c.location.circuitPos.x == -999)
            c.location.circuitPos = constants::invalidCoord;
        else
            swap(c.location.boardPos, c.location.circuitPos);

        c.chargePreference = convert::toInt(words[6]);

        c.puzzleType = ComponentPuzzleType::PuzzlePiece;

        if (words.size() >= 16)
            c.puzzleType = (ComponentPuzzleType)convert::toInt(words[15]);

        for (int fieldIndex = 0; fieldIndex < LegacyComponent::fieldCount; fieldIndex++)
        {
            c.fieldValues[fieldIndex] = convert::toInt(words[7 + fieldIndex]);
        }

        if (c.legacyType == ComponentCircuit)
        {
            //
            // ignore the circuit layout, this information is already encoded in the location of all components.
            //
            lineIndex += constants::circuitBoardSize;
        }
    }

    //
    // the remaining 24 lines encode the board layout. This information is already encoded.
    //

    result.clearAndResetBoard();

    //
    // add all circuits
    //
    for (const LegacyComponent &c : components)
    {
        if (c.legacyType == ComponentCircuit)
        {
            auto info = getLegacyInfo(c.legacyType);
            Component *newC = new Component("Circuit", ChargeType::None, c.location);

            newC->modifiers.puzzleType = c.puzzleType;

            result.addNewComponent(newC, false, false);
        }
    }

    //
    // add all non-circuits
    //
    for (const LegacyComponent &c : components)
    {
        if (c.legacyType != ComponentCircuit)
        {
            auto info = getLegacyInfo(c.legacyType);
            Component *newC = new Component(info.name, info.color, c.location);
            newC->modifiers.chargePreference = c.chargePreference;
            newC->modifiers.boundary = info.boundary;
            newC->modifiers.puzzleType = c.puzzleType;
            newC->modifiers.speed = info.speed;

            /*
            Legacy field meanings:
            Power source
            _Fields[0] = 0; -> emission delay in seconds
            _Fields[1] = 1; -> secondsPerEmission
            _Fields[2] = 1000; -> charges remaining
            */

            if (info.name == "PowerSource")
            {
                newC->intrinsics.secondsBeforeFirstEmission = c.fieldValues[0];
                newC->intrinsics.secondsPerEmission = c.fieldValues[1];
                newC->intrinsics.totalCharges = c.fieldValues[2];
            }

            result.addNewComponent(newC, false, false);
        }
    }

    result.puzzleName = util::removeExtensions(util::fileNameFromPath(filename));

    //
    // normally this is done in addNewComponent, but this is actually noticably slower
    //
    result.updateAll();
    result.resetPuzzle();
}
