//*********************************************************************************\\
// 
// Interface for operating several different types of FL members.
// Provides common behaviours for several derived member types.
//
//*********************************************************************************\\

#pragma once

#include <string>
#include <vector>

class FL_Member_Interface
{
public:

	//////////////////////////////
	/// Enums, Types & Statics ///
	//////////////////////////////

	enum class MemberTypes : int
	{
		UNDEFINED = -1,
		TRIANGLE,
		TRAPEZOID		
	};
	static const unsigned MAX_ANCHORS = 4;

	//References for accessing appropriate indexes in derived classes and how they relate to the shape
	enum AnchorDefintions : unsigned
	{
		//
		//Triangle
		//

		TRI_SLOPE_UP_ANCHOR = 0,
		TRI_PEAK_ANCHOR = 1,
		TRI_SLOPE_DOWN_ANCHOR = 2,
		TRI_COUNT = 3,

		//
		//Trapezoid
		//

		TRAP_SLOPE_UP_ANCHOR = 0,
		TRAP_PLATEAU_START_ANCHOR = 1,
		TRAP_PLATEAU_END_ANCHOR = 2,
		TRAP_SLOPE_DOWN_ANCHOR = 3,
		TRAP_COUNT = 4
	};

	////////////////////
	/// Constructors ///
	////////////////////

	FL_Member_Interface() {}
	virtual ~FL_Member_Interface() {}

	////////////////
	/// Virtuals ///
	////////////////

	//Initialises the member with values, in required manner. Must be defined by derived class.
	virtual void InitMember(const std::string& name, std::vector<float>& values) = 0;
	//Calculates the membership value in accordance to the implementation, storing it and returning it. Must be defined by derived class.
	//This should be called by the owning FL_Set when determining a values level of membership (alongside any other members in the set) 
	virtual float CalcMembershipValue(float inputValue) = 0;
	//Calculates the range of value that the member covers. Must be defined by derived class.
	virtual void CalcValueRange() = 0;

	//////////////////
	/// Operations ///
	//////////////////

	/////////////////
	/// Accessors ///
	/////////////////

	void SetMemberName(const std::string& name) { m_MemberName = name; }
	void SetMemberID(int id) { m_MemberID = id; }
	void SetMemberTypeID(MemberTypes typeID);

	const std::string& GetMemberName() { return m_MemberName; }
	int GetMemberID() { return m_MemberID; }
	MemberTypes GetMemberTypeID() { return m_MemberTypeID; }

	float GetValueRange() { return m_ValueRange; }
	float GetMembershipValue() { return m_MembershipValue; }
	float GetAnchorPointValueAtIndex(unsigned index);

protected:

	//////////////////
	/// Operations ///
	//////////////////

	////////////
	/// Data ///
	////////////

	//Members name (a category/description of the set this belongs to)
	std::string m_MemberName = "N/A";

	//Anchoring points for the members shape (3 points = triangle, 4 = trapezoid etc)
	//Limited to 4 for now (may expand if new shapes/method require)
	float m_AnchoringPoints[MAX_ANCHORS] = {0.f};
	//What range of values this member represents
	float m_ValueRange = 0.f;
	//Holds the last calculated membership value
	float m_MembershipValue = 0.f;
	//Member ID (where it is in the fuzzy set)
	int m_MemberID = -1;
	//What type of member is this (the shape of it)
	MemberTypes m_MemberTypeID = MemberTypes::UNDEFINED;
};