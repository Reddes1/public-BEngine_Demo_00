#include "Actor2D_Interface.h"

#include "Utils/Utils_Debug.h"

#include "Game.h"			//To get unique ID

Actor2D_Interface::Actor2D_Interface()
{
	m_CoreIDs.m_UniqueGameID = Game::GetGame()->RegisterNewGameObject();
}

void Actor2D_Interface::ReSyncModules()
{
	for (auto& a : m_Modules)
		a->ReSyncWithActor(this);
}

void Actor2D_Interface::GetDebugStr_CoreIDs(std::string& str)
{
	//Create local string
	std::string lStr;

	//Append Actor IDs
	lStr += "Actor ID: " + std::to_string(m_CoreIDs.m_ActorID);
	lStr += ", Unique Game ID: " + std::to_string(m_CoreIDs.m_UniqueGameID);
	lStr += "\nClass ID: " + std::to_string(m_CoreIDs.m_ActorID);
	lStr += ", Category ID: " + std::to_string(m_CoreIDs.m_UniqueGameID);
	//Set string for return
	str += lStr;
}

Module_Interface* Actor2D_Interface::GetModule(unsigned index)
{
	msg_assert(index <= m_Modules.size(), "GetModule(): Index OOR");

	return m_Modules[index].get();
}

Module_Interface* Actor2D_Interface::GetModule(const std::string& name)
{
	//Find the target module
	auto it = std::find_if(m_Modules.begin(),m_Modules.end(),[&name]
	(const std::shared_ptr<Module_Interface>& module) { return module->GetName() == name; });

	//Return module if found
	if (it != m_Modules.end())
		return it->get();

	//Not found, return nullptr
	return nullptr;
}

Module_Interface* Actor2D_Interface::GetModule(Module_Interface::ModuleTypeID typeID)
{
	//Find module with matching name via stl + lambda
	auto it = std::find_if(m_Modules.begin(), m_Modules.end(),
		[&typeID](const std::shared_ptr<Module_Interface>& module) { return module->GetType() == typeID; }
	);

	//Return module if found
	if (it != m_Modules.end())
		return it->get();

	//Not found, return nullptr
	return nullptr;
}

Module_Interface* Actor2D_Interface::GetModule(Module_Interface::ModuleTypeID typeID, const std::string& name)
{
	//Find module with matching name via stl + lambda
	auto it = std::find_if(m_Modules.begin(), m_Modules.end(),
		[&typeID, &name](const std::shared_ptr<Module_Interface>& module) { return module->GetType() == typeID && module->GetName() == name; }
	);

	//Return module if found
	if (it != m_Modules.end())
		return it->get();

	//Not found, return nullptr
	return nullptr;
}

std::vector<Module_Interface*> Actor2D_Interface::GetModuleList(Module_Interface::ModuleTypeID typeID)
{
	//Create mods container and reserve based on how many mods are currently stored
	std::vector<Module_Interface*> mods;
	mods.reserve(m_Modules.size());

	//Cycle container and type check, storing pointers on matches
	for (auto& a : m_Modules)
		if (a->GetType() == typeID)
			mods.push_back(a.get());

	//Return container
	return mods;
}
