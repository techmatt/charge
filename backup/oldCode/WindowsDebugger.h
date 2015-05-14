// VirtualQueryEx

class WindowsDebugger
{
public:
	WindowsDebugger();
	~WindowsDebugger();
	void FreeMemory();

	void EnumerateProcessList();

	Vector<WindowsProcess*>& ProcessList()
	{
		return _ProcessList;
	}

	WindowsProcess& GetProcessByName(const String &Name);

	static void ReportLastWindowsError(const String &Name);
	static String DescribeLastWindowsError();

private:
	Vector<WindowsProcess*> _ProcessList;
};