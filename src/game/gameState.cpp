
#include "main.h"

void GameState::init()
{
    stepCount = 0;
	board.cells.allocate(params().boardDims.x, params().boardDims.y);
}

void GameState::addNewComponent(Component *component)
{
    components.push_back(component);
    board.addNewComponent(component);
}

void GameState::step()
{
    cout << "step: " << stepCount << endl;
    //
    // Move charges
    //
    for (int PassIndex = 0; PassIndex < 2; PassIndex++)
    {
        for (int chargeIndex = 0; chargeIndex < int(charges.size()); chargeIndex++)
        {
            Charge &charge = charges[chargeIndex];
            auto update = charge.update(*this, PassIndex == 1);
            if (update.destroyCharge)
            {
                util::removeSwap(charges, chargeIndex);
                chargeIndex--;
            }
        }
    }

    //
    // Update buildings
    //
    for (const auto &component : components)
    {
        //
        // Power sources produce new charges
        //
        if (component->info->name == "PowerSource"
            && component->totalChargesRemaining > 0)
        {
            if (component->timeUntilEmission > 0)
            {
                component->timeUntilEmission--;
            }
            else
            {
                charges.push_back(Charge(component->location, component->color));
                component->lastChargeVisit = stepCount;

                //TODO: parameterize this better
                component->timeUntilEmission = 60;
                component->totalChargesRemaining--;
            }
        }
    }

    stepCount++;
}
