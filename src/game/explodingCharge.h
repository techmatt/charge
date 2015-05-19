
struct ExplodingCharge
{
    ExplodingCharge() {}
    ExplodingCharge(const GameLocation &_locationA, const GameLocation &_locationB, float _interpolation, ChargeType _level, int _totalFrames, float _baseRotationOffset, int _birthTick)
    {
        locationA = _locationA;
        locationB = _locationB;
        interpolation = _interpolation;
        level = _level;
        currentFrame = 0;
        totalFrames = _totalFrames;
        birthTick = _birthTick;
        baseRotationOffset = _baseRotationOffset;
    }

    float percentDone() const
    {
        return (float)currentFrame / (float)totalFrames;
    }

    bool done() const
    {
        return (currentFrame == totalFrames);
    }

    void advance()
    {
        if(!done()) currentFrame++;
    }

    GameLocation locationA, locationB;
    float interpolation;
    float baseRotationOffset;
    ChargeType level;
    int currentFrame, totalFrames;
    int birthTick;
};
