
#ifndef MLIBCORE_H_
#define MLIBCORE_H_

#ifndef _WIN32
#define LINUX
#endif

typedef unsigned int UINT;

#include "common.h"

using namespace std;

#include "utility.h"
#include "stringUtil.h"

#include "vec2.h"
#include "vec3.h"
#include "vec4.h"
#include "rect2.h"

#include "matrix4.h"

#include "RGBColor.h"
#include "grid2.h"

#include "stringUtilConvert.h"
#include "parameterFile.h"
#include "parameterTable.h"

#include "imageLoaderLodePNG.h"

#include "GLQuad.h"
#include "GLProgram.h"

#endif  // MLIBCORE_H_
