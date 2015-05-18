
struct GameState
{
	void init();
    void addNewComponent(Component *component);
    void step();

    Board board;
    vector<Component*> components;
    vector<Charge> charges;
    //vector<ExplodingCharge> explodingCharges;

    int stepCount;
};
