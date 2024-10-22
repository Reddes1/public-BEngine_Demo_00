//*********************************************************************************\\
// 
// Suite of DirectX & Game related utilities, support types functions etc.
//
//*********************************************************************************\\

#pragma once

//Core includes + commonly used
#include "SimpleMath.h"
#include "SpriteFont.h"
#include "SpriteBatch.h"
#include "DescriptorHeap.h"
#include "CommonStates.h"
#include "Effects.h"

//================================================================================\\
//Constants
//================================================================================\\

//PI is always helpful (and tasty!)
const static float g_PI = 3.1415926535f;
//Gravity Constant
const static float g_GRAVITY = 9.81f;

//Common rotations
const static float m_ROT_45 = g_PI / 4.f;
const static float m_ROT_90 = g_PI / 2.f;
const static float m_ROT_180 = g_PI;
const static float m_ROT_270 = g_PI * 1.5f;
const static float m_ROT_360 = g_PI * 2.f;

//================================================================================\\
//Depth Strata
//0.f - 1.f Depth definitions as a general rule
//================================================================================\\

//
//Broad Strata
//

#define DEPTH_STRATA_MIN 0.00f
#define DEPTH_STRATA_LOW 0.20f
#define DEPTH_STRATA_MID 0.40f
#define DEPTH_STRATA_HIGH 0.60f
#define DEPTH_STRATA_HIGHEST 0.80f
#define DEPTH_STRATA_MAX 1.00f

//
//Generic Strata Brackets
//

#define DEPTH_STRATA_MIN_00 0.00f
#define DEPTH_STRATA_MIN_01 0.10f
#define DEPTH_STRATA_LOW_02 0.20f
#define DEPTH_STRATA_LOW_03 0.30f
#define DEPTH_STRATA_MID_04 0.40f
#define DEPTH_STRATA_MID_05 0.50f
#define DEPTH_STRATA_HIGH_06 0.60f
#define DEPTH_STRATA_HIGH_07 0.70f
#define DEPTH_STRATA_HIGHEST_08 0.80f
#define DEPTH_STRATA_HIGHEST_09 0.90f
#define DEPTH_STRATA_MAX_10 1.00f

//================================================================================\\
//General Defintions
//================================================================================\\

//Justification IDs for use with "CalculateStringJustifyPosition()".
enum class StringJustificationID : unsigned short
{
	LEFT_TOP,
	LEFT,
	LEFT_BOTTOM,
	RIGHT_TOP,
	RIGHT,
	RIGHT_BOTTOM,
	TOP,
	CENTER,
	BOTTOM
};

//Defines directions in a 4-way context
enum class Directions4 : unsigned short
{
	UP,
	RIGHT,
	DOWN,
	LEFT,
	COUNT
};

//Defined directions in a 8-way context
enum class Directions8 : unsigned short
{
	NULL_DIR,
	UP = 1,
	DOWN = 2,
	LEFT = 4,
	RIGHT = 8,
	UP_LEFT = UP + LEFT,
	UP_RIGHT = UP + RIGHT,
	DOWN_LEFT = DOWN + LEFT,
	DOWN_RIGHT = DOWN + RIGHT
};

//16:9 Aspect Ratios
enum class AspectRatios_16by9 : unsigned
{
	AR_640x360,
	AR_854x480,
	AR_960x540,
	AR_1024x576,
	AR_1280x720,
	AR_1366x768,
	AR_1600x900,
	AR_1920x1080
};

//4:3 Aspect Ratios
enum class AspectRatios_4_3 : unsigned
{
	AR_320x240,
	AR_640x480,
	AR_800x600,
	AR_1024x768,
	AR_1152x864,
	AR_2048x1536
};

/*
	Pre-defined colour combinations (based on web colours, expandable as needed)
*/
namespace Colours
{
	const DirectX::SimpleMath::Vector4 White = { 1.0f, 1.0f, 1.0f, 1.0f };
	const DirectX::SimpleMath::Vector4 Black = { 0.0f, 0.0f, 0.0f, 1.0f };
	const DirectX::SimpleMath::Vector4 Red = { 1.0f, 0.0f, 0.0f, 1.0f };
	const DirectX::SimpleMath::Vector4 Green = { 0.0f, 1.0f, 0.0f, 1.0f };
	const DirectX::SimpleMath::Vector4 Blue = { 0.0f, 0.0f, 1.0f, 1.0f };
	const DirectX::SimpleMath::Vector4 Yellow = { 1.0f, 1.0f, 0.0f, 1.0f };

	const DirectX::SimpleMath::Vector4 Cyan = { 0.0f, 1.0f, 1.0f, 1.0f };
	const DirectX::SimpleMath::Vector4 Magenta = { 1.0f, 0.0f, 1.0f, 1.0f };
	const DirectX::SimpleMath::Vector4 DimGray = { 0.412f, 0.412f, 0.412f, 1.0f };
	const DirectX::SimpleMath::Vector4 AliceBlue = { 0.941f, 0.972f, 1.0f, 1.0f };
	const DirectX::SimpleMath::Vector4 AntiqueWhite = { 0.98f, 0.922f, 0.843f, 1.0f };
	const DirectX::SimpleMath::Vector4 Aquamarine = { 0.5f, 1.0f, 0.831f, 1.0f };
	const DirectX::SimpleMath::Vector4 Crimson = { 0.863f, 0.078f, 0.235f, 1.0f };
	const DirectX::SimpleMath::Vector4 SaddleBrown = { 0.545f, 0.271f, 0.074f, 1.0f };
}



