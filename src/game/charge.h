
struct Charge
{
    Charge(const GameLocation &originLocation, ChargeType _level);

    //
    // advance the charge towards its destination
    //
    void advance(GameState &state);

    //
    // if the charge has reached its destination, process the component
    //
    void interactWithDestination(GameState &state);

    //
    // if the charge has reached its destination, find a new component or destroy the charge if none can be found
    //
    void updateDestination(GameState &state);

    bool findBestDestination(GameState &state);
    void setNewDestination(GameState &state, Component &newDestination);
    
    double computePreference(GameState &state, Component &targetComponent);

    float rotationOffset(int gameTick) const;

    ChargeType level;
    GameLocation source, destination;
    int timeInTransit, totalTransitTime;
    float randomRotationOffset;

    //
    // when the update kills a charge, we mark it for deletion so the game's tick function knows to remove it.
    //
    bool markedForDeletion;
    bool showDeathAnimation;
};
