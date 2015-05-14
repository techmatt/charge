#include "Main.h"

const bool UseTexturedCharges = true;
const float ChargeSecondPerRotations = 1.0f;
const float ChargeJitterMagnitude = 1.0f;

const float CircuitChargeSize = 0.3f;
const float ChargeScaleWithLevelFactor = 0.25f;

const float SpokeRadius = 3.7f;
const float SpokeLength = 14.0f;

const float DesktopBackgroundLayerZDepth = 0.95f;

const float CircuitBackgroundLayerZDepth = 0.9f;

const float BlockerLayerZDepth = 0.85f;
const float BuildingButtonBackgroundZDepth = 0.85f;

const float BuildingPreferenceZDepth = 0.845f;

const float BuildingWireBaseLayerZDepth = 0.83f;
const float BuildingSpokeLayerZDepth = 0.82f;
const float BuildingArrowLayerZDepth = 0.81f;
const float BuildingLayerZDepth = 0.8f;
const float BuildingHoldFillZDepth = 0.79f;

const float ExplodingChargeZDepth = 0.31f;
const float ChargeLayerZDepth = 0.3f;

const float BuildableLayerZDepth = 0.26f;
const float SelectorLayerZDepth = 0.25f;
const float MaskLayerZDepth = 0.2f;

const float ToolTipLayerZDepth = 0.1f;

const Coordinate TowerMenuTopLeft(787, 10);
const Coordinate GridTopLeft(5, 5);
const Coordinate UpgradeMenuTopLeft(787, 306);
const Coordinate PreferenceMenuTopLeft(787, 232);
const Coordinate CircuitTopLeft(781, 379);
const Coordinate ToolTipDimension(448, 200);

Coordinate ThatWhichRendersGames::GridCoordinateToScreenCoordinate(const Coordinate &C)
{
	return GridTopLeft + Coordinate(C.x * GameGridDimension, C.y * GameGridDimension);
}

Coordinate ThatWhichRendersGames::ScreenCoordinateToGridCoordinate(const Coordinate &C)
{
	return Coordinate((C.x - GridTopLeft.x) / GameGridDimension, (C.y - GridTopLeft.y) / GameGridDimension);
}

Coordinate ThatWhichRendersGames::CircuitCoordinateToScreenCoordinate(const Coordinate &C)
{
	return CircuitTopLeft + Coordinate(C.x * GameGridDimension, C.y * GameGridDimension);
}

Coordinate ThatWhichRendersGames::ScreenCoordinateToCircuitCoordinate(const Coordinate &C)
{
	return Coordinate((C.x - CircuitTopLeft.x) / GameGridDimension, (C.y - CircuitTopLeft.y) / GameGridDimension);
}

Coordinate ThatWhichRendersGames::BuildingMenuToScreenCoordinate(const Coordinate &C)
{
	return TowerMenuTopLeft + Coordinate(C.x * (GameGridDimension * 2 + MenuSpacing), C.y * (GameGridDimension * 2 + MenuSpacing));
}

Coordinate ThatWhichRendersGames::UpgradeMenuToScreenCoordinate(UINT XIndex)
{
	return UpgradeMenuTopLeft + Coordinate(XIndex * (GameGridDimension * 2 + MenuSpacing), 0);
}

Coordinate ThatWhichRendersGames::PreferenceMenuToScreenCoordinate(UINT XIndex)
{
	return PreferenceMenuTopLeft + Coordinate(XIndex * (GameGridDimension * 2 + MenuSpacing), 0);
}

void ColorToAlpha(Bitmap &Bmp, RGBColor Color)
{
	for(UINT y = 0; y < Bmp.Height(); y++)
	{
		for(UINT x = 0; x < Bmp.Width(); x++)
		{
			RGBColor &C = Bmp[y][x];
			if(C == Color)
			{
				C = RGBColor(0, 0, 0, 255);
			}
			else
			{
				C.a = 0;
			}
		}
	}
}

void ThatWhichRendersGames::LoadBitmapFromCollection(FileCollection &Data, const String &Name, Bitmap &Bmp)
{
    String FileCollectionName = Name + String(".png");
    String ExistingFilename = "UncompiledData\\" + FileCollectionName;
    FileCollectionFile *Result = Data.AddAndUpdateFile(FileCollectionName, ExistingFilename);
    Bmp.LoadPNGFromMemory(Result->Data);
    Bmp.FlipVertical();
    ColorToAlpha(Bmp, RGBColor::Cyan);
}

void ThatWhichRendersGames::LoadTextureFromCollection(FileCollection &Data, GraphicsDevice &GD, TextureType &Tex, const String &Name)
{
    Bitmap Bmp;
    LoadBitmapFromCollection(Data, Name, Bmp);
    Tex.Associate(GD);
    Tex.Load(Bmp);
}

void ThatWhichRendersGames::LoadBuildingTexture(FileCollection &Data, GraphicsDevice &GD, BuildingType Type, const String &Str)
{
	Bitmap Bmp;
    LoadBitmapFromCollection(Data, Str, Bmp);
    
	if(Type == BuildingAmplifier   ||
	   Type == BuildingAmplifierL1 ||
	   Type == BuildingAmplifierL2 ||
	   Type == BuildingAmplifierL3 ||
	   Type == BuildingAmplifierL4 ||
	   Type == BuildingHold		   ||
	   Type == BuildingRedChargeGoal    ||
	   Type == BuildingOrangeChargeGoal ||
	   Type == BuildingYellowChargeGoal ||
	   Type == BuildingGreenChargeGoal  ||
	   Type == BuildingBlueChargeGoal)
	{
		for(UINT ChargeLevelIndex = 0; ChargeLevelIndex < MaxChargeLevel + 1; ChargeLevelIndex++)
		{
			Bitmap BmpCopy = Bmp;
            RGBColor ReplacementColor = Charge::ColorFromLevel(ChargeLevelIndex);
            if(   ChargeLevelIndex == 0 &&
                  (Type == BuildingAmplifier   ||
	               Type == BuildingAmplifierL1 ||
	               Type == BuildingAmplifierL2 ||
	               Type == BuildingAmplifierL3 ||
	               Type == BuildingAmplifierL4))
            {
                ReplacementColor = RGBColor::White;
            }
			BmpCopy.ReplaceColor(RGBColor::Magenta, ReplacementColor);
			if(ChargeLevelIndex == 0)
			{
				_BuildingInfo[Type].Tex.Load(BmpCopy);
			}
			_BuildingInfo[Type].PerChargeTex[ChargeLevelIndex].Load(BmpCopy);
		}
	}
	else
	{
        _BuildingInfo[Type].Tex.Associate(GD);
		_BuildingInfo[Type].Tex.Load(Bmp);
	}
}

void ThatWhichRendersGames::InitMeshes(GraphicsDevice &GD)
{
    _SpokeMesh.SetGD(GD);
    _SpokeMesh.CreatePlane(1.0f, 2, 2);
    _SpokeMesh.SetColor(RGBColor::White);

	_BuildingMesh.SetGD(GD);
	_BuildingMesh.CreatePlane(GameGridDimension * 2.0f, 2, 2);
	_BuildingMesh.Translate(Vec3(GameGridDimension * 1.0f, GameGridDimension * 1.0f, 0.0f));

	_CircuitHoldMesh.SetGD(GD);
	_CircuitHoldMesh.CreatePlane(GameGridDimension * 10.0f, 2, 2);
	_CircuitHoldMesh.Translate(Vec3(GameGridDimension * 5.0f, GameGridDimension * 5.0f, 0.0f));

	_BorderMesh.SetGD(GD);
	_BorderMesh.CreatePlane(GameGridDimension * 1.0f, 2, 2);
	_BorderMesh.Translate(Vec3(GameGridDimension * 0.5f, GameGridDimension * 0.5f, 0.0f));

    _DesktopMesh.SetGD(GD);
    _DesktopMesh.CreatePlane(Vec3::Origin, Vec3(float(ExpectedWindowDimensions.x), float(ExpectedWindowDimensions.y), 0.0f), 2);

	_TooltipMesh.SetGD(GD);
	_TooltipMesh.CreatePlane(Vec3::Origin, Vec3(ToolTipDimension.AsVec2(), 0.0f), 2);
	_TooltipMesh.SetColor(RGBColor(20, 20, 40, 50));

	_SphereChargeMesh.SetGD(GD);
	_SphereChargeMesh.CreateSphere(GameGridDimension / 7.0f, 3);
	_SphereChargeMesh.SetColor(RGBColor::White);
	for(UINT VertexIndex = 0; VertexIndex < _SphereChargeMesh.VertexCount(); VertexIndex++)
	{
		_SphereChargeMesh.Vertices()[VertexIndex].Pos.z = 0.0f;
	}

    _TexturedChargeMesh.SetGD(GD);
	_TexturedChargeMesh.CreatePlane(GameGridDimension * 0.7f, 2, 2);

	_MaskMesh = _BorderMesh;
}

