
#include "main.h"

#ifdef WIN32
#include <Windows.h>
#else
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>
#endif

double Timer::getTime()	{
#ifdef _WIN32
    unsigned __int64 pf;
    QueryPerformanceFrequency((LARGE_INTEGER *)&pf);
    double freq_ = 1.0 / (double)pf;

    unsigned __int64 val;
    QueryPerformanceCounter((LARGE_INTEGER *)&val);
    return (val)* freq_;
#endif //_WIN32

#ifdef LINUX
    struct timeval timevalue;
    gettimeofday(&timevalue, nullptr);
    return ((UINT64)timevalue.tv_sec * 1000000ULL + (UINT64)timevalue.tv_usec) * 0.000001;
#endif //LINUX
}

namespace util
{
	//
	// These hash functions are taken from http://www.burtleburtle.net/bob/hash/doobs.html
	//

	inline void hashMix(UINT &a, UINT &b, UINT &c)
	{
		a -= b; a -= c; a ^= (c>>13);
		b -= c; b -= a; b ^= (a<<8);
		c -= a; c -= b; c ^= (b>>13);
		a -= b; a -= c; a ^= (c>>12);
		b -= c; b -= a; b ^= (a<<16);
		c -= a; c -= b; c ^= (b>>5);
		a -= b; a -= c; a ^= (c>>3);
		b -= c; b -= a; b ^= (a<<10);
		c -= a; c -= b; c ^= (b>>15);
	}

	UINT hash32(const BYTE *k, UINT length)
	{
		UINT a, b, c, LocalLength;

		/* Set up the internal state */
		LocalLength = length;
		a = b = 0x9e3779b9;  /* the golden ratio; an arbitrary value */
		c = 0x9b9773e9;

		/*---------------------------------------- handle most of the key */
		while (LocalLength >= 12)
		{
			a += (k[0] + ((UINT)k[1]<<8) + ((UINT)k[2]<<16) + ((UINT)k[3]<<24));
			b += (k[4] + ((UINT)k[5]<<8) + ((UINT)k[6]<<16) + ((UINT)k[7]<<24));
			c += (k[8] + ((UINT)k[9]<<8) + ((UINT)k[10]<<16)+ ((UINT)k[11]<<24));
			hashMix(a, b, c);
			k += 12;
			LocalLength -= 12;
		}

		/*------------------------------------- handle the last 11 bytes */
		c += length;
		switch(LocalLength)              /* all the case statements fall through */
		{
		case 11: c += ((UINT)k[10]<<24);
		case 10: c += ((UINT)k[9]<<16);
		case 9 : c += ((UINT)k[8]<<8);
			/* the first byte of c is reserved for the length */
		case 8 : b += ((UINT)k[7]<<24);
		case 7 : b += ((UINT)k[6]<<16);
		case 6 : b += ((UINT)k[5]<<8);
		case 5 : b += k[4];
		case 4 : a += ((UINT)k[3]<<24);
		case 3 : a += ((UINT)k[2]<<16);
		case 2 : a += ((UINT)k[1]<<8);
		case 1 : a += k[0];
			/* case 0: nothing left to add */
		}
		hashMix(a, b, c);
		/*-------------------------------------------- report the result */
		return c;
	}

	UINT64 hash64(const BYTE *k, UINT length)
	{
		UINT a, b, c, LocalLength;

		/* Set up the internal state */
		LocalLength = length;
		a = b = 0x9e3779b9;  /* the golden ratio; an arbitrary value */
		c = 0x9b9773e9;

		/*---------------------------------------- handle most of the key */
		while (LocalLength >= 12)
		{
			a += (k[0] + ((UINT)k[1]<<8) + ((UINT)k[2]<<16) + ((UINT)k[3]<<24));
			b += (k[4] + ((UINT)k[5]<<8) + ((UINT)k[6]<<16) + ((UINT)k[7]<<24));
			c += (k[8] + ((UINT)k[9]<<8) + ((UINT)k[10]<<16)+ ((UINT)k[11]<<24));
			hashMix(a, b, c);
			k += 12;
			LocalLength -= 12;
		}

		/*------------------------------------- handle the last 11 bytes */
		c += length;
		switch(LocalLength)              /* all the case statements fall through */
		{
		case 11: c += ((UINT)k[10]<<24);
		case 10: c += ((UINT)k[9]<<16);
		case 9 : c += ((UINT)k[8]<<8);
			/* the first byte of c is reserved for the length */
		case 8 : b += ((UINT)k[7]<<24);
		case 7 : b += ((UINT)k[6]<<16);
		case 6 : b += ((UINT)k[5]<<8);
		case 5 : b += k[4];
		case 4 : a += ((UINT)k[3]<<24);
		case 3 : a += ((UINT)k[2]<<16);
		case 2 : a += ((UINT)k[1]<<8);
		case 1 : a += k[0];
			/* case 0: nothing left to add */
		}
		hashMix(a, b, c);
		/*-------------------------------------------- report the result */
		return UINT64(c) + UINT64(UINT64(a) << 32);
	}

