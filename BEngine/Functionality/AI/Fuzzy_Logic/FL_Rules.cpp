#include "FL_Rules.h"

#include "Utils/Utils_Debug.h"

bool FL_Rules::InitRulesMap(unsigned keyDepth, unsigned keyLength)
{
	//Hold and check key depth validity
	unsigned keyD = keyDepth;
	if (keyD > 10)
	{
		msg_assert(false, "InitRulesMap(): Key Depth too large, will cap to max (10)");
		keyD = 9;
	}
	if (keyD == 0)
	{
		msg_assert(false, "InitRulesMap(): Key Depth too small, will set to minimum (1)");
		keyD = 1;
	}

	//Holding string and key value
	std::string str;
	unsigned key = 0;
	for (unsigned i(0); i < keyD; ++i)
		str += std::to_string(key++);

	//Reinterpret string as char array for algorithm
	char* charArr = &str[0];

	//Check if map is empty or not, and clear if not
	if (!m_RulesAndOutputsMap.empty())
		m_RulesAndOutputsMap.clear();
	
	//Run recursive algorithm to populate map
	GenerateKeyPermutations(charArr, keyD, keyLength);

	//Job Done
	return true;
}

bool FL_Rules::InitStateNames(const std::vector<std::string>& names)
{
	//Clear old array and copy over names
	m_StateNames.clear();
	m_StateNames = names;
	return true;
}

bool FL_Rules::InitIndividualRule(std::pair<std::string, int>& pair)
{
	m_RulesAndOutputsMap.insert(pair);
	return true;
}

bool FL_Rules::SetStateValueWithKey(const std::string& key, int stateValue)
{
	//Iterate and attempt to find rule entry
	std::unordered_map<std::string, int>::iterator ruleEntry = m_RulesAndOutputsMap.find(key);

	//If found, update the value and exit out true
	if (ruleEntry != m_RulesAndOutputsMap.end())
	{
		ruleEntry->second = stateValue;
		return true;
	}
	//Failed to find entry
	else
	{
		msg_assert(false, "SetStateValueWithKey(): Failed to find rule entry!, no changes made");
		return false;
	}

	//Shouldn't hit this
	assert(false);
	return false;
}

bool FL_Rules::SetMultipleStateValues(const std::vector<std::string>& keys, std::vector<int>& stateValues)
{
	msg_assert(keys.size() <= stateValues.size(), "SetMultipleStateValues(): Container mismatch, will cause crash!");

	//Hold returning flag (this will toggle and stay false if any key fails to change)
	bool retFlag = true;

	//Cycle container and update each entry
	for (unsigned i(0); i < keys.size(); ++i)
		retFlag = (retFlag && false) || SetStateValueWithKey(keys[i], stateValues[i]);

	return retFlag;
}

bool FL_Rules::Reset()
{
	//Clear containers
	Release();

	//Reset names and state
	m_RulesetName = "N/A";
	m_CurrentRuleKey = "N/A";
	m_CurrentActiveState = -1;

	//Job done
	return true;
}

int FL_Rules::UpdateToNewState(std::string& key)
{
	//Iterate and attempt to find rule entry
	std::unordered_map<std::string, int>::iterator ruleEntry = m_RulesAndOutputsMap.find(key);

	//If found, update current state information and return state value
	if (ruleEntry != m_RulesAndOutputsMap.end())
	{
		m_CurrentRuleKey = key;
		m_CurrentActiveState = ruleEntry->second;
		return m_CurrentActiveState;
	}
	//No entry found so return appropriate code
	else
	{
		msg_assert(false, "UpdateToNewState(): Failed to update to new state! No changes made");
		return FL_RM_NO_RULE_FOUND;
	}

	//Shouldn't hit this
	assert(false);
	return false;
}

void FL_Rules::Release()
{
	//Clear up containers
	m_RulesAndOutputsMap.clear();
	m_StateNames.clear();
}

bool FL_Rules::GenerateKeyPermutations(char* str, unsigned keyDepth, unsigned keyLength)
{
	//Create temp holding array for use in generating key permutations
	char* tempArr = new char[keyLength];
	//Bookend the string with termination symbol
	tempArr[keyLength] = '\0';

	//Start recusive process
	R_StorePermutation(str, tempArr, keyDepth, keyLength, 0);

	//Clear up
	delete[] tempArr;

	//Job done
	return true;
}

void FL_Rules::R_StorePermutation(char* str, char* tempStr, unsigned keyDepth, unsigned keyLength, unsigned index)
{
	for (unsigned i(0); i < keyDepth; ++i)
	{
		tempStr[index] = str[i];

		if (index == keyLength - 1)
			m_RulesAndOutputsMap.insert(std::pair<std::string, int>(tempStr, (int)FL_RM_UNDEFINED));
		else
			R_StorePermutation(str, tempStr, keyDepth, keyLength, index + 1);
	}
}