void ThatWhichRendersGames::InitTextures(FileCollection &Data, Game &G, GraphicsDevice &GD)
{
    for(UINT Index = 0; Index < BuildingCount; Index++)
    {
        _BuildingInfo[Index].Tex.Associate(GD);
        for(UINT ChargeLevelIndex = 0; ChargeLevelIndex < MaxChargeLevel + 1; ChargeLevelIndex++)
        {
            _BuildingInfo[Index].PerChargeTex[ChargeLevelIndex].Associate(GD);
        }
        LoadBuildingTexture(Data, GD, BuildingType(Index), G.GetBuildingInfo()[Index].Name);
    }

    Bitmap Bmp;

    for(UINT Index = 0; Index < MaxPreferenceLevel; Index++)
    {
        LoadBitmapFromCollection(Data, String("PreferenceMask") + String(Index), Bmp);
        Bitmap AlphaBmp;
        LoadBitmapFromCollection(Data, String("PreferenceMask") + String(Index) + String("Alpha"), AlphaBmp);
        for(UINT y = 0; y < Bmp.Height(); y++)
        {
            for(UINT x = 0; x < Bmp.Width(); x++)
            {
                RGBColor &C = Bmp[y][x];
                C.a = AlphaBmp[y][x].r;
            }
        }
        _ChargePreferenceTextures[Index].Associate(GD);
        _ChargePreferenceTextures[Index].Load(Bmp);
    }

    LoadTextureFromCollection(Data, GD, _WireTexture, "WireBase");

    for(UINT Index = 0; Index < WireConnectorCount; Index++)
    {
        LoadTextureFromCollection(Data, GD, _WireConnectorTexture[Index], "WireConnector" + String(Index));
    }

    LoadTextureFromCollection(Data, GD, _CircuitBoundaryBaseGridVersion, "CircuitBoundaryBaseGridVersion");
    LoadTextureFromCollection(Data, GD, _CircuitHoldTexture, "MegaHoldCircuit");
    LoadTextureFromCollection(Data, GD, _BorderTexture, "Border");
    LoadTextureFromCollection(Data, GD, _SelectorTexture, "Selector");
    LoadTextureFromCollection(Data, GD, _PuzzlePieceTexture, "PuzzleSelector");
    LoadTextureFromCollection(Data, GD, _CopiedTexture, "CircuitCopyBoundary");
    LoadTextureFromCollection(Data, GD, _DesktopTexture, "Background");
    LoadTextureFromCollection(Data, GD, _BuildableTexture, "Buildable");

    Bmp.Allocate(8, 8);
    Bmp.Clear(RGBColor::White);
    _WhiteTexture.Associate(GD);
    _WhiteTexture.Load(Bmp);

    Bitmap BorderBmp;
    LoadBitmapFromCollection(Data, "Border", BorderBmp);
    for(UINT Index = 0; Index < BoundaryTextureCount; Index++)
    {
        LoadBitmapFromCollection(Data, String("BoundaryTextures") + String(Index), Bmp);
        /*for(UINT y = 0; y < BorderBmp.Height(); y++)
        {
            for(UINT x = 0; x < BorderBmp.Width(); x++)
            {
                RGBColor &C = Bmp[y][x];
                if(C == RGBColor::Cyan)
                {
                    C = RGBColor::Magenta;
                }
                else if(C == RGBColor::Black)
                {
                    C = RGBColor::Cyan;
                }
            }
        }
        //Bmp.FlipVertical();
        Bmp.SavePNG("UncompiledData\\BoundaryTextures" + String(Index) + String(".png"));*/

        for(UINT y = 0; y < BorderBmp.Height(); y++)
        {
            for(UINT x = 0; x < BorderBmp.Width(); x++)
            {
                RGBColor &C = Bmp[y][x];
                if(C == RGBColor::Magenta)
                {
                    C = BorderBmp[y][x];
                }
            }
        }

        _BoundaryTextures[Index].Associate(GD);
        _BoundaryTextures[Index].Load(Bmp);
    }

    Bitmap ChargeAlphaChannel;
    LoadBitmapFromCollection(Data, "ChargeTextureAlpha", ChargeAlphaChannel);

    for(UINT Index = 0; Index <= MaxChargeLevel; Index++)
    {
        LoadBitmapFromCollection(Data, String("ChargeTexture") + String(Index), Bmp);
        for(UINT y = 0; y < Bmp.Height(); y++)
        {
            for(UINT x = 0; x < Bmp.Width(); x++)
            {
                RGBColor &C = Bmp[y][x];
                C.a = 255 - ChargeAlphaChannel[y][x].r;
            }
        }
        _TexturedChargeTextures[Index].Associate(GD);
        _TexturedChargeTextures[Index].Load(Bmp);

        Bmp.Allocate(8, 8);
        Bmp.Clear(Charge::ColorFromLevel(Index + 1));
        _SphereChargeTextures[Index].Associate(GD);
        _SphereChargeTextures[Index].Load(Bmp);
    }
}

void ThatWhichRendersGames::Init(FileCollection &Data, Game &G, GraphicsDevice &GD)
{
    InitMeshes(GD);
    InitTextures(Data, G, GD);
    _EffectivelyBlockedGrid.Allocate(G.GetBoard().Cells().Rows(), G.GetBoard().Cells().Cols());
}

Coordinate ThatWhichRendersGames::GetScreenCoordinate(const Coordinate &CircuitLocation, const Coordinate &GridLocation)
{
	if(CircuitLocation == Coordinate::Invalid)
	{
		return GridCoordinateToScreenCoordinate(GridLocation);
	}
	else
	{
		return GridCoordinateToScreenCoordinate(CircuitLocation) + Coordinate(GridLocation.x * GameGridDimension / 7, GridLocation.y * GameGridDimension / 7);
	}
}

void ThatWhichRendersGames::RenderMegaHoldFill(GraphicsDevice &GD, Game &G, Matrix4 &InverseViewportMatrix, Building &B, float ZDepth)
{
	float ScaleFactor = 0.0f;
	UINT MaxCharge = B.Fields()[0];
	if(MaxCharge > 0)
	{
		ScaleFactor = float(B.AbsorbedCharge()) / float(MaxCharge);
		if(ScaleFactor > 1.0f)
		{
			ScaleFactor = 1.0f;
		}
	}

	Coordinate ScreenCoordinate;

	if(B.OnCircuitBoard())
	{
		ScaleFactor *= 24.2f;
		ScreenCoordinate = CircuitCoordinateToScreenCoordinate(Coordinate(7, 7));
	}
	else
	{
		ScaleFactor *= 3.1f;
		ScreenCoordinate = GetScreenCoordinate(Coordinate::Invalid, B.Location() + Coordinate(1, 1));
	}
	
	Matrix4 Transform = Matrix4::Scaling(ScaleFactor) * Matrix4::Translation(Vec3(ScreenCoordinate.AsVec2(), ZDepth)) * InverseViewportMatrix;
	
	GD.LoadMatrix(MatrixController(Transform));
	UINT ChargeLevel = 5;
	if(B.Type() >= BuildingMegaHoldRed && B.Type() <= BuildingMegaHoldBlue)
	{
		ChargeLevel = B.Type() - BuildingMegaHoldRed;
	}
	_SphereChargeTextures[ChargeLevel].Set(0);

	_SphereChargeMesh.Render();
}

void ThatWhichRendersGames::RenderCharge(GraphicsDevice &GD, Game &G, Matrix4 &InverseViewportMatrix, Charge &C, bool ActiveCircuit)
{
	Coordinate ScreenCoordinateStart, ScreenCoordinateEnd;

	Assert(C.Level() > 0 && C.Level() <= MaxChargeLevel, "Invalid charge level");
	float StartScaleFactor = 1.0f + (C._Level - 1) * ChargeScaleWithLevelFactor;
	float EndScaleFactor = 1.0f + (C._Level - 1) * ChargeScaleWithLevelFactor;

	if(ActiveCircuit)
	{
    ScreenCoordinateEnd = CircuitCoordinateToScreenCoordinate(C._Destination.Location + Coordinate(1, 1));
		ScreenCoordinateStart = CircuitCoordinateToScreenCoordinate(C._Source.Location + Coordinate(1, 1));
	}
	else
	{
		ScreenCoordinateStart = GetScreenCoordinate(C._Source.Circuit, C._Source.Location + Coordinate(1, 1));
		ScreenCoordinateEnd = GetScreenCoordinate(C._Destination.Circuit, C._Destination.Location + Coordinate(1, 1));

		if(C._Source.Circuit != Coordinate::Invalid)
		{
			StartScaleFactor = CircuitChargeSize;
		}
		if(C._Destination.Circuit != Coordinate::Invalid)
		{
			EndScaleFactor = CircuitChargeSize;
		}
	}

    if(C._Source.Location == Coordinate::Invalid)
    {
        ScreenCoordinateStart = ScreenCoordinateEnd;
    }
	
	Vec2 VecStart = ScreenCoordinateStart.AsVec2();
	Vec2 VecEnd = ScreenCoordinateEnd.AsVec2();

	float Factor = 0.0f;
	if(C._TotalTransitTime > 0)
	{
		Factor = float(C._TimeInTransit) / float(C._TotalTransitTime);
	}
	Vec2 VecFinal = Vec2::Lerp(VecStart, VecEnd, Factor);
	float ScaleFactor = Math::Lerp(StartScaleFactor, EndScaleFactor, Factor);

    Matrix4 Rotation = Matrix4::RotationZ(G.CurrentTime() * SecondsPerGameFrame * 2.0f * Math::PIf * ChargeSecondPerRotations + C.RandomValue() * 2.0f * Math::PIf);
    //Matrix4 Jitter = Matrix4::Translation(Vec3(ChargeJitterMagnitude * pmrnd(), ChargeJitterMagnitude * pmrnd(), 0.0f));
    Matrix4 Transform = Rotation * Matrix4::Scaling(Vec3(ScaleFactor, ScaleFactor, 1.0f)) * Matrix4::Translation(Vec3(VecFinal, ChargeLayerZDepth)) * InverseViewportMatrix;

    GD.LoadMatrix(MatrixController(Transform));

    if(UseTexturedCharges)
    {
        EnableAlphaBlending(GD, AlphaBlendingModeBuilding);
        _TexturedChargeTextures[C.Level() - 1].Set(0);
        _TexturedChargeMesh.Render();
        DisableAlphaBlending(GD);
    }
    else
    {
        _SphereChargeTextures[C.Level() - 1].Set(0);
        _SphereChargeMesh.Render();
    }
}

