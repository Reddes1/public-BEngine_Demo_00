#include "Box2D_UserData_Interface.h"

#include "Utils/Utils_Debug.h"

Box2D_UserData_Interface::Box2D_UserData_Interface(Actor2D_Interface* actor)
{
	msg_assert(actor, "Box2D_UserData_Interface(): Invalid pointer passed!");
	if(actor)
		m_AssociatedActor = actor;
}
