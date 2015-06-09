#include "main.h"


void ComponentSelection::copyToComponentSet(ComponentSet* cset, GameState* state) // copies a selection buffer to an already allocated componentset
{
	//empty the set
	cset->clear();


	//remember where all the circuits are
	set<vec2i> circuitLocations = {};

	// add the components in the selection
	for (Component* c : components)
	{
		ComponentDefiningProperties cProp(*c);
		cset->components.push_back(cProp);
		if (c->circuitBoard != nullptr)
			circuitLocations.insert(c->location.boardPos);
	}

	// add the components in circuit boards in the selection
	for (Component *c : state->components)
	{
		bool inList = circuitLocations.find(c->location.boardPos) != circuitLocations.end();
		if (c->location.inCircuit() && inList)
		{
			// the circuit here is in the selection set.
			ComponentDefiningProperties cProp(*c);
			cset->components.push_back(cProp);
		}
	}

	// make everything a board component
	if (selectionIsInCircuit)
		cset->takeCircuitToBoard();
}

