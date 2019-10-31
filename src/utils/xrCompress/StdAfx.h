#pragma once

//. #define MOD_COMPRESS

#include "Common/Common.hpp"
#include "xrCore/xrCore.h"

#include "lzo/lzo1x.h"

#if defined(WINDOWS)
#include <mmsystem.h>

#pragma comment(lib, "winmm")
#elif defined(LINUX)
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <unistd.h>
#include "SDL.h"
#endif


