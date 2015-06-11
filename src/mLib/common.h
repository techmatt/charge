
#ifndef CORE_BASE_COMMON_H_
#define CORE_BASE_COMMON_H_

#ifdef _WIN32

#ifndef _SCL_SECURE_NO_WARNINGS
#define _SCL_SECURE_NO_WARNINGS
#endif

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#define DEBUG_BREAK __debugbreak()

#endif

#ifdef LINUX
#define DEBUG_BREAK assert(false)
#endif

#include <cstring>
#include <exception>
#include <iostream>
#include <sstream>
#include <vector>
#include <list>
#include <string>
#include <functional>
#include <algorithm>
#include <fstream>
#include <memory>
#include <thread>
#include <mutex>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <numeric>
#include <type_traits>
#include <array>
#include <set>
#include <utility>
#include <limits>
#include <tuple>
#include <complex>
#include <queue>
#include <iomanip>

#ifndef NOMINMAX
#define NOMINMAX
#endif

#if defined (LINUX)
#define __FUNCTION__ __func__
#ifndef __LINE__
#define __LINE__
#endif
#endif

#define FUNCTION_LINE_STRING (std::string(__FUNCTION__) + ":" + std::to_string(__LINE__))
//#define FUNCTION_LINE_STRING (std::string(__FUNCTION__))

#define MLIB_WARNING(s) warningFunctionMLIB(std::string(FUNCTION_LINE_STRING) + std::string() + ": " + std::string(s))
#define MLIB_ERROR(s) errorFunctionMLIB(std::string(FUNCTION_LINE_STRING) + ": " + std::string(s))
#define MLIB_ASSERT_STR(b,s) { if(!(b)) assertFunctionMLIB(b, std::string(FUNCTION_LINE_STRING) + ": " + std::string(s)); }
#define MLIB_ASSERT(b) { if(!(b)) assertFunctionMLIB(b, FUNCTION_LINE_STRING); }

void warningFunctionMLIB(const std::string &description);
void errorFunctionMLIB(const std::string &description);
void assertFunctionMLIB(bool statement, const std::string &description);

#ifndef SAFE_DELETE
#define SAFE_DELETE(p)       { if (p) { delete (p);     (p)=nullptr; } }
#endif

#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p) { if (p) { delete[] (p);   (p)=nullptr; } }
#endif

#ifndef SAFE_FREE
#define SAFE_FREE(p) { if (p) { free (p);   (p)=nullptr; } }
#endif

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p) { if (p) { p->Release();   (p)=nullptr; } }
#endif

#ifndef UINT
typedef unsigned int UINT;
#endif

#ifndef UCHAR
typedef unsigned char UCHAR;
#endif

#ifndef INT64
#ifdef WIN32
typedef __int64 INT64;
#else
typedef int64_t INT64;
#endif
#endif

#ifndef UINT32
#ifdef WIN32
typedef unsigned __int32 UINT32;
#else
typedef uint32_t UINT32;
#endif
#endif

#ifndef UINT64
#ifdef WIN32
typedef unsigned __int64 UINT64;
#else
typedef uint64_t UINT64;
#endif
#endif

#ifndef FLOAT
typedef float FLOAT;
#endif

#ifndef DOUBLE
typedef double DOUBLE;
#endif

#ifndef BYTE
typedef unsigned char BYTE;
#endif

#ifndef USHORT
typedef unsigned short USHORT;
#endif




#ifndef sint
typedef signed int sint;
#endif

#ifndef uint
typedef unsigned int uint;
#endif 

#ifndef slong 
typedef signed long slong;
#endif

#ifndef ulong
typedef unsigned long ulong;
#endif

#ifndef uchar
typedef unsigned char uchar;
#endif

#ifndef schar
typedef signed char schar;
#endif

#endif  // CORE_BASE_COMMON_H_
