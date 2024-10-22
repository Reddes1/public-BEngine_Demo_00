//*********************************************************************************\\
// 
// Wrapper + behaviour class around an XMMatrix type for use with spritebatches
// as a transform parameter (and anything else that might use it).
// Provides functionality in the context of being a 2D camera.
//
//*********************************************************************************\\

#pragma once

#include "Utils/Utils_D3D.h"		//D3D Types
#include "Timing/GameTimer.h"

class SceneTransform
{
public:

	//////////////////////////////
	/// Enums, Types & Statics ///
	//////////////////////////////

	////////////////////
	/// Constructors ///
	////////////////////

	SceneTransform() {}
	~SceneTransform() {}

	//////////////////
	/// Operations ///
	//////////////////

	//Main update window
	void Update();
	//Performs fixed move in cardinal and inter cardinal directions
	void FixedMove(Directions4 direction);
	//Performs smooth movement based on vector given
	void SmoothMove(GameTimer& gt, const XMF2& vec);

	/////////////////
	/// Accessors ///
	/////////////////

	const DirectX::XMMATRIX& GetMatrix()					 { return m_TransformMatrix; }
	const Vec4& GetScreenBounds()							 { return m_ScreenBounds; }
	const XMF2& GetTranslation()							 { return m_Translation; }
	const XMF2& GetScale()									 { return m_Scale; }
	float GetRotation()										 { return m_Rotation; }

	const XMF2& GetSmoothCameraSpeed()						 { return m_SmoothCamSpeed; }
	const XMF2& GetFixedCameraDistance()					 { return m_FixedCamDistance; }

	void AddToTranslation(float x, float y);
	void AddToTranslation(float x, float y, bool checkBounds);
	void AddToTranslationX(float x)							 {m_Translation.x += x; }
	void AddToTranslationY(float y)							 {m_Translation.y += y; }
	void AddToTranslationX(float x, bool checkBounds);
	void AddToTranslationY(float y, bool checkBounds);
	void AddToZoom(float zoomInc);

	void SetZoom(float zoom)								 { m_Scale = { zoom, zoom }; }

	void SetTranslation(float x, float y)					 { m_Translation = { x, y }; }
	void SetTranslation(float x, float y, bool checkBounds);
	void SetTranslationX(float x)							 { m_Translation.x = x; }
	void SetTranslationY(float y)							 { m_Translation.y = y; }
	void SetTranslationX(float x, bool checkBounds);
	void SetTranslationY(float y, bool checkBounds);
	void SetScaleX(float x)									 { m_Scale.x = x; }
	void SetScaleY(float y)									 { m_Scale.y = y; }
	void SetRotation(float rot)								 { m_Rotation = rot; }
															 
	void SetSmoothCameraSpeedX(float x)						 { m_SmoothCamSpeed.x = x; }
	void SetSmoothCameraSpeedY(float y)						 { m_SmoothCamSpeed.y = y; }
	void SetFixedCameraDistanceX(float x)					 { m_FixedCamDistance.x = x; }
	void SetFixedCameraDistanceY(float y)					 { m_FixedCamDistance.y = y; }

	void SetCameraClampFlag(bool clamp)						 { m_ClampCamera = clamp; }

private:

	//////////////////
	/// Operations ///
	//////////////////


	////////////
	/// Data ///
	////////////

	//Camera Transformation Matrix (what is passed to Spritebatch)
	DirectX::XMMATRIX m_TransformMatrix = DirectX::XMMatrixIdentity();
	//Defines screen clamp boundaries (in screen space -winx/y equals bottom-left corner)
	Vec4 m_ScreenBounds = { 0.f, 0.f, 1920.f, 1080.f };
	//Translation, scale and rotation stored seperately
	XMF2 m_Translation = { 0.f, 0.f };
	XMF2 m_Scale = { 1.f, 1.f };
	float m_Rotation = 0.f;
	//Smooth camera speed
	XMF2 m_SmoothCamSpeed = { 100.f, 100.f };
	//Fixed move distance (in the case of tile based games)
	XMF2 m_FixedCamDistance = { 32.f, 32.f };
	//If enabled, restrains camera to the boundaries described with m_ScreenBounds
	bool m_ClampCamera = true;
};