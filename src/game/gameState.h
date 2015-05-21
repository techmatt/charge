
struct GameState
{
	void init();
    
    void savePuzzle(const string &filename);
    void loadPuzzle(const string &filename);

    void clearBoard();
    void resetPuzzle();
    
    void addNewComponent(Component *component);
    void removeComponent(Component *component);
    void step();

    Component* getComponent(const GameLocation &pos);
 
    Board board;
    vector<Component*> components;
    vector<Charge> charges;
    vector<ExplodingCharge> explodingCharges;

    int stepCount;
};
