
//
// most constants should go in parameters.txt and parameters.h
// this file is for static game constants that are never going to change
//

namespace constants
{
    static const vec2i invalidCoord(-100, -100);

    static const int chargeRequiredTimeDifference = 11;
    static const int chargeTransitTime = 9;
    static const int maxChargeLevel = 5;
    static const int maxPreferenceLevel = 5;

    static const float chargeScaleWithLevelFactor = 0.07f;
    static const int stepsPerSecond = 60;
    static const float secondsPerStep = 1.0f / (float)stepsPerSecond;
    static const float chargeRotationsPerSecond = 0.5f;
    static const float explodingChargeRotationFactor = 2.0f;

    static const float canonicalChargeSize = 6.5f;

    static const float trailExpansionFactor = 1.2f;

    static const int deathTrapDuration = chargeRequiredTimeDifference + 2;

    static const int explodingChargeDuration = 30;

    static const int gameBoardSize = 24;
    static const int circuitBoardSize = 14;

    static const bool useScanlinePriority = false;

    static const vec2f connectorDims[3] = { vec2f(0.18f, 0.05f), vec2f(0.2f, 0.056f), vec2f(0.18f, 0.05f) };

    static const char *fileExtension = ".pzl";


	// A slightly different approach to forming connections.
	static const vector<vec2i> nearbyComponents = {
		vec2i(0, -2),
		vec2i(1, -2),
		vec2i(2, -1),
		vec2i(2, 0),
		vec2i(2, 1),
		vec2i(1, 2),
		vec2i(0, 2),
		vec2i(-1, 2),
		vec2i(-2, 1),
		vec2i(-2, 0),
		vec2i(-2, -1),
		vec2i(-1, -2)
	};  // relative positions for nearby components.  There are 12 different places they can be.

	static const int bufferSize = 100;

	static float dragThreshold = 3.0f;
}
