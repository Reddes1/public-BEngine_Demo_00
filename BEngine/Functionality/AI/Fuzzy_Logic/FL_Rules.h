//*********************************************************************************\\
// 
// Rules module for use with the fuzzy logic AI technique.This is the main container
// that defines what the set is about, and what keys are associated with it.
// Rules are generated via permutation function that creates X* Y string keys that
// can then be associated with output type.
// 
// Does not support sub-length rules (-0-, 12- type rule entries by default), NYI feature.
// Can manually insert rule entries however but MUST use the '-' character to
// mesh with other FL modules.
//
//*********************************************************************************\\

#pragma once

#include <unordered_map>	//Rule permutation container + vector type

class FL_Rules
{
public:

	//////////////////////////////
	/// Enums, Types & Statics ///
	//////////////////////////////

	//Signal 
	enum FL_RuleCodes : int
	{
		FL_RM_UNDEFINED = -1,
		FL_RM_NO_RULE_FOUND = -2
	};

	////////////////////
	/// Constructers ///
	////////////////////

	FL_Rules() {}	
	//Alt call that initialises the rules container with given depth and length (See InitRulesMap for more details)
	FL_Rules(unsigned keyDepth, unsigned keyLength) {	InitRulesMap(keyDepth, keyLength); }

	~FL_Rules() { Release(); }

	//////////////////
	/// Operations ///
	//////////////////

	//
	//Setup
	//
	
	/*
		Initialises the rules container via permutation algorithm.
		Key depth: the number of states that an input can be (0-9 range, max 10 states)
		Key length: the number of states categories ("321" = 3 state inputs, "43452" = 5 state inputs)
		Will reinitialise the container if called again, clearing previous configuations
		Outputs will be initialised to -1 (which can be used as an undefined rule signal)
	*/
	bool InitRulesMap(unsigned keyDepth, unsigned keyLength);
	//Initialises names vector with vector of names relating to states this module works with
	bool InitStateNames(const std::vector<std::string>& names);
	
	//Initialises an individual rule. Will override any entry with the same key.
	bool InitIndividualRule(std::pair<std::string, int>& pair);
	
	//Sets the value of a state with the given key. Returns false if no rule found.
	bool SetStateValueWithKey(const std::string& key, int stateValue);
	//Sets multiple states at once using given key vector. Signals in debug on individual failures to find keys, and returns false if any fail to find.
	bool SetMultipleStateValues(const std::vector<std::string>& keys, std::vector<int>& stateValues);

	//Releases and resets the class
	bool Reset();

	//
	//Operations
	//

	//Returns the state code associated with a given rule string. See FL_RulesModuleCodes for return types in case of invalid/undefined behaviour
	int UpdateToNewState(std::string& key);

	/////////////////
	/// Accessors ///
	/////////////////

	void SetRulesetName(const std::string& name) { m_RulesetName = name; }

	const std::string& GetRulesetName() { return m_RulesetName; }
	int GetActiveState() { return m_CurrentActiveState; }
	const std::string& GetActiveStateName() { return m_StateNames[m_CurrentActiveState]; }
	const std::string& GetActiveStateRuleKey() { return m_CurrentRuleKey; }

private:

	//////////////////
	/// Operations ///
	//////////////////

	//Clean up
	void Release();

	//
	//Setup
	//
		
	bool GenerateKeyPermutations(char* str, unsigned keyDepth, unsigned keyLength);
	void R_StorePermutation(char* str, char* tempStr, unsigned keyDepth, unsigned keyLength, unsigned index);

	////////////
	/// Data ///
	////////////

	//Names for rule states (for display purposes, should mirror active state enum)
	std::vector<std::string> m_StateNames;
	//Map holding all rule combinations and paired output state
	std::unordered_map<std::string, int> m_RulesAndOutputsMap;

	//Ruleset name
	std::string m_RulesetName = "N/A";
	//The last used key to access rules table/current rule being acted on
	std::string m_CurrentRuleKey = "N/A";
	//The last determined state as a value (this should be defined via enum elsewhere)
	int m_CurrentActiveState = -1;
};