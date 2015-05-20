
#include "main.h"

void GameState::init()
{
    board.cells.allocate(params().boardDims.x, params().boardDims.y);
    resetPuzzle();
}

void GameState::resetPuzzle()
{
    stepCount = 0;
    charges.clear();
    explodingCharges.clear();
    for (Component *c : components)
        c->resetPuzzle();
}

void GameState::addNewComponent(Component *component)
{
    components.push_back(component);
    board.addNewComponent(component);
    board.updateBlockedGrid();
}

void GameState::removeComponent(Component *component)
{
    for (auto &cell : board.cells)
    {
        if (cell.value.c == component) cell.value.c = nullptr;
    }

    util::removeSwap(components, util::findFirstIndex(components, component));

    board.updateBlockedGrid();
}

void GameState::step()
{
    //
    // Advance exploding charges
    //

    for (int chargeIndex = 0; chargeIndex < int(explodingCharges.size()); chargeIndex++)
    {
        ExplodingCharge &charge = explodingCharges[chargeIndex];
        charge.advance();
        if (charge.done())
        {
            util::removeSwap(explodingCharges, chargeIndex);
            chargeIndex--;
        }
    }

    //
    // Move and update charges
    //

    for (Charge &c : charges)
        c.advance(*this);

    for (Charge &c : charges)
        c.interactWithDestination(*this);

    for (Charge &c : charges)
        c.updateDestination(*this);

    //
    // Remove dead charges
    //
    for (int chargeIndex = 0; chargeIndex < int(charges.size()); chargeIndex++)
    {
        Charge &charge = charges[chargeIndex];
        
        if (charge.markedForDeletion && charge.showDeathAnimation)
        {
            explodingCharges.push_back(ExplodingCharge(charge.source, charge.destination, (float)charge.timeInTransit / (float)charge.totalTransitTime, charge.level, constants::explodingChargeDuration, charge.rotationOffset(stepCount), stepCount));
        }

        if (charge.markedForDeletion)
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
        component->tick();

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
                charges.push_back(Charge(component->location, component->modifiers.color));
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
    if (pos == constants::invalidCoord) return nullptr;

    if (pos.inCircuit())
    {
        return board.cells(pos.boardPos).c->circuitBoard->cells(pos.circuitPos).c;
    }
    else
    {
        return board.cells(pos.boardPos).c;
    }
}
