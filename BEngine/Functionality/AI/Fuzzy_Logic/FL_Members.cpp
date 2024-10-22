#include "FL_Members.h"

#include "Utils/Utils_Debug.h"

////////////////
/// Triangle ///
////////////////

void FL_Member_Triangle::InitMember(const std::string& name, std::vector<float>& values)
{
	//Error checking
	msg_assert(values.size() >= TRI_COUNT, "InitMember(): Not enough values to initialise");

	//Store name
	m_MemberName = name;

	//Store triangle members (only first 3 members)
	for (unsigned i(0); i < TRI_COUNT; ++i)
		m_AnchoringPoints[i] = values[i];

	//Error check that the values do in fact form a triangle (that the mid point is not greater than the last point
	//or less than the first point
	msg_assert(m_AnchoringPoints[TRI_PEAK_ANCHOR] >= m_AnchoringPoints[TRI_SLOPE_UP_ANCHOR], "InitMember(): Midpoint invalid! Is less than first anchor!");
	msg_assert(m_AnchoringPoints[TRI_PEAK_ANCHOR] <= m_AnchoringPoints[TRI_SLOPE_DOWN_ANCHOR], "InitMember(): Midpoint invalid! Is greater than final anchor!");

	//Calculate the value range for this member
	CalcValueRange();
}

void FL_Member_Triangle::InitMember(const std::string& name, float rangeStart, float rangeEnd)
{
	//Set name
	m_MemberName = name;

	//Set start and end anchors
	m_AnchoringPoints[TRI_SLOPE_UP_ANCHOR] = rangeStart;
	m_AnchoringPoints[TRI_SLOPE_DOWN_ANCHOR] = rangeEnd;

	//Set middle anchor (middle point between them)
	m_AnchoringPoints[TRI_PEAK_ANCHOR] = (rangeEnd + rangeStart) * 0.5f;
}

float FL_Member_Triangle::CalcMembershipValue(float inputValue)
{
	//Determine where the value lies (or doesn't) on the shape, then calculate, store and then return that membership value
	
	//Check if the value lies on the left slope (anypoint between the anchors) 
	if (inputValue >= m_AnchoringPoints[TRI_SLOPE_UP_ANCHOR] && inputValue <= m_AnchoringPoints[TRI_PEAK_ANCHOR])
		return m_MembershipValue = (inputValue - m_AnchoringPoints[TRI_SLOPE_UP_ANCHOR]) / (m_AnchoringPoints[TRI_PEAK_ANCHOR] - m_AnchoringPoints[TRI_SLOPE_UP_ANCHOR]);
	//Check if the value lies on the right/downward slope (anypoint between the anchors)
	else if (inputValue >= m_AnchoringPoints[TRI_PEAK_ANCHOR] && inputValue <= m_AnchoringPoints[TRI_SLOPE_DOWN_ANCHOR])
		return m_MembershipValue = (m_AnchoringPoints[TRI_SLOPE_DOWN_ANCHOR] - inputValue) / (m_AnchoringPoints[TRI_SLOPE_DOWN_ANCHOR] - m_AnchoringPoints[TRI_PEAK_ANCHOR]);
	//The value must be outside of the value range so return 0.f
	else
		return m_MembershipValue = 0.f;

	//Should not reach this area
	assert(false);
	return 0.0f;
}

/////////////////
/// Trapezoid ///
/////////////////

void FL_Member_Trapezoid::InitMember(const std::string& name, std::vector<float>& values)
{
	//Error checking
	msg_assert(values.size() >= TRAP_COUNT, "InitMember(): Not enough values to initialise");

	//Store trapezoid members
	for (unsigned i(0); i < TRAP_COUNT; ++i)
		m_AnchoringPoints[i] = values[i];

	//Check that the slope anchors are properly positioned relative to the plataeu anchors
	msg_assert(m_AnchoringPoints[TRAP_SLOPE_UP_ANCHOR] <= m_AnchoringPoints[TRAP_PLATEAU_START_ANCHOR], "InitMember(): Slope up anchor invalid!");
	msg_assert(m_AnchoringPoints[TRAP_SLOPE_DOWN_ANCHOR] >= m_AnchoringPoints[TRAP_PLATEAU_END_ANCHOR], "InitMember(): Slope down anchor invalid!");
	//Check that the plataeu points are ordered correctly (can be practically overlapping but not criss-crossing)
	msg_assert(m_AnchoringPoints[TRAP_PLATEAU_START_ANCHOR] <= m_AnchoringPoints[TRAP_PLATEAU_END_ANCHOR], "InitMember(): Plateau anchors are invalid!");
}

float FL_Member_Trapezoid::CalcMembershipValue(float inputValue)
{
	//Determine where the value lies (or doesn't) on the shape, then calculate, store and then return that membership value
	
	//Check if the value is on the lies on the left slope
	if (inputValue >= m_AnchoringPoints[TRAP_SLOPE_UP_ANCHOR] && inputValue <= m_AnchoringPoints[TRAP_PLATEAU_START_ANCHOR])
	{
		//There is an edge case/feature where slope anchors can be equal to the adjacent plateau (to produce squares, rect, right-angle sides etc)
		//This can cause divide by zero errors so look for this and return 1.f if spotted
		if (m_AnchoringPoints[TRAP_SLOPE_UP_ANCHOR] && m_AnchoringPoints[TRAP_PLATEAU_START_ANCHOR])
			return m_MembershipValue = 1.f;
		//There is some kind of value difference on the slope, so run the normal calculation, store and return of membership value
		else
			return m_MembershipValue = (inputValue - m_AnchoringPoints[TRAP_SLOPE_UP_ANCHOR]) / (m_AnchoringPoints[TRAP_PLATEAU_START_ANCHOR] - m_AnchoringPoints[TRAP_SLOPE_UP_ANCHOR]);
	}
	//Check if the value lies on the plateau
	else if (inputValue > m_AnchoringPoints[TRAP_PLATEAU_START_ANCHOR] && inputValue < m_AnchoringPoints[TRAP_PLATEAU_END_ANCHOR])
		return m_MembershipValue = 1.f;
	//Check if the value lies on the plateau
	else if (inputValue >= m_AnchoringPoints[TRAP_PLATEAU_END_ANCHOR] && inputValue <= m_AnchoringPoints[TRAP_SLOPE_DOWN_ANCHOR])
		return m_MembershipValue = (m_AnchoringPoints[TRAP_SLOPE_DOWN_ANCHOR] - inputValue) / (m_AnchoringPoints[TRAP_SLOPE_DOWN_ANCHOR] - m_AnchoringPoints[TRAP_PLATEAU_END_ANCHOR]);
	//The value must be outside of the value range so return 0.f
	else
		return m_MembershipValue = 0.f;

	//Should not reach this area
	assert(false);
	return 0.0f;
}