void ThatWhichRendersGames::RenderExplodingCharge(GraphicsDevice &GD, Game &G, Matrix4 &InverseViewportMatrix, ExplodingCharge &C, bool ActiveCircuit)
{
	Coordinate ScreenCoordinate;

	float MaxScaleFactor = (1.0f + (C.ChargeLevel - 1) * ChargeScaleWithLevelFactor) * 3.0f;
	float CurScaleFactor = Math::LinearMap(0.0f, float(C.TotalFrames - 1.0f), 1.0f, MaxScaleFactor, float(C.CurrentFrame));

	if(ActiveCircuit)
	{
		ScreenCoordinate = CircuitCoordinateToScreenCoordinate(C.Location + Coordinate(1, 1));
	}
	else
	{
		ScreenCoordinate = GetScreenCoordinate(C.ParentCircuit, C.Location + Coordinate(1, 1));
		if(C.ParentCircuit != Coordinate::Invalid)
		{
			CurScaleFactor *= 0.2f;
		}
	}
	
  Matrix4 Rotation = Matrix4::RotationZ(G.CurrentTime() * SecondsPerGameFrame * 2.0f * Math::PIf * ChargeSecondPerRotations * 2.0f);
  //Matrix4 Jitter = Matrix4::Translation(Vec3(ChargeJitterMagnitude * pmrnd(), ChargeJitterMagnitude * pmrnd(), 0.0f));
	Matrix4 Transform = Rotation * Matrix4::Scaling(CurScaleFactor) * Matrix4::Translation(Vec3(float(ScreenCoordinate.x), float(ScreenCoordinate.y), ExplodingChargeZDepth)) * InverseViewportMatrix;
	
	GD.LoadMatrix(MatrixController(Transform));
	
  if(UseTexturedCharges)
  {
    EnableAlphaBlending(GD, AlphaBlendingModeBuilding);
    _TexturedChargeTextures[C.ChargeLevel - 1].Set(0);
	  _TexturedChargeMesh.Render();
    DisableAlphaBlending(GD);
  }
  else
  {
    EnableAlphaBlending(GD, AlphaBlendingModeHalfBlend);
	  _SphereChargeTextures[C.ChargeLevel - 1].Set(0);
	  _SphereChargeMesh.Render();
    DisableAlphaBlending(GD);
  }
}

void ThatWhichRendersGames::RenderBuildingSpokes(GraphicsDevice &GD, Game &G, Matrix4 &InverseViewportMatrix, Building &B, bool ActiveCircuit)
{
  if(B.Type() == BuildingBlocker ||
	   B.Type() == BuildingCircuitBoundaryOpen ||
	   B.Type() == BuildingCircuitBoundaryBlocked)
	{
    return;
  }

	GD.LoadMatrix(MatrixController(InverseViewportMatrix));
	//_BorderTexture.SetNull(0);
  //_WireConnectorTexture[Index].Set(0);

	Building *ParentCircuit = NULL;
  float ScaleFactor = 1.0f;
  float SpokeRadiusScaleFactor = 1.0f;
	if(B.ParentCircuit() != Coordinate::Invalid)
	{
		ParentCircuit = G.CircuitFromLocation(B.ParentCircuit());
    if(!ActiveCircuit)
    {
      ScaleFactor = 1.0f / 7.0f;
      SpokeRadiusScaleFactor = ScaleFactor * 1.3f;
    }
	}

	Vec3 SourceWorldPos;
	if(ParentCircuit == NULL)
	{
		SourceWorldPos = Vec3((GridCoordinateToScreenCoordinate(B.Location()) + Coordinate(GameGridDimension, GameGridDimension)).AsVec2(), BuildingSpokeLayerZDepth);
	}
	else
	{
    if(ActiveCircuit)
    {
		  SourceWorldPos = Vec3((CircuitCoordinateToScreenCoordinate(B.Location()) + Coordinate(GameGridDimension, GameGridDimension)).AsVec2(), BuildingSpokeLayerZDepth);
    }
    else
    {
      SourceWorldPos = Vec3((GridCoordinateToScreenCoordinate(ParentCircuit->Location()) +
                            Coordinate((B.Location().x + 1) * GameGridDimension / 7, (B.Location().y + 1) * GameGridDimension / 7)).AsVec2(),
                            BuildingSpokeLayerZDepth);
    }
	}

	Grid<BoardCell> *CurGrid = NULL;
	if(ParentCircuit == NULL)
	{
		CurGrid = &G.GetBoard().Cells();
	}
	else
	{
		CurGrid = &ParentCircuit->CircuitBoard().Cells();
	}

  UINT ArrowIndex = B.Fields()[BuildingFieldArrowIndex];
	if(ArrowIndex != 0)
	{
		Coordinate ArrowOffset = Building::OffsetCoordinateFromArrowIndex(ArrowIndex);

		Vec3 SourceWorldPos, DestWorldPos;
		if(ParentCircuit == NULL)
		{
			SourceWorldPos = Vec3((GridCoordinateToScreenCoordinate(B.Location()) + Coordinate(GameGridDimension, GameGridDimension)).AsVec2(), BuildingArrowLayerZDepth);
			DestWorldPos = Vec3((GridCoordinateToScreenCoordinate(B.Location() + ArrowOffset) + Coordinate(GameGridDimension, GameGridDimension)).AsVec2(), BuildingArrowLayerZDepth);
		}
		else
		{
			SourceWorldPos = Vec3((CircuitCoordinateToScreenCoordinate(B.Location()) + Coordinate(GameGridDimension, GameGridDimension)).AsVec2(), BuildingArrowLayerZDepth);
			DestWorldPos = Vec3((CircuitCoordinateToScreenCoordinate(B.Location() + ArrowOffset) + Coordinate(GameGridDimension, GameGridDimension)).AsVec2(), BuildingArrowLayerZDepth);
		}

    GetThickLineVertices(SourceWorldPos, SpokeRadius * 2.0f, (SourceWorldPos + DestWorldPos) * 0.5f, 0.0f, _SpokeMesh.Vertices()[0].Pos, _SpokeMesh.Vertices()[1].Pos, _SpokeMesh.Vertices()[2].Pos, _SpokeMesh.Vertices()[3].Pos);

    _PuzzlePieceTexture.Set(0);
		_SpokeMesh.Render();
    //_WireConnectorTexture.Set(0);
	}

	for(int Case = -1; Case <= 1; Case++)
	{
		for(int Axis = 0; Axis < 2; Axis++)
		{
			Coordinate Offset;
			if(Axis == 0) Offset = Coordinate(-2, Case);
      if(Axis == 1) Offset = Coordinate(Case, -2);
			//if(Axis == 2) Offset = Coordinate(2, Case);
			//if(Axis == 3) Offset = Coordinate(Case, 2);
      UINT ConnectorIndex = (Case + 1) * 2 + Axis;
			Coordinate Pos = B.Location() + Offset;

			if(CurGrid->ValidCoordinates(Pos.y, Pos.x))
			{
				Building *CurBuilding = CurGrid->GetElement(Pos.y, Pos.x).Bldg;
				if(	CurBuilding != NULL &&
					CurBuilding->Type() != BuildingBlocker &&
					CurBuilding->Type() != BuildingCircuitBoundaryOpen &&
					CurBuilding->Type() != BuildingCircuitBoundaryBlocked &&
          !(B.Type() == BuildingCircuitBoundaryBase && CurBuilding->Type() == BuildingCircuitBoundaryBase) &&
					(!CurBuilding->OnCircuitBoard() || CurBuilding->Type() != BuildingMegaHoldAll) &&
					CurBuilding->Location() == Pos)
				{
					Vec3 DestWorldPos;
					if(ParentCircuit == NULL)
					{
						DestWorldPos = Vec3((GridCoordinateToScreenCoordinate(CurBuilding->Location()) + Coordinate(GameGridDimension, GameGridDimension)).AsVec2(), BuildingSpokeLayerZDepth);
					}
					else
					{
            if(ActiveCircuit)
            {
						  DestWorldPos = Vec3((CircuitCoordinateToScreenCoordinate(CurBuilding->Location()) + Coordinate(GameGridDimension, GameGridDimension)).AsVec2(), BuildingSpokeLayerZDepth);
            }
            else
            {
              //DestWorldPos = Vec3((CircuitCoordinateToScreenCoordinate(CurBuilding->Location()) + Coordinate(GameGridDimension, GameGridDimension)).AsVec2(), BuildingSpokeLayerZDepth);
              //DestWorldPos = Vec3(GridCoordinateToScreenCoordinate(ParentCircuit->Location()).AsVec2() + CurBuilding->Location().AsVec2() * GameGridDimension * ScaleFactor, BuildingSpokeLayerZDepth);
              DestWorldPos = Vec3((GridCoordinateToScreenCoordinate(ParentCircuit->Location()) +
                            Coordinate((CurBuilding->Location().x + 1) * GameGridDimension / 7, (CurBuilding->Location().y + 1) * GameGridDimension / 7)).AsVec2(),
                            BuildingSpokeLayerZDepth);
            }
					}

          Vec3 Diff = Vec3::Normalize(DestWorldPos - SourceWorldPos) * SpokeLength * ScaleFactor;
          Vec3 Middle = (SourceWorldPos + DestWorldPos) * 0.5f;
					GetThickLineVertices(Middle - Diff, SpokeRadius * SpokeRadiusScaleFactor, Middle + Diff, SpokeRadius * SpokeRadiusScaleFactor, _SpokeMesh.Vertices()[0].Pos, _SpokeMesh.Vertices()[1].Pos, _SpokeMesh.Vertices()[2].Pos, _SpokeMesh.Vertices()[3].Pos);

          _WireConnectorTexture[ConnectorIndex].Set(0);
					_SpokeMesh.Render();
				}
			}
		}
	}
}

