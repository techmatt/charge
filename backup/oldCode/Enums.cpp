#include "Main.h"

String GetBuildingTypeString(BuildingType Type)
{
	switch(Type)
	{
		case BuildingPowerSourceRed:
			return "PowerSourceRed";
		case BuildingPowerSourceOrange:
			return "PowerSourceOrange";
		case BuildingPowerSourceYellow:
			return "PowerSourceYellow";
		case BuildingPowerSourceGreen:
			return "PowerSourceGreen";
		case BuildingPowerSourceBlue:
			return "PowerSourceBlue";
		case BuildingSplitter:
			return "Splitter";
		case BuildingAmplifier:
			return "Amplifier";
		case BuildingAmplifierL1:
			return "AmplifierL1";
		case BuildingAmplifierL2:
			return "AmplifierL2";
		case BuildingAmplifierL3:
			return "AmplifierL3";
		case BuildingAmplifierL4:
			return "AmplifierL4";
		case BuildingStreamer:
			return "Streamer";
		case BuildingRedChargeGoal:
			return "RedChargeGoal";
		case BuildingOrangeChargeGoal:
			return "OrangeChargeGoal";
		case BuildingYellowChargeGoal:
			return "YellowChargeGoal";
		case BuildingGreenChargeGoal:
			return "GreenChargeGoal";
		case BuildingBlueChargeGoal:
			return "BlueChargeGoal";
		case BuildingTeleportRedSource:
			return "TeleportRedSource";
		case BuildingTeleportOrangeSource:
			return "TeleportOrangeSource";
		case BuildingTeleportYellowSource:
			return "TeleportYellowSource";
		case BuildingTeleportGreenSource:
			return "TeleportGreenSource";
		case BuildingTeleportBlueSource:
			return "TeleportBlueSource";
		case BuildingRedChargeFilter:
			return "RedChargeFilter";
		case BuildingOrangeChargeFilter:
			return "OrangeChargeFilter";
		case BuildingYellowChargeFilter:
			return "YellowChargeFilter";
		case BuildingGreenChargeFilter:
			return "GreenChargeFilter";
		case BuildingBlueChargeFilter:
			return "BlueChargeFilter";
		case BuildingTeleportRedDestination:
			return "TeleportRedDestination";
		case BuildingTeleportOrangeDestination:
			return "TeleportOrangeDestination";
		case BuildingTeleportYellowDestination:
			return "TeleportYellowDestination";
		case BuildingTeleportGreenDestination:
			return "TeleportGreenDestination";
		case BuildingTeleportBlueDestination:
			return "TeleportBlueDestination";
		case BuildingCircuit:
			return "Circuit";
		case BuildingMultiplexer:
			return "Multiplexer";
		case BuildingHold:
			return "Hold";
		case BuildingWireMajorDelay:
			return "WireMajorDelay";
		case BuildingWireMinorDelay:
			return "WireMinorDelay";
		case BuildingWire:
			return "Wire";
		case BuildingWireMinorAccelerator:
			return "WireMinorAccelerator";
		case BuildingWireMajorAccelerator:
			return "WireMajorAccelerator";
		case BuildingBlocker:
			return "Blocker";
		case BuildingCircuitBoundaryOpen:
			return "CircuitBoundaryOpen";
		case BuildingCircuitBoundaryBase:
			return "CircuitBoundaryBase";
		case BuildingCircuitBoundaryBlocked:
			return "CircuitBoundaryBlocked";
		case BuildingCircuitRotate90:
			return "CircuitRotate90";
		case BuildingCircuitRotateN90:
			return "CircuitRotateN90";
		case BuildingCircuitFlipHorizontal:
			return "CircuitFlipHorizontal";
		case BuildingCircuitFlipVertical:
			return "CircuitFlipVertical";
		case BuildingCircuitCopy:
			return "CircuitCopy";
		case BuildingCircuitPaste:
			return "CircuitPaste";
		case BuildingGateOpenRed:
			return "GateOpenRed";
		case BuildingGateOpenOrange:
			return "GateOpenOrange";
		case BuildingGateOpenYellow:
			return "GateOpenYellow";
		case BuildingGateOpenGreen:
			return "GateOpenGreen";
		case BuildingGateOpenBlue:
			return "GateOpenBlue";
		case BuildingGateClosedRed:
			return "GateClosedRed";
		case BuildingGateClosedOrange:
			return "GateClosedOrange";
		case BuildingGateClosedYellow:
			return "GateClosedYellow";
		case BuildingGateClosedGreen:
			return "GateClosedGreen";
		case BuildingGateClosedBlue:
			return "GateClosedBlue";
		case BuildingGateSwitchRed:
			return "GateSwitchRed";
		case BuildingGateSwitchOrange:
			return "GateSwitchOrange";
		case BuildingGateSwitchYellow:
			return "GateSwitchYellow";
		case BuildingGateSwitchGreen:
			return "GateSwitchGreen";
		case BuildingGateSwitchBlue:
			return "GateSwitchBlue";
		case BuildingMusicBasic:
			return "MusicBasic";
		case BuildingMusicSharp:
			return "MusicSharp";
		case BuildingMusicFlat:
			return "MusicFlat";
		case BuildingMegaHoldRed:
			return "MegaHoldRed";
		case BuildingMegaHoldOrange:
			return "MegaHoldOrange";
		case BuildingMegaHoldYellow:
			return "MegaHoldYellow";
		case BuildingMegaHoldGreen:
			return "MegaHoldGreen";
		case BuildingMegaHoldBlue:
			return "MegaHoldBlue";
		case BuildingMegaHoldAll:
			return "MegaHoldAll";
		case BuildingTrapOpenGray:
			return "TrapOpenGray";
		case BuildingTrapSprungGray:
			return "TrapSprungGray";
		case BuildingGateOpenGray:
			return "GateOpenGray";
		case BuildingGateClosedGray:
			return "GateClosedGray";
		case BuildingGateSwitchGray:
			return "GateSwitchGray";
		case BuildingTrapResetGray:
			return "TrapResetGray";
		case BuildingTrapOpenRed:
			return "TrapOpenRed";
		case BuildingTrapOpenOrange:
			return "TrapOpenOrange";
		case BuildingTrapOpenYellow:
			return "TrapOpenYellow";
		case BuildingTrapOpenGreen:
			return "TrapOpenGreen";
		case BuildingTrapOpenBlue:
			return "TrapOpenBlue";
		case BuildingTrapSprungRed:
			return "TrapSprungRed";
		case BuildingTrapSprungOrange:
			return "TrapSprungOrange";
		case BuildingTrapSprungYellow:
			return "TrapSprungYellow";
		case BuildingTrapSprungGreen:
			return "TrapSprungGreen";
		case BuildingTrapSprungBlue:
			return "TrapSprungBlue";
		case BuildingTrapResetRed:
			return "TrapResetRed";
		case BuildingTrapResetOrange:
			return "TrapResetOrange";
		case BuildingTrapResetYellow:
			return "TrapResetYellow";
		case BuildingTrapResetGreen:
			return "TrapResetGreen";
		case BuildingTrapResetBlue:
			return "TrapResetBlue";
		case BuildingCount:
			return "Count";
	}
	return "Invalid BuildingType value";
}


