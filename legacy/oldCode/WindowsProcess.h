#include <tlhelp32.h>

struct MemoryRange
{
	MemoryRange();
	
	bool ReadableMemory() const;
	bool StringMatch(const String &Match, UINT DataOffset) const;
	void Dump(const String &Filename) const;
	void Dump(const String &Filename, UINT OffsetStart, UINT OffsetLength) const;
	String Describe() const;

	PVOID AddressStart;
	UINT Size;
	DWORD AllocationProtection;
	DWORD State;
	DWORD Protection;
	DWORD Type;
	BYTE *Data;
};

struct WindowsModule
{
	
};

class WindowsProcess
{
public:
	WindowsProcess();
	~WindowsProcess();
	void FreeMemory();

	void Init(const PROCESSENTRY32 &ProcessEntry);
	void EnumerateMemory();
	UINT TotalAllocatedMemory();

	UINT CountTotalSearchMemoryHits(const String &Name);
	const MemoryRange* SearchMemoryForString(const String &Name, UINT &Offset);

	__forceinline const String& Name()
	{
		return _Name;
	}
	__forceinline DWORD Id()
	{
		return _Id;
	}
	__forceinline const Vector<MemoryRange>& Memory()
	{
		return _Memory;
	}

private:
	void AttachToProcess();
	void DetachFromProcess();

	HANDLE _Handle;
	String _Name;
	DWORD _Id;
	Vector<MemoryRange> _Memory;
	Vector<WindowsModule> _Modules;
};
