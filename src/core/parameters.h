
struct GameParameters
{
    GameParameters()
    {
        memset(this, 0, sizeof(GameParameters));
    }

    void load(const ParameterFile &params)
    {
        params.readParameter("assetDir", assetDir);

        params.readParameter("canonicalDims", canonicalDims);

        params.readParameter("boardDims", boardDims);
        
        params.readParameter("boardCanonicalStart", boardCanonicalStart);
        params.readParameter("boardCanonicalCellSize", boardCanonicalCellSize);

        //
        // derived parameters
        //
        boardCanonicalRect = rect2f(boardCanonicalStart, boardCanonicalStart + float(boardCanonicalCellSize) * vec2f(boardDims));
    }

    string assetDir;

    vec2f canonicalDims;

    vec2i boardDims;

    vec2f boardCanonicalStart;
    int boardCanonicalCellSize;

    //
    // derived parameters
    //
    rect2f boardCanonicalRect;

    Database *database;
};

extern GameParameters g_gameParams;

inline const GameParameters& params()
{
    return g_gameParams;
}

inline Database& database()
{
    return *g_gameParams.database;
}

inline GameParameters& mutableParams()
{
    return g_gameParams;
}

void initGameParams(const ParameterFile &params);
