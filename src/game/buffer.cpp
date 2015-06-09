#include "main.h"


rect2i ComponentSet::boundingBox() const
{
    if (components.size() == 0)
        return rect2i(constants::invalidCoord, constants::invalidCoord);

    rect2i out = rect2i(params().boardDims[0], params().boardDims[1], 0, 0);

    for (auto &c : components)
    {
        out.include(c.location.boardPos);
        out.include(c.location.boardPos + vec2i(2, 2));
    }

    return out;
}

// rotates a buffer by 90i degrees clockwise
void ComponentSet::rotate(int i)
{
    int mod4 = i % 4;
    if (mod4 == 0) return;

    rect2i bound = this->boundingBox();
    vec2i center = bound.center();

    vec2i temp; int x; int y;

    // just go through all the components and change the c.location.boardPos and c.location.circuitPos
    if (mod4 == 1)
        for (ComponentDefiningProperties& c : components)
        {
            temp = c.location.boardPos + vec2i(1, 1) - vec2i(center);
            x = temp.x; y = temp.y;
            temp.y = -x; temp.x = y;
            c.location.boardPos = temp - vec2i(1, 1) + vec2i(center);

            if (c.location.inCircuit())
            {
                temp = c.location.circuitPos + vec2i(1, 1) - vec2i(constants::circuitBoardSize / 2 - 1, constants::circuitBoardSize / 2 - 1);
                x = temp.x; y = temp.y;
                temp.y = -x; temp.x = y;
                c.location.circuitPos = temp - vec2i(1, 1) + vec2i(center);
            }
        }
    else if (mod4 == 2)
        for (ComponentDefiningProperties &c : components)
        {
            temp = c.location.boardPos + vec2i(1, 1) - vec2i(center);
            x = temp.x; y = temp.y;
            temp.y = -y; temp.x = -x;
            c.location.boardPos = temp - vec2i(1, 1) + vec2i(center);

            if (c.location.inCircuit())
            {
                temp = c.location.circuitPos + vec2i(1, 1) - vec2i(constants::circuitBoardSize / 2 - 1, constants::circuitBoardSize / 2 - 1);
                x = temp.x; y = temp.y;
                temp.y = -y; temp.x = -x;
                c.location.circuitPos = temp - vec2i(1, 1) + vec2i(center);
            }
        }
    else if (mod4 == 3)
        for (ComponentDefiningProperties& c : components)
        {
            temp = c.location.boardPos + vec2i(1, 1) - vec2i(center);
            x = temp.x; y = temp.y;
            temp.y = x; temp.x = -y;
            c.location.boardPos = temp - vec2i(1, 1) + vec2i(center);

            if (c.location.inCircuit())
            {
                temp = c.location.circuitPos + vec2i(1, 1) - vec2i(constants::circuitBoardSize / 2 - 1, constants::circuitBoardSize / 2 - 1);
                x = temp.x; y = temp.y;
                temp.y = x; temp.x = -y;
                c.location.circuitPos = temp - vec2i(1, 1) + vec2i(center);
            }
        }
}

void ComponentSet::flipAboutHorizonal()
{
    rect2i bound = this->boundingBox();
    vec2i center = bound.center();

    vec2i temp; int x; int y;

    for (ComponentDefiningProperties &c : components)
    {
        temp = c.location.boardPos + vec2i(1, 1) - vec2i(center);
        x = temp.x; y = temp.y;
        temp.y = -y; temp.x = x;
        c.location.boardPos = temp - vec2i(1, 1) + vec2i(center);

        if (c.location.inCircuit())
        {
            temp = c.location.circuitPos + vec2i(1, 1) - vec2i(constants::circuitBoardSize / 2 - 1, constants::circuitBoardSize / 2 - 1);
            x = temp.x; y = temp.y;
            temp.y = -y; temp.x = x;
            c.location.circuitPos = temp - vec2i(1, 1) + vec2i(center);
        }
    }
}

void ComponentSet::flipAboutVerical()
{
    rect2i bound = this->boundingBox();
    vec2i center = bound.center();

    vec2i temp; int x; int y;

    for (ComponentDefiningProperties &c : components)
    {
        temp = c.location.boardPos + vec2i(1, 1) - vec2i(center);
        x = temp.x; y = temp.y;
        temp.y = y; temp.x = -x;
        c.location.boardPos = temp - vec2i(1, 1) + vec2i(center);

        if (c.location.inCircuit())
        {
            temp = c.location.circuitPos + vec2i(1, 1) - vec2i(constants::circuitBoardSize / 2 - 1, constants::circuitBoardSize / 2 - 1);
            x = temp.x; y = temp.y;
            temp.y = y; temp.x = -x;
            c.location.circuitPos = temp - vec2i(1, 1) + vec2i(center);
        }
    }
}

void ComponentSet::addToComponents(GameState &state, vec2i offset)
{
    // add the components in the list 1-by-1 to the game state
    // do the elements not in circuits first.
    for (ComponentDefiningProperties &c : components)
    {
        if (c.location.inCircuit()) continue;
        Component* newComponent = c.makeNewComponent();
        newComponent->location.boardPos += offset;
        state.addNewComponent(newComponent);
    }

    // do the elements in circuits next.
    for (ComponentDefiningProperties &c : components)
    {
        if (!c.location.inCircuit()) continue;
        Component* newComponent = c.makeNewComponent();
        newComponent->location.boardPos += offset;
        state.addNewComponent(newComponent);
    }
    state.updateAll();
}

void ComponentSet::addToCircuit(GameState &state, vec2i posOfCircuit, vec2i offset)
{
    // add the components in the list 1-by-1 to the game state
    // do the elements not in circuits first.
    for (ComponentDefiningProperties &c : components)
    {
        if (c.location.inCircuit()) errorFunctionMLIB("Attempted to add circuit to circuit when pasting");
        Component* newComponent = c.makeNewComponent();
        newComponent->location.circuitPos = c.location.boardPos + offset;
        newComponent->location.boardPos = posOfCircuit;
        state.addNewComponent(newComponent, false, false);
    }
    state.updateAll();
}

ComponentSet ComponentSet::allToBuffer(const GameState &state)
{
    return ComponentSet::toBuffer(state.components);
}

ComponentSet ComponentSet::toBuffer(const vector<Component*> &components)
{
    ComponentSet result;
    for (Component* c : components)
    {
        ComponentDefiningProperties cProp(*c);
        result.components.push_back(cProp);
    }
    return result;
}

void ComponentSet::takeCircuitToBoard()
{
    // forget that the component set isn't in the circuit;
    for (ComponentDefiningProperties c : components)
    {
        MLIB_ASSERT_STR(c.location.inCircuit(), "Component is not already in a circuit"); //TODO Check that this isn't a circuit-boundary part
        c.location = GameLocation(c.location.circuitPos);
    }
}

void ComponentSet::takeBoardToCircuit(vec2i boardLocation)
{
    // forget that the component set isn't in the circuit;
    for (ComponentDefiningProperties c : components)
    {
        MLIB_ASSERT_STR(!c.location.inCircuit(), "Component is already in a circuit"); //TODO Check that this isn't a circuit-boundary part
        c.location = GameLocation(boardLocation, c.location.boardPos);
    }
}