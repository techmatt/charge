
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

    static const float chargeScaleWithLevelFactor = 0.25f;
    static const float stepsPerSecond = 60.0f;
    static const float chargeRotationsPerSecond = 1.0f;

    static const float canonicalChargeSize = 10.0f;
}