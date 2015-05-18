
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
    //
    // Move and update charges
    //

    for (Charge &c : charges)
        c.advance(*this);

    for (int chargeIndex = 0; chargeIndex < int(charges.size()); chargeIndex++)
    {
        Charge &charge = charges[chargeIndex];
        auto update = charge.interactWithDestination(*this);

        if (update.destroyCharge)
        {
            util::removeSwap(charges, chargeIndex);
            chargeIndex--;
        }
    }

    for (int chargeIndex = 0; chargeIndex < int(charges.size()); chargeIndex++)
    {
        Charge &charge = charges[chargeIndex];
        auto update = charge.updateDestination(*this);

        if (update.destroyCharge)
        {
            util::removeSwap(charges, chargeIndex);
            chargeIndex--;
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

Component* GameState::getComponent(const GameLocation &pos)
{
    if (pos.inCircuit())
    {
        return board.cells(pos.boardPos).c->circuitBoard->cells(pos.circuitPos).c;
    }
    else
    {
        return board.cells(pos.boardPos).c;
    }
}