void ThatWhichRendersGames::RenderButtonBackground(GraphicsDevice &GD, Game &G, Matrix4 &InverseViewportMatrix,
												   const Coordinate &ScreenCoordinate, float ScaleFactor, float ZDepth)
{
	Matrix4 Transform = Matrix4::Scaling(Vec3(ScaleFactor, ScaleFactor, 1.0f)) * Matrix4::Translation(Vec3(float(ScreenCoordinate.x), float(ScreenCoordinate.y), ZDepth)) * InverseViewportMatrix;
	
	_BorderTexture.Set(0);
	GD.LoadMatrix(MatrixController(Transform));
	_BuildingMesh.Render();
}

void ThatWhichRendersGames::RenderBlocker(GraphicsDevice &GD, Game &G, Matrix4 &InverseViewportMatrix, const Coordinate &GridCoord, bool Render2x2)
{
  UINT MaxYOffset = 2, MaxXOffset = 2;
  if(!Render2x2)
  {
    MaxXOffset = 1;
    MaxYOffset = 1;
  }

  EnableAlphaBlending(GD, AlphaBlendingModeBuilding);
  for(UINT YOffset = 0; YOffset < MaxYOffset; YOffset++)
  {
    for(UINT XOffset = 0; XOffset < MaxXOffset; XOffset++)
    {
      bool LeftBlocked = false;
      bool RightBlocked = false;
      bool TopBlocked = false;
      bool BottomBlocked = false;
      bool TopLeftBlocked = false;
      bool TopRightBlocked = false;
      bool BottomLeftBlocked = false;
      bool BottomRightBlocked = false;

      Coordinate BaseOffset = GridCoord + Coordinate(XOffset, YOffset);
      for(UINT OffsetIndex = 0; OffsetIndex < 8; OffsetIndex++)
      {
        Coordinate CurOffset = BaseOffset;
        if(OffsetIndex == 0) CurOffset = CurOffset + Coordinate(-1, 0);
        if(OffsetIndex == 1) CurOffset = CurOffset + Coordinate(1, 0);
        if(OffsetIndex == 2) CurOffset = CurOffset + Coordinate(0, -1);
        if(OffsetIndex == 3) CurOffset = CurOffset + Coordinate(0, 1);
        if(OffsetIndex == 4) CurOffset = CurOffset + Coordinate(-1, -1);
        if(OffsetIndex == 5) CurOffset = CurOffset + Coordinate(1, -1);
        if(OffsetIndex == 6) CurOffset = CurOffset + Coordinate(-1, 1);
        if(OffsetIndex == 7) CurOffset = CurOffset + Coordinate(1, 1);

        Grid<BoardCell> &Cells = G.GetBoard().Cells();
        bool CellBlocked = false;
        if(Cells.ValidCoordinates(CurOffset.x, CurOffset.y))
        {
          if(_EffectivelyBlockedGrid.GetElement(CurOffset.y, CurOffset.x))
          {
            CellBlocked = true;
          }
          else
          {
            Building *CurBuilding = Cells.GetElement(CurOffset.y, CurOffset.x).Bldg;
            if(CurBuilding != NULL && CurBuilding->Type() == BuildingBlocker)
            {
              CellBlocked = true;
            }
          }
        }
        else
        {
          CellBlocked = true;
        }
        if(CellBlocked)
        {
          if(OffsetIndex == 0) LeftBlocked = true;
          if(OffsetIndex == 1) RightBlocked = true;
          if(OffsetIndex == 2) TopBlocked = true;
          if(OffsetIndex == 3) BottomBlocked = true;
          if(OffsetIndex == 4) TopLeftBlocked = true;
          if(OffsetIndex == 5) TopRightBlocked = true;
          if(OffsetIndex == 6) BottomLeftBlocked = true;
          if(OffsetIndex == 7) BottomRightBlocked = true;
        }
      }
      
      UINT Index = 8;
      if(LeftBlocked && !RightBlocked && TopBlocked && !BottomBlocked) Index = 0;
      else if(LeftBlocked && !RightBlocked && !TopBlocked && BottomBlocked) Index = 1;
      else if(!LeftBlocked && RightBlocked && TopBlocked && !BottomBlocked) Index = 2;
      else if(!LeftBlocked && RightBlocked && !TopBlocked && BottomBlocked) Index = 3;
      else if(!LeftBlocked && RightBlocked && TopBlocked && BottomBlocked)
      {
        Index = 4;
        if(!TopRightBlocked) Index = 13;
        if(!BottomRightBlocked) Index = 14;
      }
      else if(LeftBlocked && !RightBlocked && TopBlocked && BottomBlocked)
      {
        Index = 5;
        if(!TopLeftBlocked) Index = 15;
        if(!BottomLeftBlocked) Index = 16;
      }
      else if(LeftBlocked && RightBlocked && !TopBlocked && BottomBlocked)
      {
        Index = 6;
        if(!BottomLeftBlocked) Index = 17;
        if(!BottomRightBlocked) Index = 18;
      }
      else if(LeftBlocked && RightBlocked && TopBlocked && !BottomBlocked)
      {
        Index = 7;
        if(!TopLeftBlocked) Index = 19;
        if(!TopRightBlocked) Index = 20;
      }
      else if(LeftBlocked && RightBlocked && !TopBlocked && !BottomBlocked)
      {
        Index = 21;
      }

      else if(!LeftBlocked && !RightBlocked && TopBlocked && BottomBlocked)
      {
        Index = 22;
      }


      if(Index == 8)
      {
        if(!TopLeftBlocked) Index = 9;
        else if(!TopRightBlocked) Index = 10;
        else if(!BottomLeftBlocked) Index = 11;
        else if(!BottomRightBlocked) Index = 12;
      }

      Coordinate ScreenCoordinate = GridCoordinateToScreenCoordinate(BaseOffset);
      Matrix4 BuildingTransform = Matrix4::Translation(Vec3(float(ScreenCoordinate.x), float(ScreenCoordinate.y), BlockerLayerZDepth)) * InverseViewportMatrix;
      GD.LoadMatrix(MatrixController(BuildingTransform));
      _BoundaryTextures[Index].Set(0);
      _BorderMesh.Render();
    }
  }
  DisableAlphaBlending(GD);
}

void ThatWhichRendersGames::RenderBuilding(GraphicsDevice &GD, Game &G, Matrix4 &InverseViewportMatrix, BuildingType Type,
										   const Coordinate &ScreenCoordinate, int ChargeLevel, float ScaleFactor, 
										   float WireZDepth, float BuildingZDepth, float PreferenceZDepth, UINT PreferenceLevel, bool ForegroundLayer)
{
    if(Type == BuildingCircuitBoundaryBlocked && ScaleFactor < 1.0f)
    {
        return;
    }

	Matrix4 WireTransform = Matrix4::Scaling(Vec3(ScaleFactor, ScaleFactor, 1.0f)) * Matrix4::Translation(Vec3(float(ScreenCoordinate.x), float(ScreenCoordinate.y), WireZDepth)) * InverseViewportMatrix;
	Matrix4 BuildingTransform = Matrix4::Scaling(Vec3(ScaleFactor, ScaleFactor, 1.0f)) * Matrix4::Translation(Vec3(float(ScreenCoordinate.x), float(ScreenCoordinate.y), BuildingZDepth)) * InverseViewportMatrix;

    if(ForegroundLayer)
    {
        EnableAlphaBlending(GD, AlphaBlendingModeBuilding);
    }
    else
    {
        if(PreferenceLevel != MaxPreferenceLevel && ScaleFactor >= 1.0f)
        {
            EnableAlphaBlending(GD, AlphaBlendingModePreference);

            Matrix4 PreferenceTransform = Matrix4::Scaling(ScaleFactor) * Matrix4::Translation(Vec3(float(ScreenCoordinate.x), float(ScreenCoordinate.y), PreferenceZDepth)) * InverseViewportMatrix;

            GD.LoadMatrix(MatrixController(PreferenceTransform));
            _ChargePreferenceTextures[PreferenceLevel].Set(0);

            _BuildingMesh.Render();
        }

        EnableAlphaBlending(GD, AlphaBlendingModeBuilding);

        if( Type != BuildingCircuitBoundaryOpen &&
        Type != BuildingCircuitBoundaryBlocked &&
        Type != BuildingBlocker)
        {
            _WireTexture.Set(0);
            GD.LoadMatrix(MatrixController(WireTransform));
            _BuildingMesh.Render();
        }
    }

	if(ChargeLevel == -1)
	{
		if(Type == BuildingCircuitBoundaryBase && ScaleFactor < 1.0f)
		{
			_CircuitBoundaryBaseGridVersion.Set(0);
		}
		else
		{
			_BuildingInfo[Type].Tex.Set(0);
		}
	}
	else
	{
		_BuildingInfo[Type].PerChargeTex[ChargeLevel].Set(0);
	}

	GD.LoadMatrix(MatrixController(BuildingTransform));
	_BuildingMesh.Render();

	DisableAlphaBlending(GD);
}

