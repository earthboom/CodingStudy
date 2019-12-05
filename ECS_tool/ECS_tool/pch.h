#pragma once

#include <wrl.h>
#include <wrl/client.h>
#include <dxgi1_4.h>
#include <d3d12.h>
#include "Common\d3dx12.h"
#include <pix.h>
#include <DirectXColors.h>
#include <DirectXMath.h>
#include <memory>
#include <agile.h>
#include <concrt.h>
#include <iostream>

#include <map>
#include <unordered_map>
#include <vector>

#include <fbxsdk.h>

#include <ppltasks.h>
#include <synchapi.h>

#include "Content/ShaderStructures.h"
#include "Common/DeviceResources.h"

#include "Common/DirectXHelper.h"

#include "Function.h"
#include "Singleton.h"
#include "Struct.h"
#include "Const.h"

#include "ResourceMgr.h"

#include "ECS_toolMain.h"
#include "Object.h"

#if defined(_DEBUG)
#include <dxgidebug.h>
#endif
