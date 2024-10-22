#include "Utils_D3D.h"		

#include <time.h>			 
#include <wrl.h>			 //ComPtr
#include <d3dcompiler.h>	 //D3DCompileFromFile

#include "Utils_D3D_Debug.h" //ThrowIfFailed


using namespace DirectX;
using namespace DirectX::SimpleMath;
using Microsoft::WRL::ComPtr;

//========================================
//DirectXTK General Functions
//========================================

void CalcStringJustification(DirectX::SpriteFont* sf, StringJustificationID index, const std::string& message, Vec2& origin)
{
	switch (index)
	{
	case StringJustificationID::LEFT_TOP:
		origin = { 0.f, 0.f };
		break;

	case StringJustificationID::LEFT:
		origin = sf->MeasureString(message.c_str()) * 0.5f;
		origin.x = 0.f;
		break;

	case StringJustificationID::LEFT_BOTTOM:
		origin = sf->MeasureString(message.c_str());
		origin.x = 0.f;
		break;

	case StringJustificationID::RIGHT_TOP:
		origin = sf->MeasureString(message.c_str());
		origin.y = 0.f;
		break;

	case StringJustificationID::RIGHT:
		origin = sf->MeasureString(message.c_str());
		origin.y *= 0.5f;
		break;

	case StringJustificationID::RIGHT_BOTTOM:
		origin = sf->MeasureString(message.c_str());
		break;

	case StringJustificationID::TOP:
		origin = sf->MeasureString(message.c_str()) * 0.5f;
		origin.y = 0.f;
		break;

	case StringJustificationID::BOTTOM:
		origin = sf->MeasureString(message.c_str());
		origin.x *= 0.5f;
		break;

	case StringJustificationID::CENTER:
		origin = sf->MeasureString(message.c_str()) * 0.5f;
		break;

	default:
		origin = { 0.f, 0.f };
	}
}

void GetNewWindowSize_16_9(AspectRatios_16by9 index, int& x, int& y)
{
	switch (index)
	{
	case AspectRatios_16by9::AR_640x360:
		x = 640; y = 360;
		break;

	case AspectRatios_16by9::AR_854x480:
		x = 854; y = 480;
		break;

	case AspectRatios_16by9::AR_960x540:
		x = 960; y = 540;
		break;

	case AspectRatios_16by9::AR_1024x576:
		x = 1024; y = 576;
		break;

	case AspectRatios_16by9::AR_1280x720:
		x = 1280; y = 720;
		break;

	case AspectRatios_16by9::AR_1366x768:
		x = 1366; y = 768;
		break;

	case AspectRatios_16by9::AR_1600x900:
		x = 1600; y = 900;
		break;

	case AspectRatios_16by9::AR_1920x1080:
		x = 1920; y = 1080;
		break;
	}
}

void GetNewWindowSize_4_3(AspectRatios_4_3 index, int& x, int& y)
{
	switch (index)
	{
	case AspectRatios_4_3::AR_320x240:
		x = 320; y = 360;
		break;

	case AspectRatios_4_3::AR_640x480:
		x = 640; y = 480;
		break;

	case AspectRatios_4_3::AR_800x600:
		x = 800; y = 600;
		break;

	case AspectRatios_4_3::AR_1024x768:
		x = 1024; y = 768;
		break;

	case AspectRatios_4_3::AR_1152x864:
		x = 1152; y = 864;
		break;

	case AspectRatios_4_3::AR_2048x1536:
		x = 2048; y = 1536;
		break;

	}
}

//========================================
//Loading Functions
//========================================

Microsoft::WRL::ComPtr<ID3DBlob> CompileShader(const std::wstring& filename, const D3D_SHADER_MACRO* defines, const std::string& entrypoint, const std::string& target)
{
	UINT compileFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)  
	compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	HRESULT hr = S_OK;

	ComPtr<ID3DBlob> byteCode = nullptr;
	ComPtr<ID3DBlob> errors;
	hr = D3DCompileFromFile(filename.c_str(), defines, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		entrypoint.c_str(), target.c_str(), compileFlags, 0, &byteCode, &errors);

	if (errors != nullptr)
		OutputDebugStringA((char*)errors->GetBufferPointer());

	ThrowIfFailed(hr);

	return byteCode;
}
