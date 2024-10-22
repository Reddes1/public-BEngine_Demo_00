//*********************************************************************************\\
// 
// Fuzzy Logic Set class that hoses a scalable amount of members that are used to 
// define a range of value relative the sets description and purpose.
//
//*********************************************************************************\\

#pragma once

#include "FL_Members.h"		//Member types + interface access

class FL_Set
{
public:

	//////////////////////////////
	/// Enums, Types & Statics ///
	//////////////////////////////

	////////////////////
	/// Constructors ///
	////////////////////
	
	FL_Set() { }
	~FL_Set() { Reset(); }

	//////////////////
	/// Operations ///
	//////////////////

	//Initialises the set with desired member count and allocates member type based on incoming vector of types
	//Sets the type and ID of the member relative to the set but member setup must be done individually via GetMemberByIndex()
	bool InitSet(const std::string& name, unsigned memberCount, std::vector<FL_Member_Interface::MemberTypes> types);

	//Alternate method that sets up the set member by member. Call this as many times as needed (will store the sets in order).
	//Call Reset() if re-using this set before starting this process.
	//Call SetName() if not already set, and SyncMembersToSet() to align member indexes.
	bool AddNewMember(const std::string& name, FL_Member_Interface::MemberTypes typeID, std::vector<float>& points);

	//Takes the given value and calculates its membership value for each member.
	//The highest member is accessable via GetHighestMember() call.
	//In the event of a tie in membership value, the first highest seen is kept (each member stores its membership value
	//after calc so more elaborate work can be done if needed).
	void CalcMembership(float inputValue);

	//Resets set complete (deallocations, null name and null set ID)
	void Reset();

	//Syncs members with the set properly
	void SyncMembersToSet();

	/////////////////
	/// Accessors ///
	/////////////////

	const std::string& GetSetName() { return m_SetName; }
	size_t GetMemberCount() { return m_Members.size(); }
	FL_Member_Interface* GetMemberByIndex(unsigned index);
	int GetSetID() { return m_SetID; }
	FL_Member_Interface* GetHighestMember() { return m_HighestMember; }

	void SetName(const std::string& name) { m_SetName = name; }
	void SetSetID(int id) { m_SetID = id; }

private:

	////////////
	/// Data ///
	////////////

	//Set Name (an object/subject that the set represents)
	std::string m_SetName = "N/A";
	//Container of member (allocated for as required)
	std::vector<FL_Member_Interface*> m_Members;
	//Current highest member (nullptr before any operations don)
	FL_Member_Interface* m_HighestMember = nullptr;
	//Set ID (Relative the controlling model)
	int m_SetID = -1;
};