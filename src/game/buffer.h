
//
// The minimal set of information needed to define a component.  This probably overlaps with some of the code for saving and loading the circuit.
//
struct ComponentDefiningProperties
{
	ComponentDefiningProperties(const Component &c)
	{
		baseInfo = c.baseInfo;
		location = c.location;
        modifiers = c.modifiers;
        modifiers.storedChargeColor = GameUtil::chargeColor(ChargeType::Gray);
        
        if (modifiers.puzzleType == ComponentPuzzleType::PuzzlePiece && c.info->name == "Circuit")
            modifiers.puzzleType = ComponentPuzzleType::CopiedCircuit;

		intrinsics = c.intrinsics;
	}
	const ComponentInfo* baseInfo;

	GameLocation location;

    ComponentModifiers modifiers;
    ComponentIntrinsics intrinsics;

	Component* makeNewComponent()
	{
		Component* result = new Component(baseInfo->name, modifiers.color, location);

        result->modifiers = modifiers;
		result->intrinsics = intrinsics;

		return result;
	}
};

struct ComponentSet
{
	ComponentSet() { }
	ComponentSet(const ComponentInfo* info, const ChargeType color)
	{
		Component temp(info->name, color, GameLocation(vec2i(0,0)));
		components.push_back(ComponentDefiningProperties(temp));
	}
	
	// properties
	vector<ComponentDefiningProperties> components;

	void flipAboutHorizonal();
	void flipAboutVerical();
	void rotate(int i); //rotates the buffer about the centerish by i*90 degrees clockwise
	rect2i boundingBox() const;

	//functions
	void addToComponents(GameState &state, vec2i offset);
	void addToCircuit(GameState &state, vec2i posOfCircuit, vec2i offset);

	// create new component sets
	static ComponentSet allToBuffer(const GameState &state);
	static ComponentSet toBuffer(const vector<Component*> &components);

	bool isEmpty() { return components.size() == 0; }
    void clear() { components.clear(); }

	void takeCircuitToBoard();
	void takeBoardToCircuit(vec2i boardPosition);

};

struct UndoBuffer
{
	void init() {
		int current=0;
		int maxForward = 0;
		int maxBackward = 0;
		for (int i = 0; i < constants::bufferSize; i++)
			buffer[i] = nullptr;

	}

    // TODO: this would probably be easier to just use ComponentSet instead of ComponentSet*
	ComponentSet* buffer[constants::bufferSize];

	int current;
	int maxForward;
	int maxBackward;

	void save(GameState &state)
	{
		current = (current + 1) % constants::bufferSize;
		if (current == maxBackward) maxBackward = (maxBackward + 1) % constants::bufferSize;
		maxForward = current;

		if (buffer[current]!=nullptr) delete(buffer[current]);

		buffer[current] = new ComponentSet(ComponentSet::allToBuffer(state));
	}
	void back(GameState &state)
	{
		if (current == maxBackward) return;
		//ui.selectedGameLocation = GameLocation();
		current = (current - 1) % constants::bufferSize;
		state.clearBoard();
		buffer[current]->addToComponents(state,vec2i(0,0));
	}
	void forward(GameState &state)
	{
		if (current == maxForward) return;
		//ui.selectedGameLocation = GameLocation();
		current = (current + 1) % constants::bufferSize;
		state.clearBoard();
		buffer[current]->addToComponents(state, vec2i(0, 0));
	}
	void reset(GameState &state)
	{
		current = 0;
		maxBackward = 0;
		maxForward = 0;

		if (buffer[0] != nullptr) delete(buffer[0]);
		buffer[current] = new ComponentSet(ComponentSet::allToBuffer(state));
	}


};