void ThatWhichRendersGames::RenderCircuitHold(GraphicsDevice &GD, Game &G, Matrix4 &InverseViewportMatrix, const Coordinate &ScreenCoordinate, bool OnCircuitBoard)
{
	float ScaleFactor = 1.0f;
	if(!OnCircuitBoard)
	{
		ScaleFactor = 1.0f / 7.0f;
	}
	Matrix4 Transform = Matrix4::Scaling(ScaleFactor) * Matrix4::Translation(Vec3(float(ScreenCoordinate.x), float(ScreenCoordinate.y), BuildingLayerZDepth)) * InverseViewportMatrix;
	
	EnableAlphaBlending(GD, AlphaBlendingModeBuilding);

	GD.LoadMatrix(MatrixController(Transform));
	_CircuitHoldTexture.Set(0);
	
	_CircuitHoldMesh.Render();

	DisableAlphaBlending(GD);
}

void ThatWhichRendersGames::RenderDesktop(GraphicsDevice &GD, Game &G, Matrix4 &InverseViewportMatrix)
{
  Matrix4 Transform = Matrix4::Translation(Vec3(0.0f, 0.0f, DesktopBackgroundLayerZDepth)) * InverseViewportMatrix;
	
	GD.LoadMatrix(MatrixController(Transform));
	_DesktopTexture.Set(0);
	
	_DesktopMesh.Render();
}

void ThatWhichRendersGames::RenderBorder(GraphicsDevice &GD, Game &G, Matrix4 &InverseViewportMatrix, Coordinate &ScreenCoordinate, bool DoubleSized)
{
	Matrix4 Transform = Matrix4::Translation(Vec3(float(ScreenCoordinate.x), float(ScreenCoordinate.y), CircuitBackgroundLayerZDepth)) * InverseViewportMatrix;
	if(DoubleSized)
	{
		Transform = Matrix4::Scaling(Vec3(2.0f, 2.0f, 1.0f)) * Transform;
	}
	
	GD.LoadMatrix(MatrixController(Transform));
	_BorderTexture.Set(0);
	
	_BorderMesh.Render();
}

void ThatWhichRendersGames::RenderMask(GraphicsDevice &GD, Game &G, Matrix4 &InverseViewportMatrix, Coordinate &ScreenCoordinate)
{
	Matrix4 Transform = Matrix4::Translation(Vec3(float(ScreenCoordinate.x), float(ScreenCoordinate.y), MaskLayerZDepth)) * InverseViewportMatrix;
	
	EnableAlphaBlending(GD, AlphaBlendingModeHalfBlend);

	GD.LoadMatrix(MatrixController(Transform));
	_WhiteTexture.Set(0);
	
	_MaskMesh.Render();

	DisableAlphaBlending(GD);
}

void ThatWhichRendersGames::RenderBuildable(GraphicsDevice &GD, Game &G, Matrix4 &InverseViewportMatrix, Coordinate &ScreenCoordinate)
{
    Matrix4 Transform = Matrix4::Translation(Vec3(float(ScreenCoordinate.x), float(ScreenCoordinate.y), BuildableLayerZDepth)) * InverseViewportMatrix;
	
	GD.LoadMatrix(MatrixController(Transform));
	_BuildableTexture.Set(0);
	
	EnableAlphaBlending(GD, AlphaBlendingModeBuilding);

	_BorderMesh.Render();

	DisableAlphaBlending(GD);
}

void ThatWhichRendersGames::RenderSelector(GraphicsDevice &GD, Game &G, Matrix4 &InverseViewportMatrix, Coordinate &ScreenCoordinate, TextureType &Tex)
{
	Matrix4 Transform = Matrix4::Translation(Vec3(float(ScreenCoordinate.x), float(ScreenCoordinate.y), SelectorLayerZDepth)) * InverseViewportMatrix;
	
	GD.LoadMatrix(MatrixController(Transform));
	Tex.Set(0);
	
	EnableAlphaBlending(GD, AlphaBlendingModeHalfBlend);

	_BuildingMesh.Render();

	DisableAlphaBlending(GD);
}

void ThatWhichRendersGames::RenderTooltip(GraphicsDevice &GD, Game &G, Matrix4 &InverseViewportMatrix, Rectangle2i &Region,
										  const Coordinate &WindowDimensions, const Coordinate &ExpectedWindowDimensions,
										  const String &Text, bool RenderBackdrop)
{
	Coordinate BoundaryDimension(20, 20);
	Rectangle2 TextRect = Rectangle2((Region.TopLeft + BoundaryDimension).AsVec2(), (Region.BottomRight - BoundaryDimension).AsVec2());
	TextRect = RemapRectangle2FromExpectedToActual(TextRect, WindowDimensions, ExpectedWindowDimensions);
	GD.DrawString(Text, TextRect, RGBColor(250, 240, 245));

	if(RenderBackdrop)
	{
		Matrix4 Transform = Matrix4::Translation(Vec3(Region.TopLeft.AsVec2(), ToolTipLayerZDepth)) * InverseViewportMatrix;
		
		GD.LoadMatrix(MatrixController(Transform));
		_WhiteTexture.SetNull(0);
		EnableAlphaBlending(GD, AlphaBlendingModeToolTip);
		_TooltipMesh.Render();
		DisableAlphaBlending(GD);
	}
}

void ThatWhichRendersGames::RenderCircuit(GraphicsDevice &GD, Game &G, GameUI &UI, Matrix4 &InverseViewportMatrix, Building &B, bool ForegroundLayer)
{
  Grid<BoardCell> &CircuitGrid = B.CircuitBoard().Cells();
  Coordinate GridCoordinate = GridCoordinateToScreenCoordinate(B.Location());
	for(UINT y = 0; y < CircuitGrid.Rows(); y++)
	{
		for(UINT x = 0; x < CircuitGrid.Cols(); x++)
		{
			Building *CurBuilding = CircuitGrid.GetElement(y, x).Bldg;
			
			if(CurBuilding != NULL && CurBuilding->Location() == Coordinate(x, y))
			{
        if(ForegroundLayer)
        {
          int ChargeLevel = -1;
			    if(CurBuilding->IsAmplifierTower() || CurBuilding->IsHoldClass())
          {
            ChargeLevel = CurBuilding->AbsorbedCharge();
          }
          if(CurBuilding->IsForegroundLayer())
          {
            RenderBuilding(GD, G, InverseViewportMatrix, CurBuilding->Type(), GridCoordinate + Coordinate(x * GameGridDimension / 7, y * GameGridDimension / 7),
									         ChargeLevel, 1.0f / 7.0f, BuildingWireBaseLayerZDepth - 0.1f, BuildingLayerZDepth - 0.1f, BuildingPreferenceZDepth - 0.1f, MaxPreferenceLevel, true);
          }
        }
        else
        {
          RenderBuildingSpokes(GD, G, InverseViewportMatrix, *CurBuilding, false);
				  if(CurBuilding->IsMegaHold())
				  {
					  if(x == 2 && y == 2)
					  {
						  RenderCircuitHold(GD, G, InverseViewportMatrix, GridCoordinate + Coordinate(x * GameGridDimension / 7, y * GameGridDimension / 7), false);
					  }
				  }
				  else
				  {
					  bool RenderFadeMask = false;
					  if(CurBuilding->IsCircuitBoundary())
					  {
						  Building *Proxy = CurBuilding->GetCircuitBoundaryDestination(G);
						  if(Proxy == NULL || Proxy->Type() == BuildingBlocker)
						  {
							  RenderFadeMask = true;
						  }
					  }
					  /*if(RenderFadeMask)
					  {
						  RenderBuilding(GD, G, InverseViewportMatrix, BuildingCircuitBoundaryOpen, GridCoordinate + Coordinate(x * GameGridDimension / 7, y * GameGridDimension / 7),
									     ChargeLevel, 1.0f / 7.0f, BuildingWireBaseLayerZDepth - 0.1f, BuildingLayerZDepth - 0.1f, BuildingPreferenceZDepth - 0.1f, MaxPreferenceLevel);
					  }
					  else*/
            if(CurBuilding->IsForegroundLayer())
            {
              RenderBuilding(GD, G, InverseViewportMatrix, BuildingWire, GridCoordinate + Coordinate(x * GameGridDimension / 7, y * GameGridDimension / 7),
									     -1, 1.0f / 7.0f, BuildingWireBaseLayerZDepth - 0.1f, BuildingLayerZDepth - 0.1f, BuildingPreferenceZDepth - 0.1f, MaxPreferenceLevel, false);
            }
            else
					  {
						  RenderBuilding(GD, G, InverseViewportMatrix, CurBuilding->Type(), GridCoordinate + Coordinate(x * GameGridDimension / 7, y * GameGridDimension / 7),
									     -1, 1.0f / 7.0f, BuildingWireBaseLayerZDepth - 0.1f, BuildingLayerZDepth - 0.1f, BuildingPreferenceZDepth - 0.1f, MaxPreferenceLevel, false);
					  }
				  }
        }
			}
		}
	}
}

