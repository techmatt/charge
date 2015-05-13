
#ifndef MLIBCORE_H_
#define MLIBCORE_H_

#ifndef _WIN32
#define LINUX
#endif

typedef unsigned int UINT;

//
// core-base headers
//
#include "common.h"

//
// core-util headers (these are required by matrices)
//
#include "utility.h"
#include "stringUtil.h"
#include "windowsUtil.h"

//
// core-base headers
//
#include "grid2.h"

//
// core-util headers
//
#include "stringUtilConvert.h"
#include "parameterFile.h"

#include "RGBColor.h"

#endif  // MLIBCORE_H_
