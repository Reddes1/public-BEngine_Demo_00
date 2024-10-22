#include "SceneTransform.h"

void SceneTransform::Update()
{
	//Rebuild transformation matrix
	m_TransformMatrix =
		DirectX::XMMatrixRotationZ(m_Rotation) * 
		DirectX::XMMatrixScaling(m_Scale.x, m_Scale.y, 1.f) *
		DirectX::XMMatrixTranslation(m_Translation.x, m_Translation.y, 0.f);
}

void SceneTransform::FixedMove(Directions4 direction)
{
	switch (direction)
	{
	case Directions4::UP:
		//Translate scene upwards
		m_Translation.y -= m_FixedCamDistance.y;
		//If clamp enabled and scene moved below guideline, set to guideline
		if (m_ClampCamera && m_Translation.y < m_ScreenBounds.y)
			m_Translation.y = m_ScreenBounds.y;
		break;

	case Directions4::DOWN:
		//Translate scene downwards
		m_Translation.y += m_FixedCamDistance.y;
		//If clamp enabled and scene moved below guideline, set to guideline
		if (m_ClampCamera && m_Translation.y > m_ScreenBounds.w)
			m_Translation.y = m_ScreenBounds.y;
		break;

	case Directions4::LEFT:
		//Translate scene leftwards
		m_Translation.x -= m_FixedCamDistance.x;
		//If clamp enabled and scene moved below guideline, set to guideline
		if (m_ClampCamera && m_Translation.x < m_ScreenBounds.x)
			m_Translation.x = m_ScreenBounds.x;
		break;

	case Directions4::RIGHT:
		//Translate scene downwards
		m_Translation.x += m_FixedCamDistance.x;
		//If clamp enabled and scene moved below guideline, set to guideline
		if (m_ClampCamera && m_Translation.x > m_ScreenBounds.z)
			m_Translation.x = m_ScreenBounds.z;
		break;
	}
}

void SceneTransform::SmoothMove(GameTimer& gt, const XMF2& vec)
{
	//Apply translation
	m_Translation.x += (m_SmoothCamSpeed.x * vec.x) * gt.DeltaTime();
	m_Translation.y += (m_SmoothCamSpeed.y * vec.y) * gt.DeltaTime();

	//Check for boundary limiting
	if (m_ClampCamera)
	{
		//Top check
		if (m_Translation.y < m_ScreenBounds.y)
			m_Translation.y = m_ScreenBounds.y;
		//Bottom check
		else if (m_Translation.y > m_ScreenBounds.w)
			m_Translation.y = m_ScreenBounds.y;

		//Left check
		if (m_Translation.x < m_ScreenBounds.x)
			m_Translation.x = m_ScreenBounds.x;
		//Right check
		else if (m_Translation.x > m_ScreenBounds.z)
			m_Translation.x = m_ScreenBounds.z;
	}
}

void SceneTransform::AddToTranslation(float x, float y)
{
	m_Translation.x += x;
	m_Translation.y += y;
}

void SceneTransform::AddToTranslation(float x, float y, bool checkBounds)
{
	m_Translation.x += x;
	m_Translation.y += y;

	if (checkBounds)
	{
		//Left check
		if (m_Translation.x < m_ScreenBounds.x)
			m_Translation.x = m_ScreenBounds.x;
		//Right check
		else if (m_Translation.x > m_ScreenBounds.z)
			m_Translation.x = m_ScreenBounds.z;

		//Top check
		if (m_Translation.y < m_ScreenBounds.y)
			m_Translation.y = m_ScreenBounds.y;
		//Bottom check
		else if (m_Translation.y > m_ScreenBounds.w)
			m_Translation.y = m_ScreenBounds.y;
	}
}

void SceneTransform::AddToTranslationX(float x, bool checkBounds)
{
	m_Translation.x += x;
	if (checkBounds)
	{
		//Left check
		if (m_Translation.x < m_ScreenBounds.x)
			m_Translation.x = m_ScreenBounds.x;
		//Right check
		else if (m_Translation.x > m_ScreenBounds.z)
			m_Translation.x = m_ScreenBounds.z;
	}
}

void SceneTransform::AddToTranslationY(float y, bool checkBounds)
{
	m_Translation.y += y;
	if (checkBounds)
	{
		//Top check
		if (m_Translation.y < m_ScreenBounds.y)
			m_Translation.y = m_ScreenBounds.y;
		//Bottom check
		else if (m_Translation.y > m_ScreenBounds.w)
			m_Translation.y = m_ScreenBounds.y;
	}
}

void SceneTransform::AddToZoom(float zoomInc)
{
	m_Scale.x += zoomInc;
	m_Scale.y += zoomInc;
}

void SceneTransform::SetTranslation(float x, float y, bool checkBounds)
{
	m_Translation.x = x;
	m_Translation.y = y;

	if (checkBounds)
	{
		//Left check
		if (m_Translation.x < m_ScreenBounds.x)
			m_Translation.x = m_ScreenBounds.x;
		//Right check
		else if (m_Translation.x > m_ScreenBounds.z)
			m_Translation.x = m_ScreenBounds.z;

		//Top check
		if (m_Translation.y < m_ScreenBounds.y)
			m_Translation.y = m_ScreenBounds.y;
		//Bottom check
		else if (m_Translation.y > m_ScreenBounds.w)
			m_Translation.y = m_ScreenBounds.y;
	}
}

void SceneTransform::SetTranslationX(float x, bool checkBounds)
{
	m_Translation.x = x;
	if (checkBounds)
	{
		//Left check
		if (m_Translation.x < m_ScreenBounds.x)
			m_Translation.x = m_ScreenBounds.x;
		//Right check
		else if (m_Translation.x > m_ScreenBounds.z)
			m_Translation.x = m_ScreenBounds.z;
	}
}

void SceneTransform::SetTranslationY(float y, bool checkBounds)
{
	m_Translation.y = y;
	if (checkBounds)
	{
		//Top check
		if (m_Translation.y < m_ScreenBounds.y)
			m_Translation.y = m_ScreenBounds.y;
		//Bottom check
		else if (m_Translation.y > m_ScreenBounds.w)
			m_Translation.y = m_ScreenBounds.y;
	}
}

