#pragma once

#ifdef _DEBUG
#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")
#endif

//C
#include <assert.h>
#include <Windows.h>

//C++
#include <memory>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <chrono>

//D3D
#include <d3d11.h>
#include <d3dcompiler.h>

//WRL(for ComPtr)
#include <wrl.h>
using namespace Microsoft;
using namespace WRL;

//DirectXTK
#include <directxtk/SimpleMath.h>
using namespace DirectX;
using namespace SimpleMath;

//IMGUI
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>

//ReadData
#include "ReadData.h"

//Macros
#define CHECK(hr) { if (FAILED(hr)) std::cout << "Error Code : 0x" << std::hex << hr << " - " << std::system_category().message(hr) << '\n'; assert(SUCCEEDED(hr)); }

#define SAFE_DELETE(p) { if (p) { delete p; p = nullptr; } }
#define SAFE_DELETE_ARRAY(p) { if (p) { delete[] p; p = nullptr; } }