
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
    // Upgrades from wire tower
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
    LegacyComponentInfo(const string _name, ChargeType _color = ChargeNone, WireSpeedType _speed = WireStandard)
    {
        name = _name;
        color = _color;
        speed = _speed;
        boundary = CircuitBoundaryInvalid;
    }
    LegacyComponentInfo(CircuitBoundaryType _boundary)
    {
        name = "CircuitBoundary";
        color = ChargeNone;
        speed = WireStandard;
        boundary = _boundary;
    }
    string name;
    ChargeType color;
    WireSpeedType speed;
    CircuitBoundaryType boundary;
};

LegacyComponentInfo getLegacyInfo(LegacyComponentType type)
{
    switch (type)
    {
    case ComponentPowerSourceRed: return LegacyComponentInfo("PowerSource", ChargeRed);
    case ComponentPowerSourceOrange: return LegacyComponentInfo("PowerSource", ChargeOrange);
    case ComponentPowerSourceYellow: return LegacyComponentInfo("PowerSource", ChargeYellow);
    case ComponentPowerSourceGreen: return LegacyComponentInfo("PowerSource", ChargeGreen);
    case ComponentPowerSourceBlue: return LegacyComponentInfo("PowerSource", ChargeBlue);

    case ComponentSplitter: return LegacyComponentInfo("Splitter");

    case ComponentAmplifier: return LegacyComponentInfo("Amplifier");
    case ComponentAmplifierL1: return LegacyComponentInfo("FilteredAmplifier", ChargeOrange);
    case ComponentAmplifierL2: return LegacyComponentInfo("FilteredAmplifier", ChargeYellow);
    case ComponentAmplifierL3: return LegacyComponentInfo("FilteredAmplifier", ChargeGreen);
    case ComponentAmplifierL4: return LegacyComponentInfo("FilteredAmplifier", ChargeBlue);

    case ComponentRedChargeGoal: return LegacyComponentInfo("ChargeGoal", ChargeRed);
    case ComponentOrangeChargeGoal: return LegacyComponentInfo("ChargeGoal", ChargeOrange);
    case ComponentYellowChargeGoal: return LegacyComponentInfo("ChargeGoal", ChargeYellow);
    case ComponentGreenChargeGoal: return LegacyComponentInfo("ChargeGoal", ChargeGreen);
    case ComponentBlueChargeGoal: return LegacyComponentInfo("ChargeGoal", ChargeBlue);

    case ComponentTeleportRedSource: return LegacyComponentInfo("TeleportSource", ChargeRed);
    case ComponentTeleportOrangeSource: return LegacyComponentInfo("TeleportSource", ChargeOrange);
    case ComponentTeleportYellowSource: return LegacyComponentInfo("TeleportSource", ChargeYellow);
    case ComponentTeleportGreenSource: return LegacyComponentInfo("TeleportSource", ChargeGreen);
    case ComponentTeleportBlueSource: return LegacyComponentInfo("TeleportSource", ChargeBlue);

    case ComponentTeleportRedDestination: return LegacyComponentInfo("TeleportDestination", ChargeRed);
    case ComponentTeleportOrangeDestination: return LegacyComponentInfo("TeleportDestination", ChargeOrange);
    case ComponentTeleportYellowDestination: return LegacyComponentInfo("TeleportDestination", ChargeYellow);
    case ComponentTeleportGreenDestination: return LegacyComponentInfo("TeleportDestination", ChargeGreen);
    case ComponentTeleportBlueDestination: return LegacyComponentInfo("TeleportDestination", ChargeBlue);

    case ComponentRedChargeFilter: return LegacyComponentInfo("ChargeFilter", ChargeRed);
    case ComponentOrangeChargeFilter: return LegacyComponentInfo("ChargeFilter", ChargeOrange);
    case ComponentYellowChargeFilter: return LegacyComponentInfo("ChargeFilter", ChargeYellow);
    case ComponentGreenChargeFilter: return LegacyComponentInfo("ChargeFilter", ChargeGreen);
    case ComponentBlueChargeFilter: return LegacyComponentInfo("ChargeFilter", ChargeBlue);

    case ComponentWireMajorDelay: return LegacyComponentInfo("Wire", ChargeNone, WireMajorDelay);
    case ComponentWireMinorDelay: return LegacyComponentInfo("Wire", ChargeNone, WireMinorDelay);
    case ComponentWire: return LegacyComponentInfo("Wire", ChargeNone, WireStandard);
    case ComponentWireMinorAccelerator: return LegacyComponentInfo("Wire", ChargeNone, WireMinorAccelerator);
    case ComponentWireMajorAccelerator: return LegacyComponentInfo("Wire", ChargeNone, WireMajorAccelerator);

    case ComponentBlocker: return LegacyComponentInfo("Blocker");

    //NOTE: I am not sure where exactly ComponentCircuitBoundaryOpen occurs; Open components seem to actually be represented as Base.
    case ComponentCircuitBoundaryOpen: return LegacyComponentInfo(CircuitBoundaryOpen);
    case ComponentCircuitBoundaryBase: return LegacyComponentInfo(CircuitBoundaryOpen);
    case ComponentCircuitBoundaryBlocked: return LegacyComponentInfo(CircuitBoundaryClosed);

    case ComponentHold: return LegacyComponentInfo("Hold");

    case ComponentGateOpenRed: return LegacyComponentInfo("GateOpen", ChargeRed);
    case ComponentGateOpenOrange: return LegacyComponentInfo("GateOpen", ChargeOrange);
    case ComponentGateOpenYellow: return LegacyComponentInfo("GateOpen", ChargeYellow);
    case ComponentGateOpenGreen: return LegacyComponentInfo("GateOpen", ChargeGreen);
    case ComponentGateOpenBlue: return LegacyComponentInfo("GateOpen", ChargeBlue);
    case ComponentGateOpenGray: return LegacyComponentInfo("GateOpen", ChargeGray);

    case ComponentGateClosedRed: return LegacyComponentInfo("GateClosed", ChargeRed);
    case ComponentGateClosedOrange: return LegacyComponentInfo("GateClosed", ChargeOrange);
    case ComponentGateClosedYellow: return LegacyComponentInfo("GateClosed", ChargeYellow);
    case ComponentGateClosedGreen: return LegacyComponentInfo("GateClosed", ChargeGreen);
    case ComponentGateClosedBlue: return LegacyComponentInfo("GateClosed", ChargeBlue);
    case ComponentGateClosedGray: return LegacyComponentInfo("GateClosed", ChargeGray);

    case ComponentGateSwitchRed: return LegacyComponentInfo("GateSwitch", ChargeRed);
    case ComponentGateSwitchOrange: return LegacyComponentInfo("GateSwitch", ChargeOrange);
    case ComponentGateSwitchYellow: return LegacyComponentInfo("GateSwitch", ChargeYellow);
    case ComponentGateSwitchGreen: return LegacyComponentInfo("GateSwitch", ChargeGreen);
    case ComponentGateSwitchBlue: return LegacyComponentInfo("GateSwitch", ChargeBlue);
    case ComponentGateSwitchGray: return LegacyComponentInfo("GateSwitch", ChargeGray);

    case ComponentMegaHoldRed: return LegacyComponentInfo("MegaHold", ChargeRed);
    case ComponentMegaHoldOrange: return LegacyComponentInfo("MegaHold", ChargeOrange);
    case ComponentMegaHoldYellow: return LegacyComponentInfo("MegaHold", ChargeYellow);
    case ComponentMegaHoldGreen: return LegacyComponentInfo("MegaHold", ChargeGreen);
    case ComponentMegaHoldBlue: return LegacyComponentInfo("MegaHold", ChargeBlue);
    case ComponentMegaHoldAll: return LegacyComponentInfo("MegaHold", ChargeGray);

    case ComponentTrapOpenRed: return LegacyComponentInfo("TrapOpen", ChargeRed);
    case ComponentTrapOpenOrange: return LegacyComponentInfo("TrapOpen", ChargeOrange);
    case ComponentTrapOpenYellow: return LegacyComponentInfo("TrapOpen", ChargeYellow);
    case ComponentTrapOpenGreen: return LegacyComponentInfo("TrapOpen", ChargeGreen);
    case ComponentTrapOpenBlue: return LegacyComponentInfo("TrapOpen", ChargeBlue);
    case ComponentTrapOpenGray: return LegacyComponentInfo("TrapOpen", ChargeGray);

    case ComponentTrapSprungRed: return LegacyComponentInfo("TrapSprung", ChargeRed);
    case ComponentTrapSprungOrange: return LegacyComponentInfo("TrapSprung", ChargeOrange);
    case ComponentTrapSprungYellow: return LegacyComponentInfo("TrapSprung", ChargeYellow);
    case ComponentTrapSprungGreen: return LegacyComponentInfo("TrapSprung", ChargeGreen);
    case ComponentTrapSprungBlue: return LegacyComponentInfo("TrapSprung", ChargeBlue);
    case ComponentTrapSprungGray: return LegacyComponentInfo("TrapSprung", ChargeGray);

    case ComponentTrapResetRed: return LegacyComponentInfo("TrapReset", ChargeRed);
    case ComponentTrapResetOrange: return LegacyComponentInfo("TrapReset", ChargeOrange);
    case ComponentTrapResetYellow: return LegacyComponentInfo("TrapReset", ChargeYellow);
    case ComponentTrapResetGreen: return LegacyComponentInfo("TrapReset", ChargeGreen);
    case ComponentTrapResetBlue: return LegacyComponentInfo("TrapReset", ChargeBlue);
    case ComponentTrapResetGray: return LegacyComponentInfo("TrapReset", ChargeGray);
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

        c.puzzleType = ComponentPuzzlePiece;

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

    result.clearBoard();

    //
    // add all circuits
    //
    for (const LegacyComponent &c : components)
    {
        if (c.legacyType == ComponentCircuit)
        {
            Component *newC = new Component("Circuit", ChargeNone, c.location);
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
            result.addNewComponent(newC, false, false);
        }
    }

    //
    // normally this is done in addNewComponent, but this is actually noticably slower
    //
    result.board.updateBlockedGrid();
    result.updateCircuitBoundaries();
    result.updateComponentConnections();
}
