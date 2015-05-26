#define MeshType D3D9Mesh
#define TextureType D3D9Texture

struct RenderBuildingInfo
{
	TextureType Tex;
	TextureType PerChargeTex[MaxChargeLevel + 1];
};

const int GameGridDimension = 32;
const int MenuSpacing = 10;
const int MaxPreferenceLevel = 5;
const int WireConnectorCount = 6;
const int BoundaryTextureCount = 23;

extern const Coordinate CircuitTopLeft;
extern const Coordinate PreferenceMenuTopLeft;

enum AlphaBlendingModeType
{
	AlphaBlendingModeHalfBlend,
	AlphaBlendingModePreference,
	AlphaBlendingModeBuilding,
	AlphaBlendingModeToolTip,
};

class ThatWhichRendersGames
{
public:
    friend class MainControl;

	void Init(FileCollection &Data, Game &G, GraphicsDevice &GD);
	void Render(GraphicsDevice &GD, Game &G, GameUI &UI, Matrix4 &InverseViewportMatrix,
				const Coordinate &WindowDimensions, const Coordinate &ExpectedWindowDimensions);
	
	static Coordinate GridCoordinateToScreenCoordinate(const Coordinate &C);
	static Coordinate ScreenCoordinateToGridCoordinate(const Coordinate &C);
	
	static Coordinate CircuitCoordinateToScreenCoordinate(const Coordinate &C);
	static Coordinate ScreenCoordinateToCircuitCoordinate(const Coordinate &C);

	static Coordinate BuildingMenuToScreenCoordinate(const Coordinate &C);
	static Coordinate UpgradeMenuToScreenCoordinate(UINT XIndex);
	static Coordinate PreferenceMenuToScreenCoordinate(UINT XIndex);
	
private:
    //
    // Init
    //
    void LoadBitmapFromCollection(FileCollection &Data, const String &Name, Bitmap &Bmp);
    void LoadTextureFromCollection(FileCollection &Data, GraphicsDevice &GD, TextureType &Tex, const String &Name);
    void LoadBuildingTexture(FileCollection &Data, GraphicsDevice &GD, BuildingType Type, const String &Str);
    void InitMeshes(GraphicsDevice &GD);
    void InitTextures(FileCollection &Data, Game &G, GraphicsDevice &GD);
	
    //
    // Alpha blending
    //
    void EnableAlphaBlending(GraphicsDevice &GD, AlphaBlendingModeType Type);
	void DisableAlphaBlending(GraphicsDevice &GD);

    //
    // Rendering
    //
	void RenderButtonBackground(GraphicsDevice &GD, Game &G, Matrix4 &InverseViewportMatrix,
					    const Coordinate &ScreenCoordinate, float ScaleFactor, float ZDepth);
	void RenderMegaHoldFill(GraphicsDevice &GD, Game &G, Matrix4 &InverseViewportMatrix, Building &B, float ZDepth);
	void RenderBuilding(GraphicsDevice &GD, Game &G, Matrix4 &InverseViewportMatrix, BuildingType Type,
					    const Coordinate &ScreenCoordinate, int ChargeLevel, float ScaleFactor,
					    float WireZDepth, float BuildingZDepth, float PreferenceZDepth, UINT PreferenceLevel, bool ForegroundLayer);
    void RenderBlocker(GraphicsDevice &GD, Game &G, Matrix4 &InverseViewportMatrix, const Coordinate &GridCoord, bool Render2x2);
	void RenderBuildingSpokes(GraphicsDevice &GD, Game &G, Matrix4 &InverseViewportMatrix, Building &B, bool ActiveCircuit);
    void RenderDesktop(GraphicsDevice &GD, Game &G, Matrix4 &InverseViewportMatrix);
	void RenderBorder(GraphicsDevice &GD, Game &G, Matrix4 &InverseViewportMatrix, Coordinate &Coord, bool DoubleSized);
	void RenderCircuitHold(GraphicsDevice &GD, Game &G, Matrix4 &InverseViewportMatrix, const Coordinate &Coord, bool OnCircuitBoard);
	void RenderMask(GraphicsDevice &GD, Game &G, Matrix4 &InverseViewportMatrix, Coordinate &Coord);
	void RenderSelector(GraphicsDevice &GD, Game &G, Matrix4 &InverseViewportMatrix, Coordinate &Coord, TextureType &Tex);
    void RenderBuildable(GraphicsDevice &GD, Game &G, Matrix4 &InverseViewportMatrix, Coordinate &Coord);
	void RenderTooltip(GraphicsDevice &GD, Game &G, Matrix4 &InverseViewportMatrix, Rectangle2i &Region,
					   const Coordinate &WindowDimensions, const Coordinate &ExpectedWindowDimensions,
					   const String &Text, bool RenderBackdrop);
	void RenderCharge(GraphicsDevice &GD, Game &G, Matrix4 &InverseViewportMatrix, Charge &C, bool ActiveCircuit);
	void RenderExplodingCharge(GraphicsDevice &GD, Game &G, Matrix4 &InverseViewportMatrix, ExplodingCharge &C, bool ActiveCircuit);
    void RenderCircuit(GraphicsDevice &GD, Game &G, GameUI &UI, Matrix4 &InverseViewportMatrix, Building &B, bool ForegroundLayer);
  
    void UpdateForeground(GraphicsDevice &GD, Game &G, GameUI &UI, Matrix4 &InverseViewportMatrix,
				const Coordinate &WindowDimensions, const Coordinate &ExpectedWindowDimensions);
    void UpdateBackdrop(GraphicsDevice &GD, Game &G, GameUI &UI, Matrix4 &InverseViewportMatrix,
				const Coordinate &WindowDimensions, const Coordinate &ExpectedWindowDimensions);
    void UpdateEffectiveBlockedGrid(Game &G);

	Coordinate GetScreenCoordinate(const Coordinate &CircuitLocation, const Coordinate &GridLocation);

	RenderBuildingInfo _BuildingInfo[BuildingCount];
	TextureType _CircuitHoldTexture;
	TextureType _BorderTexture;
	TextureType _WireTexture;
    TextureType _WireConnectorTexture[WireConnectorCount];
	TextureType _WhiteTexture;
	TextureType _SelectorTexture, _PuzzlePieceTexture;
	TextureType _CopiedTexture;
    TextureType _SphereChargeTextures[MaxChargeLevel + 1];
    TextureType _TexturedChargeTextures[MaxChargeLevel + 1];
	TextureType _ChargePreferenceTextures[MaxPreferenceLevel];
	TextureType _CircuitBoundaryBaseGridVersion;
    TextureType _BoundaryTextures[BoundaryTextureCount];
    TextureType _DesktopTexture;
    TextureType _BuildableTexture;

	Mesh _SpokeMesh;
	MeshType _CircuitHoldMesh;
	MeshType _SelectorMesh;
	MeshType _BuildingMesh;
	MeshType _BorderMesh;
	MeshType _MaskMesh;
	MeshType _SphereChargeMesh;
    MeshType _TexturedChargeMesh;
	MeshType _TooltipMesh;
    MeshType _DesktopMesh;

    D3D9RenderTargetSurface _Backdrop;

    Grid<UINT> _EffectivelyBlockedGrid;
};
