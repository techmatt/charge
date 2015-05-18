
#include "main.h"

bool Component::willAcceptCharge(GameState &state, const Charge &charge)
{
    int currentTimeDifference = state.stepCount - lastChargeVisit;
    int requiredTimeDifference = constants::chargeRequiredTimeDifference;

    if (currentTimeDifference < requiredTimeDifference)
    {
        return false;
    }

    return true;
}
