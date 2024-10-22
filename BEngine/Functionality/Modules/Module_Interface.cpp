#include "Module_Interface.h"

#include "Utils/Utils_Debug.h"

#include "Actors/Actor2D_Interface.h"

void Module_Interface::ReSyncWithActor(Actor2D_Interface* actor)
{
	msg_assert(actor, "ReSyncWithActor(): Invalid pointer passed!");

	if (!actor)
		return;
	m_Actor = actor;
}

bool Module_Interface::BindActor(Actor2D_Interface* actor)
{
	msg_assert(actor, "BindActor(): No actor pointer passed!");
	
	//Return if no actor pointer passed (keeping any 
	if (!actor)
		return false;

	//Bind actor
	m_Actor = actor;
	return true;
}
