#include "Box2D_WorldData.h"

//Utilties
#include "Utils/Utils_Debug.h"

//Engine Includes
#include "Types/ContactListener_BE_Default.h"

Box2D_WorldData::Box2D_WorldData()
	:m_World(m_DefaultGravity)
{
	//Create default listener and assign it to world
	m_ContactListener = std::make_shared<ContactListener_BE_Default>();
	m_World.SetContactListener(m_ContactListener.get());
}

Box2D_WorldData::Box2D_WorldData(b2Vec2& initGravity, int velIters, int posIters)
	:m_World(initGravity), m_VelocityIters(velIters), m_PositionIters(posIters)
{
	//Create default listener and assign it to world
	m_ContactListener = std::make_shared<ContactListener_BE_Default>();
	m_World.SetContactListener(m_ContactListener.get());
}

Box2D_WorldData::Box2D_WorldData(b2Vec2& initGravity, int velIters, int posIters, b2ContactListener* cListener)
	:m_World(initGravity), m_VelocityIters(velIters), m_PositionIters(posIters)
{
	if (cListener)
	{
		//Take over management of the resource
		m_ContactListener = std::shared_ptr<b2ContactListener>(cListener);
	}
	else
	{
		//No valid pointer, alert and create default in its place
		msg_assert(false, "Box2D_WorldData(): Invalid pointer passed, creating default contact listener!");
		m_ContactListener = std::make_shared<ContactListener_BE_Default>();
	}

	//Set listener to world
	m_World.SetContactListener(m_ContactListener.get());
}
