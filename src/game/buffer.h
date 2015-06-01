struct ComponentDefiningProperties
{
	ComponentDefiningProperties(Component* c)
	{
		baseInfo = c->baseInfo;
		location = c->location;
		color = c->modifiers.color;
		speed = c->modifiers.speed;
		preference = c->modifiers.chargePreference;
		intrinsics = c->intrinsics;
	}

	//The minimal set of information needed to define a component.  This probably overlaps with some of the code for saving and loading the circuit.
	const ComponentInfo* baseInfo;

	//location
	GameLocation location;

	//settable properties (what is circuit boundary type?)
	ChargeType color;
	WireSpeedType speed;
	int preference;

	//puzzle properties
	ComponentPuzzleType puzzleType;

	ComponentIntrinsics intrinsics;

	Component* toComponent()
	{
		Component* result = new Component(baseInfo->name, color, location);

		result->modifiers.speed = speed;

		result->modifiers.chargePreference = preference;
		result->modifiers.speed = speed;
		//result->modifiers.boundary = (CircuitBoundaryType)table.getInt("boundary");
		result->modifiers.puzzleType = puzzleType;

		result->intrinsics = intrinsics;

		return result;
	}
	//Component* toComponent() { return toComponent(vec2i(0, 0)); }

};

struct ComponentSet
{
	ComponentSet() { components = {}; }
	~ComponentSet()
	{
		for (ComponentDefiningProperties* c : components)
			delete(c);
	}
	
	// properties
	vector<ComponentDefiningProperties*> components;


	void flipAboutHorizonal();
	void flipAboutVerical();
	void rotate(int i); //rotates the buffer about the centerish by i*90 degrees clockwise
	rect2i boundingBox();

	//functions
	void addToComponents(GameState &state, vec2i offset);
	void addToCircuit(GameState &state, vec2i posOfCircuit, vec2i offset);

	static ComponentSet* allToBuffer(const GameState &state);
};

struct BackBuffer
{
	void init() {
		int current=0;
		int maxForward = 0;
		int maxBackward = 0;
		for (int i = 0; i < constants::bufferSize; i++)
			buffer[i] = nullptr;

	}

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

		buffer[current] = ComponentSet::allToBuffer(state);
	}
	void back(GameState &state)
	{
		if (current == maxBackward) return;
		current = (current - 1) % constants::bufferSize;
		state.clearBoard();
		buffer[current]->addToComponents(state,vec2i(0,0));
	}
	void forward(GameState &state)
	{
		if (current == maxForward) return;
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
		buffer[current] = ComponentSet::allToBuffer(state);
	}


};