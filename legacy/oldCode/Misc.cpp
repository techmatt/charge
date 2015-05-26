#include "Main.h"

void ChargeOptions::SaveToFile()
{
    FileCollection FileData;
    String Data;
    {
	    ofstream File("Temp.txt");
	    File << HighestPuzzleIndexSolved << ' ';
        for(UINT Index = 0; Index < MaxSkippedLevels; Index++)
        {
            File << SkippedLevels[Index] << ' ';
        }
        File << UINT(ToolTip) << ' ';
        File << AutoAdvanceLevels << ' ';
        File << ShowFPS << ' ';
        File << PlaySounds << ' ';
        File << PlayMusic << endl;
    }
    FileData.AddAndUpdateFile("Options.txt", "Temp.txt");
    FileData.SaveAll("Options.dat");
    {
        ofstream File("Temp.txt");
    }
}

void ChargeOptions::LoadFromFile()
{
    Paused = false;
	if(Utility::FileExists("Options.dat"))
	{
        FileCollection FileData;
        FileData.LoadAll("Options.dat");
        Vector<String> Lines;
        FileData.GetFileLines("Options.txt", Lines);
        {
            ofstream File("Temp.txt");
            File << Lines[0] << endl;
        }
        {
		    ifstream File("Temp.txt");
		    UINT ToolTipTemp;
		    File >> HighestPuzzleIndexSolved;
            for(UINT Index = 0; Index < MaxSkippedLevels; Index++)
            {
                File >> SkippedLevels[Index];
            }
            File >> ToolTipTemp >> AutoAdvanceLevels >> ShowFPS >> PlaySounds >> PlayMusic;
		    ToolTip = ToolTipBehaviorType(ToolTipTemp);
        }
        {
            ofstream File("Temp.txt");
        }
	}
	else
	{
		HighestPuzzleIndexSolved = -1;
        for(UINT Index = 0; Index < MaxSkippedLevels; Index++)
        {
            SkippedLevels[Index] = -1;
        }
		ToolTip = ToolTipBehaviorImmediate;
		AutoAdvanceLevels = 1;
        ShowFPS = 0;
        PlaySounds = 1;
        PlayMusic = 1;
	}
}

bool ChargeOptions::PuzzleSkipped(UINT PuzzleIndex)
{
    for(UINT Index = 0; Index < MaxSkippedLevels; Index++)
    {
        if(PuzzleIndex == SkippedLevels[Index])
        {
            return true;
        }
    }
    return false;
}

bool ChargeOptions::PuzzleSolutionViewable(UINT PuzzleIndex)
{
    return (!PuzzleSkipped(PuzzleIndex) && int(PuzzleIndex) <= HighestPuzzleIndexSolved);
}

bool ChargeOptions::PuzzleAttemptable(UINT PuzzleIndex)
{
    return (PuzzleIndex <= HighestPuzzleIndexSolved + 4 - PuzzlesSkipped());
}

void ChargeOptions::RegisterSkippedPuzzle(UINT PuzzleIndex)
{
    for(UINT Index = 0; Index < MaxSkippedLevels; Index++)
    {
        if(SkippedLevels[Index] == -1)
        {
            SkippedLevels[Index] = PuzzleIndex;
            return;
        }
    }
    MessageBox(NULL, "No skip slots in RegisterSkippedPuzzle", "Internal error", MB_OK);
}

void ChargeOptions::RegisterPuzzleSolved(UINT PuzzleIndex)
{
    for(UINT Index = 0; Index < MaxSkippedLevels; Index++)
    {
        if(PuzzleIndex == SkippedLevels[Index])
        {
            SkippedLevels[Index] = -1;
        }
    }
    for(UINT Index = HighestPuzzleIndexSolved + 1; Index < PuzzleIndex; Index++)
    {
        if(!PuzzleSkipped(Index))
        {
            RegisterSkippedPuzzle(Index);
        }
    }
    HighestPuzzleIndexSolved = Math::Max(int(PuzzleIndex), HighestPuzzleIndexSolved);
}

UINT ChargeOptions::PuzzlesSkipped()
{
    UINT Total = 0;
    for(UINT Index = 0; Index < MaxSkippedLevels; Index++)
    {
        if(SkippedLevels[Index] != -1)
        {
            Total++;
        }
    }
    return Total;
}

BuildingInfo::BuildingInfo()
{
	Name = "None";
}

BuildingInfo::BuildingInfo(const String &DatabaseLine)
{
	const int UpgradeCount = 8;

	Vector<String> Lines;
	DatabaseLine.Partition('\t', Lines);
	Assert(Lines.Length() == 16, "Invalid database line");

	Name = Lines[0];
	MaxMP = Lines[2].ConvertToInteger();
	MenuCoordinate.x = Lines[3].ConvertToInteger();
	MenuCoordinate.y = Lines[4].ConvertToInteger();

	Hotkey = 0;
	unsigned char HotkeyChar = Lines[5][0];
	if(HotkeyChar >= 'A' && HotkeyChar <= 'Z')
	{
		Hotkey = KEY_A + HotkeyChar - 'A';
	}
	/*if(HotkeyChar >= '0' && HotkeyChar <= '9')
	{
		Hotkey = KEY_0 + HotkeyChar - '0';
	}*/
	
	if(Lines[1] == "Infinity")
	{
		MaxBuildable = 100000;
	}
	else
	{
		MaxBuildable = Lines[1].ConvertToInteger();
		if(MaxBuildable == 0)
		{
			Hotkey = 0;
			MenuCoordinate.x = -1;
			MenuCoordinate.y = -1;
		}
	}

	Circuit = false;
	if(Lines[6][0] == 'Y')
	{
		Circuit = true;
	}
	for(UINT UpgradeIndex = 0; UpgradeIndex < UpgradeCount; UpgradeIndex++)
	{
		if(Lines[7 + UpgradeIndex] != "None")
		{
			Upgrades.PushEnd(GetBuildingTypeFromString(String("Building") + Lines[7 + UpgradeIndex]));
		}
	}
	Description = Lines[15];
    BuildableInPuzzle = true;
    if(Name == "PowerSourceRed" ||
       Name == "RedChargeGoal" ||
       Name == "MegaHoldRed" ||
       Name == "Blocker")
    {
        BuildableInPuzzle = false;
    }
}

void GetThickLineVertices(const Vec3 &Pt1, float Radius1, const Vec3 &Pt2, float Radius2, Vec3 &V0, Vec3 &V1, Vec3 &V2, Vec3 &V3)
{
	Vec3 Diff = Pt2 - Pt1;
	Vec3 Offset = Vec3::Normalize(Vec3::Cross(Diff, Vec3::eZ));
	V0 = Pt1 + Radius1 * Offset;
	V1 = Pt1 - Radius1 * Offset;
	V2 = Pt2 + Radius2 * Offset;
	V3 = Pt2 - Radius2 * Offset;
}