void ThatWhichRendersGames::UpdateEffectiveBlockedGrid(Game &G)
{
  Grid<BoardCell> &BoardCells = G.GetBoard().Cells();
  for(UINT y = 0; y < BoardCells.Rows(); y++)
  {
	  for(UINT x = 0; x < BoardCells.Cols(); x++)
	  {
      _EffectivelyBlockedGrid.GetElement(y, x) = false;
    }
  }

  for(UINT Pass = 0; Pass < 2; Pass++)
  {
    for(UINT y = 0; y < BoardCells.Rows(); y++)
	  {
		  for(UINT x = 0; x < BoardCells.Cols(); x++)
		  {
        if(!_EffectivelyBlockedGrid.GetElement(y, x) && BoardCells.GetElement(y, x).Bldg == NULL)
        {
          bool LeftBlocker = false;
          bool RightBlocker = false;
          bool TopBlocker = false;
          bool BottomBlocker = false;

          for(UINT Index = 0; Index < 4; Index++)
          {
            Coordinate CurCoord;
            bool CurCellBlocker = false;
            if(Index == 0) CurCoord = Coordinate(x + 0, y - 1);
            if(Index == 1) CurCoord = Coordinate(x + 0, y + 1);
            if(Index == 2) CurCoord = Coordinate(x - 1, y + 0);
            if(Index == 3) CurCoord = Coordinate(x + 1, y + 0);
            if(BoardCells.ValidCoordinates(CurCoord.y, CurCoord.x))
            {
              Building *CurBuilding = BoardCells.GetElement(CurCoord.y, CurCoord.x).Bldg;
              if((CurBuilding != NULL && CurBuilding->Type() == BuildingBlocker) || _EffectivelyBlockedGrid.GetElement(CurCoord.y, CurCoord.x))
              {
                CurCellBlocker = true;
              }
            }
            else
            {
              CurCellBlocker = true;
            }
            if(CurCellBlocker)
            {
              if(Index == 0) LeftBlocker = true;
              if(Index == 1) RightBlocker = true;
              if(Index == 2) TopBlocker = true;
              if(Index == 3) BottomBlocker = true;
            }
          }
          if((LeftBlocker && RightBlocker) ||
             (TopBlocker && BottomBlocker))
          {
            _EffectivelyBlockedGrid.GetElement(y, x) = true;
          }
        }
		  }
	  }
  }
}

void ThatWhichRendersGames::Render(GraphicsDevice &GD, Game &G, GameUI &UI, Matrix4 &InverseViewportMatrix, const Coordinate &WindowDimensions, const Coordinate &ExpectedWindowDimensions)
{
    if(_Backdrop.ReSizeToBackBuffer(GD.CastD3D9()) || G.QueryAndResetGameActionTaken())
    {
        {
            D3DProtectRenderTarget RenderTargetProtector(GD.CastD3D9().GetDevice(), false, true);
            _Backdrop.SetAsRenderTarget(GD.CastD3D9());
            UpdateBackdrop(GD, G, UI, InverseViewportMatrix, WindowDimensions, ExpectedWindowDimensions);
            _Backdrop.CopySurfaceToOffScreen(GD.CastD3D9());
        }
        GD.CastD3D9().GetDevice()->Clear( 0, NULL, D3DCLEAR_ZBUFFER, 0, 1.0f, 0 );
    }
  
    _Backdrop.CopyOffScreenToBackBuffer(GD.CastD3D9());
    UpdateForeground(GD, G, UI, InverseViewportMatrix, WindowDimensions, ExpectedWindowDimensions);
}

