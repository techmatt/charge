
struct GameState
{
	void init();
    void resetPuzzle();
    void addNewComponent(Component *component);
    void step();

    Component* getComponent(const GameLocation &pos);
 
    Board board;
    vector<Component*> components;
    vector<Charge> charges;
    vector<ExplodingCharge> explodingCharges;

    int stepCount;
};
