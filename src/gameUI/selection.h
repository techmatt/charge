
struct ComponentSelection {
	// just consists of a vector of the selected components.  This should work without any intermediate steps because we're very limited in the number of things we can select.
	vector<Component*> components;
	vec2i circuitLocation;
	bool selectionIsInCircuit;
	//Grid2<BoardCell> cells;

	// check whether the selection is in the circuit
	// bool selectionIsInCircuit() { return (cells.dimX() == constants::circuitBoardSize) }

	void empty() {
		components.clear();
		circuitLocation = constants::invalidCoord;
		selectionIsInCircuit = false;
	}

	// creates a new selection set
	void newSelectionFromComponent(Component *c) {
		components.clear();
		selectionIsInCircuit = c->location.inCircuit();
		if (selectionIsInCircuit)
		{
			circuitLocation = c->location.boardPos;
			selectionIsInCircuit = true;
		}
		else
		{
			circuitLocation = constants::invalidCoord;
			selectionIsInCircuit = false;
		}

		components.push_back(c);
	}

	// adds an element to a selection set
	void add(Component* c)
	{
		if (selectionIsInCircuit != c->location.inCircuit())
			newSelectionFromComponent(c);
		else if (selectionIsInCircuit && circuitLocation != c->location.boardPos)
			newSelectionFromComponent(c);
		else
			components.push_back(c);
	}

	// Checks whether the component is already in the selection buffer
	// this should actually be pretty fast because the selection has at most 144 pointers in it.
	bool isIn(const Component* c)
	{
		for (int i = 0; i < components.size(); i++)
			if (components[i] == c)
			{
				return true;
			}
		return false;
	}

	// searches through the selection buffer and removes a single component
	void remove(Component* c)
	{
		for (int i = 0; i < components.size(); i++)
			if (components[i] == c)
			{
				components.erase(components.begin() + i);
				return;
			}
	}

	//adds or removes a component at a given location depending on whether it's in the buffer already
	void toggle(Component* c)
	{
		if (isIn(c)) remove(c);
		else add(c);
	}

	// returns nullptr if the selection does not have exactly one element.  Otherwise returns the component
	Component* singleElement()
	{
		if (components.size() == 1) return components[0];
		return nullptr;
	}

	// also returns the circuit that's selected
	Component* singleElementOrCircuit(GameState *state)
	{
		if (selectionIsInCircuit)
			return state->getComponent(GameLocation(circuitLocation));
		
		if (components.size() == 1) return components[0];
		return nullptr;
	}

};