BuildingType GetBuildingTypeFromString(const String &Str)
{
	if(Str == "BuildingPowerSourceRed")
	{
		return BuildingPowerSourceRed;
	}
	else if(Str == "BuildingPowerSourceOrange")
	{
		return BuildingPowerSourceOrange;
	}
	else if(Str == "BuildingPowerSourceYellow")
	{
		return BuildingPowerSourceYellow;
	}
	else if(Str == "BuildingPowerSourceGreen")
	{
		return BuildingPowerSourceGreen;
	}
	else if(Str == "BuildingPowerSourceBlue")
	{
		return BuildingPowerSourceBlue;
	}
	else if(Str == "BuildingSplitter")
	{
		return BuildingSplitter;
	}
	else if(Str == "BuildingAmplifier")
	{
		return BuildingAmplifier;
	}
	else if(Str == "BuildingAmplifierL1")
	{
		return BuildingAmplifierL1;
	}
	else if(Str == "BuildingAmplifierL2")
	{
		return BuildingAmplifierL2;
	}
	else if(Str == "BuildingAmplifierL3")
	{
		return BuildingAmplifierL3;
	}
	else if(Str == "BuildingAmplifierL4")
	{
		return BuildingAmplifierL4;
	}
	else if(Str == "BuildingStreamer")
	{
		return BuildingStreamer;
	}
	else if(Str == "BuildingRedChargeGoal")
	{
		return BuildingRedChargeGoal;
	}
	else if(Str == "BuildingOrangeChargeGoal")
	{
		return BuildingOrangeChargeGoal;
	}
	else if(Str == "BuildingYellowChargeGoal")
	{
		return BuildingYellowChargeGoal;
	}
	else if(Str == "BuildingGreenChargeGoal")
	{
		return BuildingGreenChargeGoal;
	}
	else if(Str == "BuildingBlueChargeGoal")
	{
		return BuildingBlueChargeGoal;
	}
	else if(Str == "BuildingTeleportRedSource")
	{
		return BuildingTeleportRedSource;
	}
	else if(Str == "BuildingTeleportOrangeSource")
	{
		return BuildingTeleportOrangeSource;
	}
	else if(Str == "BuildingTeleportYellowSource")
	{
		return BuildingTeleportYellowSource;
	}
	else if(Str == "BuildingTeleportGreenSource")
	{
		return BuildingTeleportGreenSource;
	}
	else if(Str == "BuildingTeleportBlueSource")
	{
		return BuildingTeleportBlueSource;
	}
	else if(Str == "BuildingRedChargeFilter")
	{
		return BuildingRedChargeFilter;
	}
	else if(Str == "BuildingOrangeChargeFilter")
	{
		return BuildingOrangeChargeFilter;
	}
	else if(Str == "BuildingYellowChargeFilter")
	{
		return BuildingYellowChargeFilter;
	}
	else if(Str == "BuildingGreenChargeFilter")
	{
		return BuildingGreenChargeFilter;
	}
	else if(Str == "BuildingBlueChargeFilter")
	{
		return BuildingBlueChargeFilter;
	}
	else if(Str == "BuildingTeleportRedDestination")
	{
		return BuildingTeleportRedDestination;
	}
	else if(Str == "BuildingTeleportOrangeDestination")
	{
		return BuildingTeleportOrangeDestination;
	}
	else if(Str == "BuildingTeleportYellowDestination")
	{
		return BuildingTeleportYellowDestination;
	}
	else if(Str == "BuildingTeleportGreenDestination")
	{
		return BuildingTeleportGreenDestination;
	}
	else if(Str == "BuildingTeleportBlueDestination")
	{
		return BuildingTeleportBlueDestination;
	}
	else if(Str == "BuildingCircuit")
	{
		return BuildingCircuit;
	}
	else if(Str == "BuildingMultiplexer")
	{
		return BuildingMultiplexer;
	}
	else if(Str == "BuildingHold")
	{
		return BuildingHold;
	}
	else if(Str == "BuildingWireMajorDelay")
	{
		return BuildingWireMajorDelay;
	}
	else if(Str == "BuildingWireMinorDelay")
	{
		return BuildingWireMinorDelay;
	}
	else if(Str == "BuildingWire")
	{
		return BuildingWire;
	}
	else if(Str == "BuildingWireMinorAccelerator")
	{
		return BuildingWireMinorAccelerator;
	}
	else if(Str == "BuildingWireMajorAccelerator")
	{
		return BuildingWireMajorAccelerator;
	}
	else if(Str == "BuildingBlocker")
	{
		return BuildingBlocker;
	}
	else if(Str == "BuildingCircuitBoundaryOpen")
	{
		return BuildingCircuitBoundaryOpen;
	}
	else if(Str == "BuildingCircuitBoundaryBase")
	{
		return BuildingCircuitBoundaryBase;
	}
	else if(Str == "BuildingCircuitBoundaryBlocked")
	{
		return BuildingCircuitBoundaryBlocked;
	}
	else if(Str == "BuildingCircuitRotate90")
	{
		return BuildingCircuitRotate90;
	}
	else if(Str == "BuildingCircuitRotateN90")
	{
		return BuildingCircuitRotateN90;
	}
	else if(Str == "BuildingCircuitFlipHorizontal")
	{
		return BuildingCircuitFlipHorizontal;
	}
	else if(Str == "BuildingCircuitFlipVertical")
	{
		return BuildingCircuitFlipVertical;
	}
	else if(Str == "BuildingCircuitCopy")
	{
		return BuildingCircuitCopy;
	}
	else if(Str == "BuildingCircuitPaste")
	{
		return BuildingCircuitPaste;
	}
	else if(Str == "BuildingGateOpenRed")
	{
		return BuildingGateOpenRed;
	}
	else if(Str == "BuildingGateOpenOrange")
	{
		return BuildingGateOpenOrange;
	}
	else if(Str == "BuildingGateOpenYellow")
	{
		return BuildingGateOpenYellow;
	}
	else if(Str == "BuildingGateOpenGreen")
	{
		return BuildingGateOpenGreen;
	}
	else if(Str == "BuildingGateOpenBlue")
	{
		return BuildingGateOpenBlue;
	}
	else if(Str == "BuildingGateClosedRed")
	{
		return BuildingGateClosedRed;
	}
	else if(Str == "BuildingGateClosedOrange")
	{
		return BuildingGateClosedOrange;
	}
	else if(Str == "BuildingGateClosedYellow")
	{
		return BuildingGateClosedYellow;
	}
	else if(Str == "BuildingGateClosedGreen")
	{
		return BuildingGateClosedGreen;
	}
	else if(Str == "BuildingGateClosedBlue")
	{
		return BuildingGateClosedBlue;
	}
	else if(Str == "BuildingGateSwitchRed")
	{
		return BuildingGateSwitchRed;
	}
	else if(Str == "BuildingGateSwitchOrange")
	{
		return BuildingGateSwitchOrange;
	}
	else if(Str == "BuildingGateSwitchYellow")
	{
		return BuildingGateSwitchYellow;
	}
	else if(Str == "BuildingGateSwitchGreen")
	{
		return BuildingGateSwitchGreen;
	}
	else if(Str == "BuildingGateSwitchBlue")
	{
		return BuildingGateSwitchBlue;
	}
	else if(Str == "BuildingMusicBasic")
	{
		return BuildingMusicBasic;
	}
	else if(Str == "BuildingMusicSharp")
	{
		return BuildingMusicSharp;
	}
	else if(Str == "BuildingMusicFlat")
	{
		return BuildingMusicFlat;
	}
	else if(Str == "BuildingMegaHoldRed")
	{
		return BuildingMegaHoldRed;
	}
	else if(Str == "BuildingMegaHoldOrange")
	{
		return BuildingMegaHoldOrange;
	}
	else if(Str == "BuildingMegaHoldYellow")
	{
		return BuildingMegaHoldYellow;
	}
	else if(Str == "BuildingMegaHoldGreen")
	{
		return BuildingMegaHoldGreen;
	}
	else if(Str == "BuildingMegaHoldBlue")
	{
		return BuildingMegaHoldBlue;
	}
	else if(Str == "BuildingMegaHoldAll")
	{
		return BuildingMegaHoldAll;
	}
	else if(Str == "BuildingTrapOpenGray")
	{
		return BuildingTrapOpenGray;
	}
	else if(Str == "BuildingTrapSprungGray")
	{
		return BuildingTrapSprungGray;
	}
	else if(Str == "BuildingGateOpenGray")
	{
		return BuildingGateOpenGray;
	}
	else if(Str == "BuildingGateClosedGray")
	{
		return BuildingGateClosedGray;
	}
	else if(Str == "BuildingGateSwitchGray")
	{
		return BuildingGateSwitchGray;
	}
	else if(Str == "BuildingTrapResetGray")
	{
		return BuildingTrapResetGray;
	}
	else if(Str == "BuildingTrapOpenRed")
	{
		return BuildingTrapOpenRed;
	}
	else if(Str == "BuildingTrapOpenOrange")
	{
		return BuildingTrapOpenOrange;
	}
	else if(Str == "BuildingTrapOpenYellow")
	{
		return BuildingTrapOpenYellow;
	}
	else if(Str == "BuildingTrapOpenGreen")
	{
		return BuildingTrapOpenGreen;
	}
	else if(Str == "BuildingTrapOpenBlue")
	{
		return BuildingTrapOpenBlue;
	}
	else if(Str == "BuildingTrapSprungRed")
	{
		return BuildingTrapSprungRed;
	}
	else if(Str == "BuildingTrapSprungOrange")
	{
		return BuildingTrapSprungOrange;
	}
	else if(Str == "BuildingTrapSprungYellow")
	{
		return BuildingTrapSprungYellow;
	}
	else if(Str == "BuildingTrapSprungGreen")
	{
		return BuildingTrapSprungGreen;
	}
	else if(Str == "BuildingTrapSprungBlue")
	{
		return BuildingTrapSprungBlue;
	}
	else if(Str == "BuildingTrapResetRed")
	{
		return BuildingTrapResetRed;
	}
	else if(Str == "BuildingTrapResetOrange")
	{
		return BuildingTrapResetOrange;
	}
	else if(Str == "BuildingTrapResetYellow")
	{
		return BuildingTrapResetYellow;
	}
	else if(Str == "BuildingTrapResetGreen")
	{
		return BuildingTrapResetGreen;
	}
	else if(Str == "BuildingTrapResetBlue")
	{
		return BuildingTrapResetBlue;
	}
	else if(Str == "BuildingCount")
	{
		return BuildingCount;
	}
	else
	{
		SignalError(String("Invalid BuildingType: ") + Str);
		return BuildingType(0);
	}
}


