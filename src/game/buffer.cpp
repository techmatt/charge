#include "main.h"


rect2i ComponentSet::boundingBox()
{
	if (components.size() == 0)
		return rect2i(constants::invalidCoord, constants::invalidCoord);

	rect2i out = rect2i(params().boardDims[0], params().boardDims[1], 0, 0);

	for (auto &c : components)
	{
		out.setMinX( min(out.min().x, c->location.boardPos.x) );
		out.setMinY ( min(out.min().y, c->location.boardPos.y) );

		out.setMaxX( max(out.max().x, c->location.boardPos.x) );
		out.setMaxY( max(out.max().y, c->location.boardPos.y) );
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
		for (ComponentDefiningProperties* c : components)
		{
			temp = c->location.boardPos + vec2i(1, 1) - vec2i(center);
			x = temp.x; y = temp.y;
			temp.y = -x; temp.x = y;
			c->location.boardPos = temp - vec2i(1, 1) + vec2i(center);

			if (c->location.inCircuit())
			{
				temp = c->location.circuitPos + vec2i(1, 1) - vec2i(constants::circuitBoardSize / 2 - 1, constants::circuitBoardSize / 2 - 1);
				x = temp.x; y = temp.y;
				temp.y = -x; temp.x = y;
				c->location.circuitPos = temp - vec2i(1, 1) + vec2i(center);
			}
		}
	else if (mod4==2)
		for (ComponentDefiningProperties *c : components)
		{
			temp = c->location.boardPos + vec2i(1, 1) - vec2i(center);
			x = temp.x; y = temp.y;
			temp.y = -y; temp.x = -x;
			c->location.boardPos = temp - vec2i(1, 1) + vec2i(center);

			if (c->location.inCircuit())
			{
				temp = c->location.circuitPos + vec2i(1, 1) - vec2i(constants::circuitBoardSize / 2 - 1, constants::circuitBoardSize / 2 - 1);
				x = temp.x; y = temp.y;
				temp.y = -y; temp.x = -x;
				c->location.circuitPos = temp - vec2i(1, 1) + vec2i(center);
			}
		}
	else if (mod4 == 3)
		for (ComponentDefiningProperties* c : components)
		{
			temp = c->location.boardPos + vec2i(1, 1) - vec2i(center);
			x = temp.x; y = temp.y;
			temp.y = x; temp.x = -y;
			c->location.boardPos = temp - vec2i(1, 1) + vec2i(center);

			if (c->location.inCircuit())
			{
				temp = c->location.circuitPos + vec2i(1, 1) - vec2i(constants::circuitBoardSize / 2 - 1, constants::circuitBoardSize / 2 - 1);
				x = temp.x; y = temp.y;
				temp.y = x; temp.x = -y;
				c->location.circuitPos = temp - vec2i(1, 1) + vec2i(center);
			}
		}
}

void ComponentSet::flipAboutHorizonal()
{
	rect2i bound = this->boundingBox();
	vec2i center = bound.center();

	vec2i temp; int x; int y;

	for (ComponentDefiningProperties* c : components)
	{
		temp = c->location.boardPos + vec2i(1, 1) - vec2i(center);
		x = temp.x; y = temp.y;
		temp.y = -y; temp.x = x;
		c->location.boardPos = temp - vec2i(1, 1) + vec2i(center);

		if (c->location.inCircuit())
		{
			temp = c->location.circuitPos + vec2i(1, 1) - vec2i(constants::circuitBoardSize / 2 - 1, constants::circuitBoardSize / 2 - 1);
			x = temp.x; y = temp.y;
			temp.y = -y; temp.x = x;
			c->location.circuitPos = temp - vec2i(1, 1) + vec2i(center);
		}
	}
}

void ComponentSet::flipAboutVerical()
{
	rect2i bound = this->boundingBox();
	vec2i center = bound.center();

	vec2i temp; int x; int y;

	for (ComponentDefiningProperties* c : components)
	{
		temp = c->location.boardPos + vec2i(1, 1) - vec2i(center);
		x = temp.x; y = temp.y;
		temp.y = y; temp.x = -x;
		c->location.boardPos = temp - vec2i(1, 1) + vec2i(center);

		if (c->location.inCircuit())
		{
			temp = c->location.circuitPos + vec2i(1, 1) - vec2i(constants::circuitBoardSize / 2 - 1, constants::circuitBoardSize / 2 - 1);
			x = temp.x; y = temp.y;
			temp.y = y; temp.x = -x;
			c->location.circuitPos = temp - vec2i(1, 1) + vec2i(center);
		}
	}
}

void ComponentSet::addToComponents(GameState &state, vec2i offset)
{
	// add the components in the list 1-by-1 to the game state
	// do the elements not in circuits first.
	for (ComponentDefiningProperties* c : components)
	{
		if (c->location.inCircuit()) continue;
		Component* newComponent = c->toComponent();
		newComponent->location.boardPos += offset;
		state.addNewComponent(newComponent, false, false);
	}

	// do the elements in circuits next.
	for (ComponentDefiningProperties* c : components)
	{
		if (!c->location.inCircuit()) continue;
		Component* newComponent = c->toComponent();
		newComponent->location.boardPos += offset;
		state.addNewComponent(newComponent, false, false);
	}
	state.updateAll();
}

void ComponentSet::addToCircuit(GameState &state, vec2i posOfCircuit, vec2i offset)
{
	// add the components in the list 1-by-1 to the game state
	// do the elements not in circuits first.
	for (ComponentDefiningProperties* c : components)
	{
		if (c->location.inCircuit()) errorFunctionMLIB("Attempted to add circuit to circuit when pasting");
		Component* newComponent = c->toComponent();
		newComponent->location.circuitPos = c->location.boardPos+offset;
		newComponent->location.boardPos = posOfCircuit;
		state.addNewComponent(newComponent, false, false);
	}
	state.updateAll();
}

ComponentSet* ComponentSet::allToBuffer(const GameState &state)
{
	// takes all the components on the board and stores them in a vector
	//int length = state.components.size();
	ComponentSet* out = new ComponentSet();

	for (Component* c : state.components)
	{
		ComponentDefiningProperties* cProp = new ComponentDefiningProperties(c);
		out->components.push_back(cProp); // I think that this will successfully reserve the data.
	}
	
	return out;
}