void ThatWhichRendersGames::UpdateForeground(GraphicsDevice &GD, Game &G, GameUI &UI, Matrix4 &InverseViewportMatrix, const Coordinate &WindowDimensions, const Coordinate &ExpectedWindowDimensions)
{
    Building *SelectedGridBuilding = UI.GetSelectedGridBuilding(G);
	Building *CopiedCircuitBuilding = UI.GetCopiedCircuitBuilding(G);
	Building *SelectedCircuitBuilding = UI.GetSelectedCircuitBuilding(G);
	Building *SelectedUpgradeBuilding = UI.GetSelectedUpgradeBuilding(G);
    Grid<BoardCell> &BoardCells = G.GetBoard().Cells();

    //
	// Building layer
	//
	for(UINT BuildingIndex = 0; BuildingIndex < G.Buildings().Length(); BuildingIndex++)
	{
		Building &CurBuilding = *(G.Buildings()[BuildingIndex]);
		if(!CurBuilding.OnCircuitBoard())
		{
			int ChargeLevel = -1;
			float ZModifier = 0.0f;
			Coordinate GridCoordinate = GridCoordinateToScreenCoordinate(CurBuilding.Location());
            if(CurBuilding.IsAmplifierTower() || CurBuilding.IsHoldClass())
            {
                ChargeLevel = CurBuilding.AbsorbedCharge();
            }
			if(CurBuilding.IsForegroundLayer())
			{
				RenderBuilding(GD, G, InverseViewportMatrix, CurBuilding.Type(), GridCoordinate, ChargeLevel, 1.0f,
					             BuildingWireBaseLayerZDepth, BuildingLayerZDepth, BuildingPreferenceZDepth, CurBuilding.ChargePreference(), true);
			}

			if(CurBuilding.IsMegaHold())
			{
				RenderMegaHoldFill(GD, G, InverseViewportMatrix, CurBuilding, BuildingHoldFillZDepth);
			}

            if(CurBuilding.Type() == BuildingCircuit)
            {
                RenderCircuit(GD, G, UI, InverseViewportMatrix, CurBuilding, true);
            }
		}
	}

    if(SelectedGridBuilding != NULL && SelectedGridBuilding->Type() == BuildingCircuit)
	{
		Grid<BoardCell> &CircuitGrid = SelectedGridBuilding->CircuitBoard().Cells();
		for(UINT y = 0; y < CircuitGrid.Rows(); y++)
		{
			for(UINT x = 0; x < CircuitGrid.Cols(); x++)
			{
				Building *CurBuilding = CircuitGrid.GetElement(y, x).Bldg;
				if(CurBuilding != NULL && CurBuilding->Location() == Coordinate(x, y))
				{
					if(CurBuilding->IsMegaHold())
					{
						if(CurBuilding->Location() == Coordinate(2, 2))
						{
							RenderMegaHoldFill(GD, G, InverseViewportMatrix, *CurBuilding, BuildingHoldFillZDepth);
						}
					}
					else
					{
						int ChargeLevel = -1;
						bool RenderFadeMask = false;
            if(CurBuilding->IsAmplifierTower() || CurBuilding->IsHoldClass())
            {
              ChargeLevel = CurBuilding->AbsorbedCharge();
            }
            if(CurBuilding->IsForegroundLayer())
						{
              RenderBuilding(GD, G, InverseViewportMatrix, CurBuilding->Type(), CircuitCoordinateToScreenCoordinate(CurBuilding->Location()),
							  ChargeLevel, 1.0f, BuildingWireBaseLayerZDepth, BuildingLayerZDepth, BuildingPreferenceZDepth, CurBuilding->ChargePreference(), true);
						}
					}
				}
			}
		}
	}

  //
	// Exploding charge layer
	//
	for(UINT ExplodingChargeIndex = 0; ExplodingChargeIndex < G.ExplodingCharges().Length(); ExplodingChargeIndex++)
	{
		ExplodingCharge &CurExplodingCharge = G.ExplodingCharges()[ExplodingChargeIndex];
		RenderExplodingCharge(GD, G, InverseViewportMatrix, CurExplodingCharge, false);
		if(CurExplodingCharge.ParentCircuit != Coordinate::Invalid &&
		   CurExplodingCharge.ParentCircuit == UI._SelectedBuildingGridCoordinate)
		{
			RenderExplodingCharge(GD, G, InverseViewportMatrix, CurExplodingCharge, true);
		}
	}

	//
	// Charge layer
	//
	for(UINT ChargeIndex = 0; ChargeIndex < G.Charges().Length(); ChargeIndex++)
	{
		Charge &CurCharge = *(G.Charges()[ChargeIndex]);
		RenderCharge(GD, G, InverseViewportMatrix, CurCharge, false);
    bool SourceInvalid = CurCharge._Source.Location == Coordinate::Invalid;
		if(CurCharge._Destination.Circuit == UI._SelectedBuildingGridCoordinate &&
       CurCharge._Destination.Circuit != Coordinate::Invalid &&
       (SourceInvalid || CurCharge._Source.Circuit == UI._SelectedBuildingGridCoordinate))
		{
			RenderCharge(GD, G, InverseViewportMatrix, CurCharge, true);
		}
	}

	//
	// Placement mask layer
	//
	if(UI._CurState == GameUIStateBuildingMask)
	{
		if(UI._MouseInsideCircuit)
		{
			Building *CircuitBuilding = G.CircuitFromLocation(UI._SelectedBuildingGridCoordinate);
			const Grid<BoardCell> &Cells = CircuitBuilding->CircuitBoard().Cells();
			if(UI._CurCoordinateForBuilding.InRange(Cells.Cols() - 2, Cells.Rows() - 2))
			{
				RenderBuilding(GD, G, InverseViewportMatrix, UI._CandidateBuilding, CircuitCoordinateToScreenCoordinate(UI._CurCoordinateForBuilding), -1, 1.0f,
							   BuildingWireBaseLayerZDepth, BuildingLayerZDepth, BuildingPreferenceZDepth, 2, false);
				for(UINT XOffset = 0; XOffset <= 1; XOffset++)
				{
					for(UINT YOffset = 0; YOffset <= 1; YOffset++)
					{
						Coordinate CurCoordinate = UI._CurCoordinateForBuilding + Coordinate(XOffset, YOffset);
						const BoardCell &CurCell = Cells.GetElement(CurCoordinate.y, CurCoordinate.x);
						RGBColor CurColor = RGBColor::Red;
                        if(CurCell.Bldg == NULL)
						{
							CurColor = RGBColor::Green;
						}
						_MaskMesh.SetColor(CurColor);
						RenderMask(GD, G, InverseViewportMatrix, CircuitCoordinateToScreenCoordinate(CurCoordinate));
					}
				}
			}
		}
		else
		{
			const Grid<BoardCell> &Cells = G.GetBoard().Cells();
			if(UI._CurCoordinateForBuilding.InRange(Cells.Cols() - 2, Cells.Rows() - 2))
			{
				RenderBuilding(GD, G, InverseViewportMatrix, UI._CandidateBuilding, GridCoordinateToScreenCoordinate(UI._CurCoordinateForBuilding), -1, 1.0f,
					BuildingWireBaseLayerZDepth, BuildingLayerZDepth, BuildingPreferenceZDepth, 2, false);
				for(UINT XOffset = 0; XOffset <= 1; XOffset++)
				{
					for(UINT YOffset = 0; YOffset <= 1; YOffset++)
					{
						Coordinate CurCoordinate = UI._CurCoordinateForBuilding + Coordinate(XOffset, YOffset);
						const BoardCell &CurCell = Cells.GetElement(CurCoordinate.y, CurCoordinate.x);
						RGBColor CurColor = RGBColor::Red;
						if(CurCell.Bldg == NULL && !_EffectivelyBlockedGrid.GetElement(CurCoordinate.y, CurCoordinate.x))
						{
							CurColor = RGBColor::Green;
						}
						_MaskMesh.SetColor(CurColor);
						RenderMask(GD, G, InverseViewportMatrix, GridCoordinateToScreenCoordinate(CurCoordinate));
					}
				}
			}
		}
	}

	//
	// Selector layer
	//
	TextureType &CurSelectorTexture = Utility::Choose(_PuzzlePieceTexture, _SelectorTexture, UI._PuzzlePieceSelected);
	if(SelectedGridBuilding != NULL)
	{
		RenderSelector(GD, G, InverseViewportMatrix, GridCoordinateToScreenCoordinate(UI._SelectedBuildingGridCoordinate), CurSelectorTexture);
	}
	if(SelectedCircuitBuilding != NULL)
	{
		RenderSelector(GD, G, InverseViewportMatrix, CircuitCoordinateToScreenCoordinate(UI._SelectedBuildingCircuitCoordinate), CurSelectorTexture);
	}

	//
	// Copied circuit layer
	//
	if(CopiedCircuitBuilding != NULL)
	{
		RenderSelector(GD, G, InverseViewportMatrix, GridCoordinateToScreenCoordinate(CopiedCircuitBuilding->Location()), _CopiedTexture);
	}

	//
	// Upgrade menu
	//
	if(SelectedUpgradeBuilding != NULL && !UI._PuzzlePieceSelected)
	{
		BuildingInfo &CurInfo = G.GetBuildingInfo()[SelectedUpgradeBuilding->Type()];
		for(UINT UpgradeIndex = 0; UpgradeIndex < CurInfo.Upgrades.Length(); UpgradeIndex++)
		{
			RenderButtonBackground(GD, G, InverseViewportMatrix, UpgradeMenuToScreenCoordinate(UpgradeIndex), 1.0f, BuildingButtonBackgroundZDepth);
			RenderBuilding(GD, G, InverseViewportMatrix, CurInfo.Upgrades[UpgradeIndex], UpgradeMenuToScreenCoordinate(UpgradeIndex), -1, 1.0f,
				BuildingWireBaseLayerZDepth, BuildingLayerZDepth, BuildingPreferenceZDepth, 2, false);
            if(G.GetGameMode() == GameModeGod)
            {
                if(G.GetBuildingInfo()[CurInfo.Upgrades[UpgradeIndex]].BuildableInPuzzle)
                {
                    RenderBuildable(GD, G, InverseViewportMatrix, UpgradeMenuToScreenCoordinate(UpgradeIndex));
                }
            }
			if(SelectedUpgradeBuilding->Type() == CurInfo.Upgrades[UpgradeIndex])
			{
				RenderSelector(GD, G, InverseViewportMatrix, UpgradeMenuToScreenCoordinate(UpgradeIndex), _SelectorTexture);
			}
		}

		if(SelectedUpgradeBuilding->CanChangePreferenceLevel() && G.AllowPreferences())
		{
			for(UINT PreferenceIndex = 0; PreferenceIndex < MaxPreferenceLevel; PreferenceIndex++)
			{
				RenderButtonBackground(GD, G, InverseViewportMatrix, PreferenceMenuToScreenCoordinate(PreferenceIndex), 1.0f, BuildingButtonBackgroundZDepth);
				RenderBuilding(GD, G, InverseViewportMatrix, SelectedUpgradeBuilding->Type(), PreferenceMenuToScreenCoordinate(PreferenceIndex), -1, 1.0f,
					BuildingWireBaseLayerZDepth, BuildingLayerZDepth, BuildingPreferenceZDepth, PreferenceIndex, false);
                if(G.GetGameMode() == GameModeGod)
                {
                    if(G.GetBuildingInfo()[SelectedUpgradeBuilding->Type()].BuildableInPuzzle)
                    {
                        RenderBuildable(GD, G, InverseViewportMatrix, PreferenceMenuToScreenCoordinate(PreferenceIndex));
                    }
                }
				if(SelectedUpgradeBuilding->ChargePreference() == PreferenceIndex)
				{
					RenderSelector(GD, G, InverseViewportMatrix, PreferenceMenuToScreenCoordinate(PreferenceIndex), _SelectorTexture);
				}
			}
		}
	}

	//
	// Tower menu
	//
	for(UINT BuildingIndex = 0; BuildingIndex < BuildingCount; BuildingIndex++)
	{
		BuildingInfo &CurInfo = G.GetBuildingInfo()[BuildingIndex];
		if(CurInfo.MenuCoordinate.x != -1)
		{
			RenderButtonBackground(GD, G, InverseViewportMatrix, BuildingMenuToScreenCoordinate(CurInfo.MenuCoordinate), 1.0f, BuildingButtonBackgroundZDepth);
			RenderBuilding(GD, G, InverseViewportMatrix, BuildingType(BuildingIndex), BuildingMenuToScreenCoordinate(CurInfo.MenuCoordinate), -1, 1.0f,
				BuildingWireBaseLayerZDepth, BuildingLayerZDepth, BuildingPreferenceZDepth, 2, false);
            if(G.GetGameMode() == GameModeGod)
            {
                if(G.GetBuildingInfo()[BuildingType(BuildingIndex)].BuildableInPuzzle)
                {
                    RenderBuildable(GD, G, InverseViewportMatrix, BuildingMenuToScreenCoordinate(CurInfo.MenuCoordinate));
                }
            }
			if(UI._CurState == GameUIStateBuildingMask && UI._CandidateBuilding == BuildingType(BuildingIndex))
			{
				RenderSelector(GD, G, InverseViewportMatrix, BuildingMenuToScreenCoordinate(CurInfo.MenuCoordinate), _PuzzlePieceTexture);
			}
		}
	}

	if(UI._ToolTipActive && UI._ToolTipText.Length() > 0)
	{
		Rectangle2i Region(UI._ToolTipStart, UI._ToolTipStart + ToolTipDimension);
		RenderTooltip(GD, G, InverseViewportMatrix, Region, WindowDimensions, ExpectedWindowDimensions, UI._ToolTipText, UI._ToolTipBackdrop);
	}
}

