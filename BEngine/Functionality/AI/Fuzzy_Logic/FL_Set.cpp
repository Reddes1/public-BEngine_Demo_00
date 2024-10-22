#include "FL_Set.h"

#include "Utils/Utils_Debug.h"

bool FL_Set::InitSet(const std::string& name, unsigned memberCount, std::vector<FL_Member_Interface::MemberTypes> types)
{
	//Check there is enough defined types
	msg_assert(types.size() >= memberCount, "InitSet(): Not enough types defined!");

	//Setup container
	m_Members.reserve(memberCount);
	m_Members.insert(m_Members.begin(), memberCount, nullptr);

	//Allocate each member based on type given and setup IDs
	for (unsigned i(0); i < memberCount; ++i)
	{
		switch (types[i])
		{
		case FL_Member_Interface::MemberTypes::TRIANGLE:
			m_Members[i] = new FL_Member_Triangle();
			m_Members[i]->SetMemberID(i);
			m_Members[i]->SetMemberTypeID(FL_Member_Interface::MemberTypes::TRIANGLE);
			break;

		case FL_Member_Interface::MemberTypes::TRAPEZOID:
			m_Members[i] = new FL_Member_Triangle();
			m_Members[i]->SetMemberID(i);
			m_Members[i]->SetMemberTypeID(FL_Member_Interface::MemberTypes::TRAPEZOID);
			break;

		case FL_Member_Interface::MemberTypes::UNDEFINED:
			msg_assert(false, "InitSet(): Invalid type defined!");
			return false;
			break;
		}
	}

	//Set name
	m_SetName = name;

	//Job done
	return true;
}

bool FL_Set::AddNewMember(const std::string& name, FL_Member_Interface::MemberTypes typeID, std::vector<float>& points)
{
	//Do some pre-lim work to make sure indexes are sorted
	size_t size = m_Members.size();	//Will be the correct index after next step
	m_Members.reserve(size + 1);
	//Insert nullptr for use in the next section
	m_Members.push_back(nullptr);

	//Setup new member based on type ID
	switch (typeID)
	{
	case FL_Member_Interface::MemberTypes::TRIANGLE:
		//Allocate and set data
		m_Members[size] = new FL_Member_Triangle();
		m_Members[size]->SetMemberID((int)size);
		m_Members[size]->SetMemberTypeID(FL_Member_Interface::MemberTypes::TRIANGLE);
		m_Members[size]->InitMember(name, points);
	break;

	case FL_Member_Interface::MemberTypes::TRAPEZOID:
		//Allocate and set data
		m_Members[size] = new FL_Member_Trapezoid();
		m_Members[size]->SetMemberID((int)size);
		m_Members[size]->SetMemberTypeID(FL_Member_Interface::MemberTypes::TRAPEZOID);
		m_Members[size]->InitMember(name, points);
		break;
	}

	//Job done
	return true;
}

void FL_Set::CalcMembership(float inputValue)
{
	msg_assert(m_Members.size() > 0, "CalcMembership(): No members found!");
	//Holding container for highest member (set to first)

	//Solo calculate the first member and set as highest member automatically
	m_Members[0]->CalcMembershipValue(inputValue);
	FL_Member_Interface* highest = m_Members[0];

	//Start loop past the first
	for (unsigned i(1); i < m_Members.size(); ++i)
	{
		//Calc and keep the membership value
		float memValue = m_Members[i]->CalcMembershipValue(inputValue);
		//If the new value is higher than the currently held then replace the highest ptr
		if(memValue > highest->GetMembershipValue())
			highest = m_Members[i];
	}
}

void FL_Set::Reset()
{
	//Deallocate current members
	for (auto& a : m_Members)
	{
		delete a;
	}
	m_Members.clear();

	//Clear highest member ptr
	m_HighestMember = nullptr;
	//Reset name and id
	m_SetName = "NULL";
	m_SetID = -1;
}

void FL_Set::SyncMembersToSet()
{
	//Give each member their index in the array
	for (unsigned i(0); i < m_Members.size(); ++i)
		m_Members[i]->SetMemberID(i);
}

FL_Member_Interface* FL_Set::GetMemberByIndex(unsigned index)
{
	msg_assert(index < m_Members.size(), "GetMemberByIndex(): Index OOR!");
	return m_Members[index];
}
