//*********************************************************************************\\
// 
// Collection of Fuzzy Logic shape classes that can be used to different shapes
// for use in FL sets for modelling.
//
//*********************************************************************************\\

#pragma once

#include "FL_Member_Interface.h"	//Parent Interface Class

/*
	Triangle shaped Fuzzy Logic member. Uses 3 anchoring points to form scalene or
	equalateral triangles that define member's membership values.
*/
class FL_Member_Triangle : public FL_Member_Interface
{
public:

	////////////////////
	/// Constructors ///
	////////////////////

	FL_Member_Triangle() { }
	~FL_Member_Triangle() { }

	/////////////////
	/// Overrides ///
	/////////////////

	//Main init window, setting up the member with name and 3 values for each anchor point
	void InitMember(const std::string& name, std::vector<float>& values) override;
	//Alternate init where the triangle is created via range (forms equally spaced points)
	void InitMember(const std::string& name, float rangeStart, float rangeEnd);
	//Main calculation function (See interface)
	float CalcMembershipValue(float inputValue) override;
	//Calculate the range of value that the member covers (e.g. 1.f - 0.3f = 0.7f)
	void CalcValueRange() override { m_ValueRange = m_AnchoringPoints[TRI_SLOPE_DOWN_ANCHOR] - m_AnchoringPoints[TRI_SLOPE_UP_ANCHOR]; }
	
};

/*
	Trapezoid shaped Fuzzy Logic member. Uses 4 anchoring points to form regular, irregular or
	rect/box shapes to define the member's membership values.
*/
class FL_Member_Trapezoid : public FL_Member_Interface
{
public:

	////////////////////
	/// Constructors ///
	////////////////////

	FL_Member_Trapezoid() { }
	~FL_Member_Trapezoid() { }

	/////////////////
	/// Overrides ///
	/////////////////

	//Main init window, setting up the member with name and 3 values for each anchor point
	void InitMember(const std::string& name, std::vector<float>& values) override;
	//Main calculation function (See interface)
	float CalcMembershipValue(float inputValue) override;
	//Calculate the range of value that the member covers (e.g. 1.f - 0.3f = 0.7f)
	void CalcValueRange() override { m_ValueRange = m_AnchoringPoints[TRAP_SLOPE_DOWN_ANCHOR] - m_AnchoringPoints[TRAP_SLOPE_UP_ANCHOR]; }

};