//================================================================================\\
//D3D Defintions
//================================================================================\\

//
//Redefined types
//

typedef DirectX::SimpleMath::Vector2 Vec2;
typedef DirectX::SimpleMath::Vector3 Vec3;
typedef DirectX::SimpleMath::Vector4 Vec4;

typedef DirectX::XMVECTOR XMVec4;

typedef DirectX::XMFLOAT2 XMF2;
typedef DirectX::XMFLOAT2A XMF2_Aligned;
typedef DirectX::XMFLOAT3 XMF3;
typedef DirectX::XMFLOAT3A XMF3_Aligned;
typedef DirectX::XMFLOAT4 XMF4;
typedef DirectX::XMFLOAT4A XMF4_Aligned;

typedef DirectX::XMUINT2 XMUI2;
typedef DirectX::XMUINT3 XMUI3;
typedef DirectX::XMUINT4 XMUI4;

//================================================================================\\
//DirectXTK General Functions
//================================================================================\\

//Calculates the origin point of a string using JustificationPosition input, alongside spritefont and message to position correctly
void CalcStringJustification(DirectX::SpriteFont* sf, StringJustificationID index, const std::string& message, Vec2& origin);

//Updates window dims using AR index
void GetNewWindowSize_16_9(AspectRatios_16by9 index, int& x, int& y);
void GetNewWindowSize_4_3(AspectRatios_4_3 index, int& x, int& y);

//================================================================================\\
//Resource Loading + Setup
//================================================================================\\

//Assists with the loading of shaders resources from file
Microsoft::WRL::ComPtr<ID3DBlob> CompileShader(const std::wstring& filename, const D3D_SHADER_MACRO* defines,
	const std::string& entrypoint, const std::string& target);

//Assists with the setup of constant buffer resources (to ensure they are properly 
static UINT CalcConstantBufferByteSize(UINT byteSize)
{
	// Constant buffers must be a multiple of the minimum hardware
	// allocation size (usually 256 bytes).  So round up to nearest
	// multiple of 256.  We do this by adding 255 and then masking off
	// the lower 2 bytes which store all bits < 256.
	// Example: Suppose byteSize = 300.
	// (300 + 255) & ~255
	// 555 & ~255
	// 0x022B & ~0x00ff
	// 0x022B & 0xff00
	// 0x0200
	// 512
	return (byteSize + 255) & ~255;
}

//================================================================================\\
//3D Utilities
//================================================================================\\

//Generates a vector of face normals for an object
inline void GenerateFaceNormals(std::vector<Vec3>& vertices, std::vector<Vec3>& normals, int numOfHalfQuads)
{
	for (int i = 0; i < numOfHalfQuads; ++i)
	{
		int idx = i * 3;
		DirectX::SimpleMath::Vector3 a(vertices[idx] - vertices[idx + 1]), b(vertices[idx + 2] - vertices[idx + 1]);
		a.Normalize();
		b.Normalize();
		normals[idx] = normals[idx + 1] = normals[idx + 2] = b.Cross(a);
	}
}

//update the aspect ratio and recompute the projection matrix.
inline void CreateProjectionMatrix(DirectX::SimpleMath::Matrix& projM, float fieldOfView, float aspectRatio, float nearZ, float farZ)
{
	projM = DirectX::XMMatrixPerspectiveFovLH(fieldOfView, aspectRatio, nearZ, farZ);
}

//the view matrix represents the camera
inline void CreateViewMatrix(DirectX::SimpleMath::Matrix& viewM, const Vec3& camPos, const Vec3& camTgt, const Vec3& camUp)
{
	viewM = XMMatrixLookAtLH(camPos, camTgt, camUp);
}

//Creates tranformation matrix in line with how SpriteBatch does (used in custom behaviours)
inline DirectX::XMMATRIX CreateTransformationMatrix(const Vec2& pos, const Vec2& scale, float rotation)
{
	//Create 
	DirectX::XMMATRIX translationM = DirectX::XMMatrixTranslation(pos.x, pos.y, 0.f);
	DirectX::XMMATRIX scaleM = DirectX::XMMatrixScaling(scale.x, scale.y, 1.f);
	DirectX::XMMATRIX rotationM = DirectX::XMMatrixRotationZ(rotation);

	//Combine in appropriate order and return
	DirectX::XMMATRIX transformM = DirectX::XMMatrixMultiply(DirectX::XMMatrixMultiply(scaleM, rotationM), translationM);
	return transformM;
}

//================================================================================\\
//Conversions
//================================================================================\\

//Convert from Vec3 to Vec4 (DX Types)
inline Vec4 Vec3To4(const Vec3& src, float w)
{
	return Vec4(src.x, src.y, src.z, w);
}

//Convert from Vec4 to Vec3 (DX Types)
inline Vec3 Vec4To3(const Vec4& src)
{
	return Vec3(src.x, src.y, src.z);
}

//================================================================================\\
//Other bits and pieces
//================================================================================\\

/*
	Useful for searching any array that is organised in a way where its members use a fixed
	(x, y) coordinate system, with the origin being (0, 0). An example of this may be a tile-based map
	that is organised equivlently to its x and y position. This method assumes left->right, top->down ordering.
*/
inline size_t GetArrayIndexByCoordinate(size_t x, size_t y, size_t maxX)
{
	return (x + y) + (y * maxX);
}