void ThatWhichRendersGames::UpdateBackdrop(GraphicsDevice &GD, Game &G, GameUI &UI, Matrix4 &InverseViewportMatrix, const Coordinate &WindowDimensions, const Coordinate &ExpectedWindowDimensions)
{
	Building *SelectedGridBuilding = UI.GetSelectedGridBuilding(G);
	Building *CopiedCircuitBuilding = UI.GetCopiedCircuitBuilding(G);
	Building *SelectedCircuitBuilding = UI.GetSelectedCircuitBuilding(G);
	Building *SelectedUpgradeBuilding = UI.GetSelectedUpgradeBuilding(G);
  Grid<BoardCell> &BoardCells = G.GetBoard().Cells();

  UpdateEffectiveBlockedGrid(G);
  
  //
  // Desktop layer
  //
  RenderDesktop(GD, G, InverseViewportMatrix);

  //
  // Render _EffectivelyBlockedGrid
  //
  for(UINT y = 0; y < BoardCells.Rows(); y++)
	{
		for(UINT x = 0; x < BoardCells.Cols(); x++)
		{
      if(_EffectivelyBlockedGrid.GetElement(y, x))
      {
        RenderBlocker(GD, G, InverseViewportMatrix, Coordinate(x, y), false);
      }
    }
  }

	//
	// Background grid layer
	//
	for(UINT y = 0; y < BoardCells.Rows(); y++)
	{
		for(UINT x = 0; x < BoardCells.Cols(); x++)
		{
      Building *CurBuilding = BoardCells.GetElement(y, x).Bldg;
      if(!_EffectivelyBlockedGrid.GetElement(y, x) && !(CurBuilding != NULL && CurBuilding->Type() == BuildingBlocker))
      {
			  RenderBorder(GD, G, InverseViewportMatrix, GridCoordinateToScreenCoordinate(Coordinate(x, y)), false);
      }
		}
	}

    //
    // Border layer
    //
    for(int Offset = -2; Offset <= int(G.GetBoard().Cells().Rows()); Offset += 2)
    {
        RenderBlocker(GD, G, InverseViewportMatrix, Coordinate(Offset, -2), true);
        RenderBlocker(GD, G, InverseViewportMatrix, Coordinate(-2, Offset), true);
        RenderBlocker(GD, G, InverseViewportMatrix, Coordinate(Offset, G.GetBoard().Cells().Rows()), true);
        RenderBlocker(GD, G, InverseViewportMatrix, Coordinate(G.GetBoard().Cells().Rows(), Offset), true);
    }
	
	//
	// Building spoke layer
	//
	for(UINT BuildingIndex = 0; BuildingIndex < G.Buildings().Length(); BuildingIndex++)
	{
		Building &CurBuilding = *(G.Buildings()[BuildingIndex]);
		if(!CurBuilding.OnCircuitBoard())
		{
			RenderBuildingSpokes(GD, G, InverseViewportMatrix, CurBuilding, false);
		}
	}

	//
	// Building layer
	//
	for(UINT BuildingIndex = 0; BuildingIndex < G.Buildings().Length(); BuildingIndex++)
	{
		Building &CurBuilding = *(G.Buildings()[BuildingIndex]);
		if(!CurBuilding.OnCircuitBoard())
		{
			float ZModifier = 0.0f;
			Coordinate GridCoordinate = GridCoordinateToScreenCoordinate(CurBuilding.Location());
      if(CurBuilding.Type() == BuildingBlocker)
      {
        RenderBlocker(GD, G, InverseViewportMatrix, CurBuilding.Location(), true);
      }
      else
      {
        if(CurBuilding.IsForegroundLayer())
        {
          RenderBuilding(GD, G, InverseViewportMatrix, BuildingWire, GridCoordinate, -1, 1.0f,
					         BuildingWireBaseLayerZDepth, BuildingLayerZDepth, BuildingPreferenceZDepth, CurBuilding.ChargePreference(), false);
        }
        else
        {
          RenderBuilding(GD, G, InverseViewportMatrix, CurBuilding.Type(), GridCoordinate, -1, 1.0f,
					         BuildingWireBaseLayerZDepth, BuildingLayerZDepth, BuildingPreferenceZDepth, CurBuilding.ChargePreference(), false);
        }
      }
		  if(CurBuilding.Type() == BuildingCircuit)
		  {
			  RenderCircuit(GD, G, UI, InverseViewportMatrix, CurBuilding, false);
		  }
		}
	}

	if(SelectedGridBuilding != NULL && SelectedGridBuilding->Type() == BuildingCircuit)
	{
		Grid<BoardCell> &CircuitGrid = SelectedGridBuilding->CircuitBoard().Cells();
		for(UINT y = 0; y < CircuitGrid.Rows(); y++)
		{
			for(UINT x = 0; x < CircuitGrid.Cols(); x++)
			{
				if(x <= 1 || x >= CircuitGrid.Cols() - 2 ||
				   y <= 1 || y >= CircuitGrid.Rows() - 2)
				{
					if(x / 2 * 2 == x && y / 2 * 2 == y)
					{
						RenderBorder(GD, G, InverseViewportMatrix, CircuitCoordinateToScreenCoordinate(Coordinate(x, y)), true);
					}
				}
				else
				{
					RenderBorder(GD, G, InverseViewportMatrix, CircuitCoordinateToScreenCoordinate(Coordinate(x, y)), false);
				}
			}
		}

		for(UINT y = 0; y < CircuitGrid.Rows(); y++)
		{
			for(UINT x = 0; x < CircuitGrid.Cols(); x++)
			{
				Building *CurBuilding = CircuitGrid.GetElement(y, x).Bldg;
				if(CurBuilding != NULL && CurBuilding->Location() == Coordinate(x, y) && !CurBuilding->IsMegaHold())
				{
					RenderBuildingSpokes(GD, G, InverseViewportMatrix, *CurBuilding, true);
				}
			}
		}

		for(UINT y = 0; y < CircuitGrid.Rows(); y++)
		{
			for(UINT x = 0; x < CircuitGrid.Cols(); x++)
			{
				Building *CurBuilding = CircuitGrid.GetElement(y, x).Bldg;
				if(CurBuilding != NULL && CurBuilding->Location() == Coordinate(x, y))
				{
					if(CurBuilding->IsMegaHold())
					{
						if(CurBuilding->Location() == Coordinate(2, 2))
						{
							RenderCircuitHold(GD, G, InverseViewportMatrix, CircuitTopLeft + Coordinate(GameGridDimension * 2, GameGridDimension * 2), true);
						}
					}
					else
					{
                        bool RenderFadeMask = false;
                        if(CurBuilding->IsCircuitBoundary())
                        {
                            Building *Proxy = CurBuilding->GetCircuitBoundaryDestination(G);
                            if(Proxy == NULL || Proxy->Type() == BuildingBlocker)
                            {
                            RenderFadeMask = true;
                            }
                        }
                        if(CurBuilding->IsForegroundLayer())
                        {
                            RenderBuilding(GD, G, InverseViewportMatrix, BuildingWire, CircuitCoordinateToScreenCoordinate(CurBuilding->Location()),
                                -1, 1.0f, BuildingWireBaseLayerZDepth, BuildingLayerZDepth, BuildingPreferenceZDepth, CurBuilding->ChargePreference(), false);
                        }
                        else
                        {
                            RenderBuilding(GD, G, InverseViewportMatrix, CurBuilding->Type(), CircuitCoordinateToScreenCoordinate(CurBuilding->Location()),
                                -1, 1.0f, BuildingWireBaseLayerZDepth, BuildingLayerZDepth, BuildingPreferenceZDepth, CurBuilding->ChargePreference(), false);
                        }
                        
                        if(RenderFadeMask)
                        {
                            RenderSelector(GD, G, InverseViewportMatrix, CircuitCoordinateToScreenCoordinate(CurBuilding->Location()), _WhiteTexture);
                            RenderSelector(GD, G, InverseViewportMatrix, CircuitCoordinateToScreenCoordinate(CurBuilding->Location()), _WhiteTexture);
                        }
                    }
				}
			}
		}
	}
}

void ThatWhichRendersGames::EnableAlphaBlending(GraphicsDevice &GD, AlphaBlendingModeType Type)
{
	GD.CastD3D9().GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	GD.CastD3D9().GetDevice()->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	if(Type == AlphaBlendingModeHalfBlend)
	{
		GD.CastD3D9().GetDevice()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_BLENDFACTOR);
		GD.CastD3D9().GetDevice()->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_BLENDFACTOR);
		GD.CastD3D9().GetDevice()->SetRenderState(D3DRS_BLENDFACTOR, D3DCOLOR_ARGB(128, 128, 128, 128));
	}
	else if(Type == AlphaBlendingModePreference)
	{
		GD.CastD3D9().GetDevice()->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		GD.CastD3D9().GetDevice()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	}
	else if(Type == AlphaBlendingModeBuilding)
	{
		GD.CastD3D9().GetDevice()->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_SRCALPHA);
		GD.CastD3D9().GetDevice()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_INVSRCALPHA);
	}
	else if(Type == AlphaBlendingModeToolTip)
	{
		GD.CastD3D9().GetDevice()->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_SRCALPHA);
		GD.CastD3D9().GetDevice()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_INVSRCALPHA);
	}
}

void ThatWhichRendersGames::DisableAlphaBlending(GraphicsDevice &GD)
{
	GD.CastD3D9().GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	GD.CastD3D9().GetDevice()->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
}