	bool fileExists(const std::string &filename)
	{
		std::ifstream file(filename);
		return (!file.fail());
	}

	std::string getNextLine(std::ifstream &file)
	{
		std::string line;
		getline(file, line);
		return std::string(line.c_str());
	}

	std::vector<std::string> getFileLines(std::ifstream &file, UINT minLineLength)
	{
		std::vector<std::string> result;
		std::string line;
		while(!file.fail())
		{
			getline(file, line);
			if(!file.fail() && line.length() >= minLineLength) result.push_back(line.c_str());
		}
		return result;
	}

	void saveLinesToFile(const std::vector<std::string>& lines, const std::string& filename) {
		std::ofstream file;
		file.open(filename, std::ios::out);
		for (const std::string line : lines) { file << line << std::endl; }
		file.close();
	}

    std::string directoryFromPath(const std::string &path)
    {
        return replace(path, fileNameFromPath(path), "");
    }

    std::string fileNameFromPath(const std::string &path)
    {
        return util::split(util::replace(path, '\\', '/'), '/').back();
    }

    std::string removeExtensions(const std::string &path)
    {
        return util::split(path, '.').front();
    }

	std::vector<std::string> getFileLines(const std::string &filename, UINT minLineLength)
	{
		std::ifstream file(filename);
		MLIB_ASSERT_STR(!file.fail(), std::string("Failed to open ") + filename);
		return getFileLines(file, minLineLength);
	}

	std::vector<BYTE> getFileData(const std::string &filename)
	{
		FILE *inputFile = util::checkedFOpen(filename.c_str(), "rb");
		UINT64 fileSize = util::getFileSize(filename);
		std::vector<BYTE> result(fileSize);
		util::checkedFRead(&result[0], sizeof(BYTE), fileSize, inputFile);
		fclose(inputFile);
		return result;
	}

	void copyFile(const std::string &sourceFile, const std::string &destFile)
	{
		std::vector<BYTE> data = getFileData(sourceFile);
		FILE *file = util::checkedFOpen(destFile.c_str(), "wb");
		util::checkedFWrite(&data[0], sizeof(BYTE), data.size(), file);
		fclose(file);
	}

	void renameFile(const std::string& oldFilename, const std::string& newFilename) 
	{
		rename(oldFilename.c_str(), newFilename.c_str());
	}

#ifdef WIN32
    string getWorkingDirectory()
    {
        char result[1024];
        GetCurrentDirectory(1024, result);
        return string(result) + "/";
    }

	UINT64 getFileSize(const std::string &filename)
	{
		BOOL success;
		WIN32_FILE_ATTRIBUTE_DATA fileInfo;
		success = GetFileAttributesExA(filename.c_str(), GetFileExInfoStandard, (void*)&fileInfo);
		MLIB_ASSERT_STR(success != 0, std::string("GetFileAttributesEx failed on ") + filename);
		//return fileInfo.nFileSizeLow + fileInfo.nFileSizeHigh;
		LARGE_INTEGER size;
		size.HighPart = fileInfo.nFileSizeHigh;
		size.LowPart = fileInfo.nFileSizeLow;
		return size.QuadPart;
	}

    void makeDirectory(const std::string &directory)
	{
		const std::string dir = replace(directory,'\\', '/');
		const std::vector<std::string> dirParts = split(dir, '/');
		std::string soFar = "";
		for (const std::string& part : dirParts) {
			soFar += part + "/";
			CreateDirectoryA(soFar.c_str(), nullptr);
		}
	}

    void sleep(int milliseconds)
    {
        Sleep(milliseconds);
    }
#endif

#ifdef LINUX

string getWorkingDirectory()
    {
        char result[1024];
        getcwd(result, 1024);
        return string(result) + "/";
    }

	void copyStringToClipboard(const std::string &S)
	{

	}

	std::string loadStringFromClipboard()
	{
		return "";
	}

	size_t getFileSize(const std::string &filename)
	{
		struct stat statbuf;
		int success = stat(filename.c_str(), &statbuf);
		MLIB_ASSERT_STR(success == 0, std::string("stat failed on ") + filename);
		return statbuf.st_size;
	}

	void makeDirectory(const std::string &directory)
	{
		mkdir(directory.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	}

void sleep(int milliseconds)
    {
        usleep(milliseconds * 1000);
    }
#endif
}  // namespace util
