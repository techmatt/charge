
struct GameParameters
{
    GameParameters()
    {
        memset(this, 0, sizeof(GameParameters));
    }

    void load(const ParameterFile &params)
    {
        params.readParameter("canonicalDims", canonicalDims);

        params.readParameter("boardDims", boardDims);
        
        params.readParameter("boardCanonicalStart", boardCanonicalStart);
        params.readParameter("boardCanonicalCellSize", boardCanonicalCellSize);

        //boardCanonicalRect = rect2f(
    }

    vec2f canonicalDims;

    vec2f boardDims;

    vec2f boardCanonicalStart;
    int boardCanonicalCellSize;

    //
    // derived parameters
    //
    rect2f boardCanonicalRect;
};

extern GameParameters g_gameParams;

inline const GameParameters& params()
{
    return g_gameParams;
}

inline GameParameters& mutableParams()
{
    return g_gameParams;
}

inline void initGameParams(const ParameterFile &params)
{
    g_gameParams.load(params);
}
