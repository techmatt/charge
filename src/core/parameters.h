
struct GameParameters
{
    GameParameters()
    {
        memset(this, 0, sizeof(GameParameters));
    }

    void load(const ParameterFile &params)
    {
        params.readParameter("assetDir", assetDirBase);

        assetDir = util::getWorkingDirectory() + assetDirBase;
        rootDir = assetDir + "../";

        params.readParameter("canonicalDims", canonicalDims);

        params.readParameter("boardDims", boardDims);
        
        params.readParameter("boardCanonicalStart", boardCanonicalStart);
        params.readParameter("circuitCanonicalStart", circuitCanonicalStart);
        params.readParameter("circuitBackgroundCanonicalBoundarySize", circuitBackgroundCanonicalBoundarySize);
        params.readParameter("canonicalCellSize", canonicalCellSize);

        params.readParameter("componentMenuCanonicalStart", componentMenuCanonicalStart);
        params.readParameter("componentMenuCanonicalEntrySize", componentMenuCanonicalEntrySize);

        params.readParameter("puzzleMenuCanonicalStart", puzzleMenuCanonicalStart);
        params.readParameter("puzzleMenuCanonicalEntrySize", puzzleMenuCanonicalEntrySize);

        params.readParameter("attributeMenuCanonicalStart", attributeMenuCanonicalStart);
        params.readParameter("attributeMenuCanonicalEntrySize", attributeMenuCanonicalEntrySize);

        params.readParameter("tooltipDefaultStart", tooltipDefaultStart);
        params.readParameter("tooltipSize", tooltipSize);

        params.readParameter("godMode", godMode);

        params.readParameter("maxSkippedLevels", maxSkippedLevels);

        circuitDims = vec2i(constants::circuitBoardSize, constants::circuitBoardSize);

        //
        // derived parameters
        //
        boardCanonicalRect = rect2f(boardCanonicalStart, boardCanonicalStart + float(canonicalCellSize) * vec2f(boardDims));
        circuitCanonicalRect = rect2f(circuitCanonicalStart, circuitCanonicalStart + float(canonicalCellSize) * vec2f(circuitDims));

		// The canonical frame covers the rendered area and runs from 0,0 to params().canonicalDims
		boardInWindow = CoordinateFrame(boardCanonicalRect, vec2f(boardDims));
		componentMenuInWindow = CoordinateFrame(boardCanonicalRect);
		puzzleMenuInWindow = CoordinateFrame(puzzleMenuCanonicalStart);
		circuitInWindow = CoordinateFrame(circuitCanonicalRect, vec2f(circuitDims));
    }

    string rootDir;
    string assetDir;
    string assetDirBase;

    vec2f canonicalDims;

    vec2i boardDims;
    vec2i circuitDims;

    vec2f boardCanonicalStart;
    vec2f circuitCanonicalStart;
    float circuitBackgroundCanonicalBoundarySize;
    int canonicalCellSize;
    
	vec2f componentMenuCanonicalStart;
	int componentMenuCanonicalEntrySize;

    vec2f puzzleMenuCanonicalStart;
    int puzzleMenuCanonicalEntrySize;

    vec2f attributeMenuCanonicalStart;
    vec2f attributeMenuCanonicalEntrySize;

    vec2f tooltipDefaultStart;
    vec2f tooltipSize;

	CoordinateFrame boardInWindow;
	CoordinateFrame componentMenuInWindow;
	CoordinateFrame puzzleMenuInWindow;
	CoordinateFrame circuitInWindow;

    bool godMode;

    int maxSkippedLevels;

    //
    // derived parameters
    //
    rect2f boardCanonicalRect;
    rect2f circuitCanonicalRect;

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
