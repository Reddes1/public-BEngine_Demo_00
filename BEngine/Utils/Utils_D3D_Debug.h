//*********************************************************************************\\
// 
// Collection of tools and utilities taliored for debugging D3D/Game related tasks,
// actions and issues.
//
//*********************************************************************************\\

#pragma once

#include <windows.h>
#include <sstream>
#include <cassert>
#include <d3d12.h>
#include <dxgi1_4.h>

//
//Debugging & Error Handling
//

//Primary Error Handler + support func
inline std::wstring AnsiToWString(const std::string& str)
{
	WCHAR buffer[512];
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, buffer, 512);
	return std::wstring(buffer);
}
#ifndef ThrowIfFailed
#define ThrowIfFailed(x)                                              \
{                                                                     \
    HRESULT hr__ = (x);                                               \
    std::wstring wfn = AnsiToWString(__FILE__);                       \
    if(FAILED(hr__)) { throw DxException(hr__, L#x, wfn, __LINE__); } \
}
#endif

inline void d3dSetDebugName(IDXGIObject* obj, const char* name)
{
	if (obj)
	{
		obj->SetPrivateData(WKPDID_D3DDebugObjectName, lstrlenA(name), name);
	}
}
inline void d3dSetDebugName(ID3D12Device* obj, const char* name)
{
	if (obj)
	{
		obj->SetPrivateData(WKPDID_D3DDebugObjectName, lstrlenA(name), name);
	}
}
inline void d3dSetDebugName(ID3D12DeviceChild* obj, const char* name)
{
	if (obj)
	{
		obj->SetPrivateData(WKPDID_D3DDebugObjectName, lstrlenA(name), name);
	}
}

/*
	Makes checking if functions worked neater e.x. HR(SomeD3DFunction());
	If it fails the error checking code will get the error message, the
	line number and file name and display it all. Saves us typing the same thing over and over. The app will be stopped.
	This checking will disappear in a release build, which is more professional. Just saves typing and so stops silly bugs.
*/
#if defined(DEBUG) | defined(_DEBUG)
#ifndef HR													
inline void DXError(HRESULT hr, const char* pFileStr, int lineNum)
{
	LPSTR output;
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&output, 0, NULL);
	static char sErrBuffer[256];
	sprintf_s(sErrBuffer, 256, "FILE:%s \rLINE:%i \rERROR:%s", pFileStr, lineNum, output);
	MessageBox(0, sErrBuffer, "Error", 0);
	assert(false);
}
#define HR(x)                             \
{                                         \
	HRESULT hr = (x);                     \
	if(FAILED(hr))						  \
		DXError( hr, __FILE__, __LINE__); \
}
#endif
#else
#ifndef HR
#define HR(x) (x)
#endif
#endif

class DxException
{
public:
	DxException() = default;
	DxException(HRESULT hr, const std::wstring& functionName, const std::wstring& filename, int lineNumber);

	std::wstring ToString()const;

	HRESULT ErrorCode = S_OK;
	std::wstring FunctionName;
	std::wstring Filename;
	int LineNumber = -1;
};