#include "Mgr_UI.h"

//Library Includes
#include <algorithm>	//std::sort

Mgr_UI::Mgr_UI()
{
	//Reserve comfortable amount of UI pointer space
	m_MouseColliders.reserve(50);
}

Mgr_UI::~Mgr_UI()
{
	//Clear held pointers
	m_MouseColliders.clear();
}

void Mgr_UI::Update_MouseColliders(System& sys)
{
	//Depths may have changes due to game logic, so sort from highest to lowest
	std::sort(m_MouseColliders.begin(), m_MouseColliders.end(), [](Module_UI_MouseCollider* a, Module_UI_MouseCollider* b)
	{
		return a->GetDepthLevel() > b->GetDepthLevel();
	});

	//Trigger bool for when a successful collision is found to prevent depth errors down the line
	bool collisionDetected = false;

	//Now sorted from high to low, cycle container till done
	for (auto& a : m_MouseColliders)
	{
		//Call update (runs collision check)
		a->Update_Main(sys);
		/*
			If a collision was detected, we need to forcibly disable hover flags for all lower depth UIs
			as they may have lingering flags AND we get to save on doing a collision check for each of them.
		*/
		if (collisionDetected)
			a->SetHoverState(false);
		//Check for a collision event, and if one found flag trigger for future container elements
		else if (a->IsMouseHovering())
			collisionDetected = true;
	}
}

bool Mgr_UI::AddCollider(Module_UI_MouseCollider* collider)
{
	//Search container to see if this already exists, adding it if no duplicate found
	std::vector<Module_UI_MouseCollider*>::iterator iter = std::find(m_MouseColliders.begin(), m_MouseColliders.end(), collider);
	if (iter == m_MouseColliders.end())
	{
		m_MouseColliders.push_back(collider);
		return true;
	}

	//Collider was found, so return false
	return false;
}

bool Mgr_UI::RemoveCollider(Module_UI_MouseCollider* collider)
{
	//Search container for target collider. Remove and return true if found.
	std::vector<Module_UI_MouseCollider*>::iterator iter = std::find(m_MouseColliders.begin(), m_MouseColliders.end(), collider);
	if (iter != m_MouseColliders.end())
	{
		m_MouseColliders.erase(iter);
		return true;
	}

	//No collider removed, return false
	return false;
}
