
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

    static const float chargeScaleWithLevelFactor = 0.2f;
    static const float stepsPerSecond = 60.0f;
    static const float chargeRotationsPerSecond = 1.0f;
    static const float explodingChargeRotationFactor = 2.0f;

    static const float canonicalChargeSize = 7.0f;

    static const int deathTrapDuration = chargeRequiredTimeDifference + 2;

    static const int explodingChargeDuration = 30;

    static const int circuitBoardSize = 14;

    static const vec2f connectorDims[3] = { vec2f(0.18f, 0.05f), vec2f(0.2f, 0.056f), vec2f(0.18f, 0.05f) };

}
