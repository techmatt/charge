enum BuildingType
{
	BuildingPowerSourceRed,
	BuildingPowerSourceOrange,
	BuildingPowerSourceYellow,
	BuildingPowerSourceGreen,
	BuildingPowerSourceBlue,

	BuildingSplitter,
	BuildingAmplifier,
	BuildingAmplifierL1,
	BuildingAmplifierL2,
	BuildingAmplifierL3,
	BuildingAmplifierL4,
	BuildingStreamer,
	
	BuildingRedChargeGoal,
	BuildingOrangeChargeGoal,
	BuildingYellowChargeGoal,
	BuildingGreenChargeGoal,
	BuildingBlueChargeGoal,

	BuildingTeleportRedSource,
	BuildingTeleportOrangeSource,
	BuildingTeleportYellowSource,
	BuildingTeleportGreenSource,
	BuildingTeleportBlueSource,

	BuildingRedChargeFilter,
	BuildingOrangeChargeFilter,
	BuildingYellowChargeFilter,
	BuildingGreenChargeFilter,
	BuildingBlueChargeFilter,

	BuildingTeleportRedDestination,
	BuildingTeleportOrangeDestination,
	BuildingTeleportYellowDestination,
	BuildingTeleportGreenDestination,
	BuildingTeleportBlueDestination,
	
	BuildingCircuit,
	BuildingMultiplexer,
	BuildingHold,

	//
	// Upgrades from wire tower
	//
	BuildingWireMajorDelay,
	BuildingWireMinorDelay,
	BuildingWire,
	BuildingWireMinorAccelerator,
	BuildingWireMajorAccelerator,

	BuildingBlocker,

	BuildingCircuitBoundaryOpen,
	BuildingCircuitBoundaryBase,
	BuildingCircuitBoundaryBlocked,

	BuildingCircuitRotate90,
	BuildingCircuitRotateN90,
	BuildingCircuitFlipHorizontal,
	BuildingCircuitFlipVertical,
	BuildingCircuitCopy,
	BuildingCircuitPaste,

	BuildingGateOpenRed,
	BuildingGateOpenOrange,
	BuildingGateOpenYellow,
	BuildingGateOpenGreen,
	BuildingGateOpenBlue,

	BuildingGateClosedRed,
	BuildingGateClosedOrange,
	BuildingGateClosedYellow,
	BuildingGateClosedGreen,
	BuildingGateClosedBlue,

	BuildingGateSwitchRed,
	BuildingGateSwitchOrange,
	BuildingGateSwitchYellow,
	BuildingGateSwitchGreen,
	BuildingGateSwitchBlue,

	BuildingMusicBasic,
	BuildingMusicSharp,
	BuildingMusicFlat,

	BuildingMegaHoldRed,
	BuildingMegaHoldOrange,
	BuildingMegaHoldYellow,
	BuildingMegaHoldGreen,
	BuildingMegaHoldBlue,

	BuildingMegaHoldAll,

	BuildingTrapOpenGray,
	BuildingTrapSprungGray,

	BuildingGateOpenGray,
	BuildingGateClosedGray,
	BuildingGateSwitchGray,

	BuildingTrapOpenRed,
	BuildingTrapOpenOrange,
	BuildingTrapOpenYellow,
	BuildingTrapOpenGreen,
	BuildingTrapOpenBlue,

	BuildingTrapSprungRed,
	BuildingTrapSprungOrange,
	BuildingTrapSprungYellow,
	BuildingTrapSprungGreen,
	BuildingTrapSprungBlue,

	BuildingTrapResetRed,
	BuildingTrapResetOrange,
	BuildingTrapResetYellow,
	BuildingTrapResetGreen,
	BuildingTrapResetBlue,
	BuildingTrapResetGray,

	BuildingCount,
};

String GetBuildingTypeString(BuildingType Type);
BuildingType GetBuildingTypeFromString(const String &Str);
