#include "FL_Member_Interface.h"

#include "Utils/Utils_Debug.h"

void FL_Member_Interface::SetMemberTypeID(MemberTypes typeID)
{
	//Member type shouldn't generally be set to this, so assert false just in case
	assert(typeID != MemberTypes::UNDEFINED);
	m_MemberTypeID = typeID;
}

float FL_Member_Interface::GetAnchorPointValueAtIndex(unsigned index)
{
	assert(index <= MAX_ANCHORS);
	return m_AnchoringPoints[index];
}