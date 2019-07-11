// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef PCH_H
#define PCH_H

// add headers that you want to pre-compile here
#include "framework.h"

using namespace DirectX;

#include "MathHelper.h"
#include "Unique_Singleton.h"

#include "Const.h"
#include "Function.h"
#include "Struct.h"

#include "d3dutil.h"
#include "GraphicDev.h"
#include "UploadBuffer.h"
#include "FrameResource.h"

#endif //PCH_H
