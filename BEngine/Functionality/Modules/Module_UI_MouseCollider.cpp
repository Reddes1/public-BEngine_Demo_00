#include "Module_UI_MouseCollider.h"

//Utils
#include "Utils/Utils_General.h"

//Engine Includes
#include "Game.h"
#include "Includes/BE_All_Managers.h"
#include "Includes/BE_All_Modules.h"

void Module_UI_MouseCollider::Update_Main(System& sys)
{
	//Store current hover status
	m_WasHovered = m_IsHovering;

	//Get the mouse position
	Vec2 pos = sys.m_KBMMgr->GetMouseRelativePosition();

	//Check against appropriate collider type and if collision detected, flag it and return
	switch (m_Type)
	{
	case ColliderType::BOX:
		if (std::get<DirectX::BoundingBox>(m_Collider).Contains(pos))
		{
			m_IsHovering = true;
			m_HoverElapsed += sys.m_GameTime->DeltaTime();
			return;
		}
		break;

	case ColliderType::CIRCLE:
		if (std::get<DirectX::BoundingSphere>(m_Collider).Contains(pos))
		{
			m_IsHovering = true;
			m_HoverElapsed += sys.m_GameTime->DeltaTime();
			return;
		}
		break;
	case ColliderType::UNDEFINED:
		//Do nothing, No collider type set

		break;
	}

	//No collision detected so flag appropriately
	m_IsHovering = false;
	m_HoverElapsed = 0.f;
}

void Module_UI_MouseCollider::SyncModulePosition(Module_Interface* otherMod)
{
	if (!otherMod)
	{
		msg_assert(false, "SyncModulePosition(): No valid pointer passed!");
		return;
	}

	//Get type from the mod, recast and seek data as required
	switch (otherMod->GetType())
	{
	case ModuleTypeID::SPRITE:
	{
		//Call existing function
		SyncPositionWithSpriteData(recast_static(Module_Sprite*, otherMod)->GetSpriteData());
	}
	break;

	case ModuleTypeID::ANIMATED_SPRITE:
	{
		//Call existing function
		SyncPositionWithSpriteData(recast_static(Module_AnimatedSprite*, otherMod)->GetSpriteData());
	}
	break;

	case ModuleTypeID::UI_SF_STRING:
	{
		//Take position from SFString and override current
		SyncPositionWithStringData(recast_static(Module_UI_SFString*, otherMod)->GetStringData());
	}
	break;

	//TBD: Other appropriate modules added

	}
}

void Module_UI_MouseCollider::SyncPositionWithSpriteData(const SpriteData& sprData)
{
	switch (m_Type)
	{
	case ColliderType::BOX:
		std::get<DirectX::BoundingBox>(m_Collider).Center =
		{
			sprData.m_Position.x,
			sprData.m_Position.y,
			1.f
		};
		break;

	case ColliderType::CIRCLE:
		std::get<DirectX::BoundingSphere>(m_Collider).Center =
		{
			sprData.m_Position.x,
			sprData.m_Position.y,
			1.f
		};
	}
}

void Module_UI_MouseCollider::SyncPositionWithStringData(const SFString& strData)
{
	switch (m_Type)
	{
	case ColliderType::BOX:
		std::get<DirectX::BoundingBox>(m_Collider).Center =
		{
			strData.m_Position.x,
			strData.m_Position.y,
			1.f
		};
		break;

	case ColliderType::CIRCLE:
		std::get<DirectX::BoundingSphere>(m_Collider).Center =
		{
			strData.m_Position.x,
			strData.m_Position.y,
			1.f
		};
	}
}

void Module_UI_MouseCollider::SetAsBoundingBox(const DirectX::BoundingBox& box)
{
	m_Type = ColliderType::BOX;
	m_Collider = box;
}

void Module_UI_MouseCollider::SetAsCircle(const DirectX::BoundingSphere& circle)
{
	m_Type = ColliderType::CIRCLE;
	m_Collider = circle;
}

void Module_UI_MouseCollider::SetPosition(float x, float y)
{
	switch (m_Type)
	{
	case ColliderType::BOX:
		std::get<DirectX::BoundingBox>(m_Collider).Center = { x, y, 1.f };
		break;

	case ColliderType::CIRCLE:
		std::get<DirectX::BoundingSphere>(m_Collider).Center = { x, y, 1.f };
		break;

	}
}
