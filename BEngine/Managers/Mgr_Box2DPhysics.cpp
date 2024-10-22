#include "Mgr_Box2DPhysics.h"

void Mgr_Box2DPhysics::CreateNewWorldData(std::string& worldName, b2Vec2& initGravity, int velIters, int posIters)
{
	WorldPair nP;
	nP.first = worldName;
	nP.second = std::make_shared<Box2D_WorldData>(initGravity, velIters, posIters);
	m_Worlds.insert(std::move(nP));
}

Box2D_WorldData* Mgr_Box2DPhysics::GetWorldData(const std::string key)
{
	//Attempt to find world data via key
	WorldMap::iterator it = m_Worlds.find(key);
	//If found, return world data pointer
	if (it != m_Worlds.end())
		return it->second.get();
	//No data found, return nullptr
	return nullptr;
}
